TARGET = main

all: $(TARGET)

$(TARGET): myjsontest.cc myjson.hpp 
	g++ -g -Wall -std=c++11 -o $@ $<

.PHONY: clean
clean:
	-rm $(TARGET)

.PHONY: test
test: all
	@./$(TARGET)