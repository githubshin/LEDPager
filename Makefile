CMPL=g++-4.7
FLAGS=-lgloox -ldstructs -lreactor -lwiringPi -std=c++11 -g3 $(LP_CFLAGS)
TARGET=ledpager
INSTALL_DEST=/usr/local/bin

all: ledpager install

ledpager:
	$(CMPL) $(FLAGS) LEDPager.cpp main.cpp -o $(TARGET)

install:
	cp $(TARGET) $(INSTALL_DEST)

.PHONY: clean
clean:
	rm -f $(TARGET)
