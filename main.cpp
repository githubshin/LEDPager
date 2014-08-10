#include "LEDPager.cpp"
#include <iostream>
#include <string>
#include <unistd.h>

using namespace std;
using namespace gloox;
int main(int argc, char **argv)
{
  std::cout << "enter password:\n";
  std::string password;
  std::getline(std::cin , password);
  int debug = 0;
  if (argc > 1) {
    debug = 1;
  }
  LEDPager* l = new LEDPager("yaakov.kuperman@chat.facebook.com", password, debug);
  while (1) {
    sleep(1);
  };
}

