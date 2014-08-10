#include "LEDPager.cpp"
#include <wiringPi.h>
#include <iostream>
#include <string>
#include <stdint.h>
#include <signal.h>
#include <unistd.h>

#define MY_LED_PIN 3
#define MY_IN_PIN 4

using namespace std;
using namespace gloox;
static int do_exit = 0;
void callback()
{
  digitalWrite(MY_LED_PIN, LOW); 
}

void myOnRecv()
{
  digitalWrite(MY_LED_PIN, HIGH);
}

void initLEDPager(int debug)
{
  std::cout << "enter password:\n";
  std::string password;
  std::getline(std::cin , password);
  LEDPager* l = new LEDPager("yaakov.kuperman@chat.facebook.com", password, myOnRecv, debug);
}

void teardown(int unused)
{
  do_exit = 1;
}
int main(int argc, char **argv)
{
  int debug = 0;
  if (argc > 1) {
    debug = 1;
  }
  wiringPiSetup();
  pinMode(MY_LED_PIN, OUTPUT);
  pinMode(MY_IN_PIN, INPUT);
  wiringPiISR (MY_IN_PIN, INT_EDGE_BOTH, callback);

  initLEDPager(debug);
  signal(SIGINT, teardown);
  while (do_exit == 0) {};
  digitalWrite(MY_LED_PIN, LOW);
  exit(0);
}

