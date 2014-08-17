CMPL=g++-4.7
FLAGS=-lgloox -lwiringPi -std=c++11 -Wall -g3
TARGET=ledpager
INSTALL_DEST=/usr/local/bin

all: ledpager install

ledpager:
	$(CMPL) LEDPager.cpp main.cpp $(FLAGS) -o $(TARGET)

install:
	cp $(TARGET) $(INSTALL_DEST)

.PHONY: clean
clean:
	rm -f $(TARGET)
