TARGET = testdiff diffconfig
CFLAGS = -Wall -g
GTEST = -DGOOGLE_TEST `pkg-config --cflags --libs gtest`
CPPFLAGS = -Wall -std=c++11

all:$(TARGET)

diffconfig.o:diffconfig.cpp diffconfig.hpp
	g++ $(CPPFLAGS) -c -o $@ $< -DGOOGLE_TEST

diffconfig:diffconfig.cpp diffconfig.hpp
	g++ $(CFLAGS) -o $@ $<

testdiff:testdiff.cpp diffconfig.o
	g++ $(CPPFLAGS) -o $@ $^ $(GTEST)

clean:
	rm -rf $(TARGET) *.o

