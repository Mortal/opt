CXX=g++
ifeq ($(DEBUG),)
	CXXFLAGS=-O3
else
	ifeq ($(PROFILE),)
		CXXFLAGS=-ggdb -DDEBUG
	else
		CXXFLAGS=-pg
	endif
endif
CXXFLAGS+=-Wall -Wextra --std=gnu++0x
LDFLAGS=-lboost_system -lboost_timer -lboost_thread

opt: opt.o
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(LIBS) -o $@ $^

all: opt test geninput

test: test.o random.o
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(LIBS) -o $@ $^

geninput: geninput.o
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(LIBS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

opt.o: opt.cpp types.h objective.h solve.h io.h assignment.h

geninput.o: geninput.cpp

test.o: test.cpp solve.h types.h random.h objective.h assignment.h

clean:
	$(RM) opt test geninput opt.o random.o test.o geninput.o
