#include "LEDPager.cpp"
#include <reactor.h>
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
#define IN_PIN        4

using namespace std;
using namespace gloox;
static int do_exit = 0;
static int debug = 0;
static LEDPager* slack_pager = NULL;
static LEDPager* fb_pager   = NULL;
static reactor* pager_reactor = NULL;

void clear_pins()
{
  digitalWrite(FB_LED_PIN, LOW); 
  digitalWrite(SLACK_LED_PIN, LOW); 
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
  fb_pager = new LEDPager("yaakov.kuperman@chat.facebook.com", password, fbOnRecv, debug);
  std::cout << "finished connection to FB!" << std::endl;

  std::cout << "enter slack password:\n";
  std::getline(std::cin , password);
  std::cout << password << std::endl;
  slack_pager = new LEDPager("yaakov@msys.xmpp.slack.com", password, slackOnRecv, debug);
  std::cout << "finished connecting to slack!" << std::endl;
}

void teardown(int unused)
{
  clear_pins();
  /* this will tear the connection down nicely */
  delete slack_pager;
  delete fb_pager;
  pager_reactor->stop(pager_reactor, 0);
}

int read_clients(reactor *self)
{
  fb_pager->recv(50000); 
  slack_pager->recv(50000);
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
  pinMode(IN_PIN, INPUT);
  wiringPiISR (IN_PIN, INT_EDGE_BOTH, clear_pins);

  initLEDPager(debug);
  signal(SIGINT, teardown);
  pager_reactor =(reactor*) malloc (sizeof (reactor *));
  simple_reactor_new(pager_reactor, read_clients);
  /* this will block until the program exits */
  pager_reactor->start(pager_reactor);
  exit(0);
}

/* vim: et ts=2 sw=2
*/
