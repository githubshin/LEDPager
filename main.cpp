#include "LEDPager.cpp"
#include <reactor.h>
#include <string.h>
#include <simple_reactor.h>
#include <heap/heap.h>
#include <wiringPi.h>
#include <iostream>
#include <string>
#include <stdint.h>
#include <signal.h>
#include <unistd.h>

#define SLACK_LED_PIN 3
#define FB_LED_PIN    1
#define GTALK_LED_PIN 2
#define IN_PIN        4

using namespace std;
using namespace gloox;
static int do_exit = 0;
static int debug = 0;
static LEDPager* gtalk_pager = NULL;
static LEDPager* slack_pager = NULL;
static LEDPager* fb_pager   = NULL;
static reactor* pager_reactor = NULL;

void clear_pins()
{
  digitalWrite(FB_LED_PIN, LOW); 
  digitalWrite(SLACK_LED_PIN, LOW); 
  digitalWrite(GTALK_LED_PIN, LOW); 
}

void gtalkOnRecv()
{
  digitalWrite(GTALK_LED_PIN, HIGH);
}

void slackOnRecv()
{
  digitalWrite(SLACK_LED_PIN, HIGH);
}

void fbOnRecv()
{
  digitalWrite(FB_LED_PIN, HIGH);
}

void initLEDPager(int debug)
{
  std::string password;

  std::cout << "enter facebook password:\n";
  std::getline(std::cin , password);
  std::cout << password << std::endl;
  fb_pager = new LEDPager("yaakov.kuperman@chat.facebook.com", password, fbOnRecv, debug);
  std::cout << "finished connection to FB!" << std::endl;

  std::cout << "enter slack password:\n";
  std::getline(std::cin , password);
  std::cout << password << std::endl;
  slack_pager = new LEDPager("yaakov@msys.xmpp.slack.com", password, slackOnRecv, debug);
  std::cout << "finished connecting to slack!" << std::endl;

  std::cout << "enter gtalk password:\n";
  std::getline(std::cin , password);
  std::cout << password << std::endl;
  gtalk_pager = new LEDPager("ykuperman@gmail.com", password, gtalkOnRecv, debug);
  std::cout << "finished connecting to gmail!" << std::endl;
}

void teardown(int unused)
{
  clear_pins();
  /* this will tear the connection down nicely */
  delete slack_pager;
  delete fb_pager;
  delete gtalk_pager;
  pager_reactor->stop(pager_reactor, 0);
}

int reconnect_clients(event_handler *event)
{
  /* keeps the connections 'alive'.  I was seeing that FB would start ignoring connections
     after too long, so this will prevent that */
  fprintf (stderr, "reconnecting facebook\n");
  fb_pager->disconnect();
  fb_pager->connect(false);
  fprintf (stderr, "reconnected facebook!\n");

  fprintf (stderr, "reconnecting slack\n");
  slack_pager->disconnect();
  slack_pager->connect(false);
  fprintf (stderr, "reconnected slack!\n");

  fprintf (stderr, "reconnecting gtalk\n");
  gtalk_pager->disconnect();
  gtalk_pager->connect(false);
  fprintf (stderr, "reconnected gtalk!\n");
  return REACTOR_EVENT_RESCHEDULE;
}

int read_clients(reactor *self)
{
  fb_pager->recv(5000); 
  slack_pager->recv(5000);
  gtalk_pager->recv(5000);
  /* in the current implementation of simple_reactor, this retval does nothing*/
  if (debug) 
    std::cout << "recv'd" << std::endl;
  return 1;
}

int main(int argc, char **argv)
{
  if (argc > 1) {
    debug = 1;
  }
  wiringPiSetup();
  pinMode(SLACK_LED_PIN, OUTPUT);
  pinMode(FB_LED_PIN, OUTPUT);
  pinMode(GTALK_LED_PIN, OUTPUT);
  pinMode(IN_PIN, INPUT);
  wiringPiISR (IN_PIN, INT_EDGE_BOTH, clear_pins);

  initLEDPager(debug);
  signal(SIGINT, teardown);
  pager_reactor =(reactor*) malloc (sizeof (reactor *));
  simple_reactor_new(pager_reactor, read_clients);
  /* this will block until the program exits */
  event_handler* reconnect_event =(event_handler *) malloc (sizeof *reconnect_event);
  memset(reconnect_event, 0, sizeof *reconnect_event);
  reconnect_event->callback = reconnect_clients;
  reconnect_event->data = NULL;
  reconnect_event->data_destructor_func = NULL;
  pager_reactor->start_timer(pager_reactor, reconnect_event, 1800);

  /* starts event loop */
  pager_reactor->start(pager_reactor);
  exit(0);
}

/* vim: et ts=2 sw=2
*/
