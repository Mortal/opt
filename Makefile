CXX=g++
CXXFLAGS=-O3 -Wall -Wextra

opt: opt.o random.o
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(LIBS) -o $@ $^

all: opt test geninput

test: test.o random.o
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(LIBS) -o $@ $^

geninput: geninput.o
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(LIBS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

opt.o: opt.cpp types.h rate.h solve.h

rate.o: rate.cpp rate.h types.h

solve.o: solve.cpp solve.h types.h

test.o: test.cpp solve.h types.h

clean:
	$(RM) opt test geninput opt.o random.o test.o geninput.o
