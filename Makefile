CMPL=g++
FLAGS=-lgloox -std=c++11
TARGET=LEDPager
all:
	$(CMPL) LEDPager.cpp main.cpp $(FLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET)
