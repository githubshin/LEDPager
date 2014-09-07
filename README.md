LEDPager
========

some gloox code to enable getting LEDs to go off on a pi when messages come in

ROADMAP
=======

milestone 0.1 - clean, sane, OO design
  [ ] - abstract the clients and pins to the point where I don't need to essentially c&p the code to mess with them (thinking that means that a lot of what we're doing in main.cpp might be a wrapper around the 3 clients)
  [ ] - make separate events for all the clients instead of lumping them all together.
  [ ] - make the recon event get scheduled in the callback - when a message comes in that should reset the timer to be N minutes after that message
  [ ] - abstract the gloox handlers to not be in the LEDPager class (for code coherency)

milestone 0.2 - flexible and easy to grok logging
  [ ] - improve logging so it's more parseable
  [ ] - get the printing part of this project to happen entirely in the callbacks (that means passing message data as a callback arg
  [ ] - implement a logging module so that we're not just bleeting to stderr

KNOWN ISSUES
============

* slack gives me crap about making a connection
