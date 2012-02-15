CXX=g++
CXXFLAGS=-g -Wall -Wextra

all: opt

opt: opt.o rate.o
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $^

clean:
	$(RM) opt
