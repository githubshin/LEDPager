CMPL=g++-4.7
FLAGS=-lgloox -lwiringPi -std=c++11 -Wall -g3
TARGET=LEDPager
all:
	$(CMPL) LEDPager.cpp main.cpp $(FLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET)
