CXX=g++
CXXFLAGS=
LDFLAGS=
ifeq ($(DEBUG),)
	CXXFLAGS=-O3
else
	ifeq ($(PROFILE),)
		CXXFLAGS=-ggdb -DDEBUG
	else
		CXXFLAGS=-pg
	endif
endif

ifneq ($(BOOST_ROOT),)
	CXXFLAGS+=-I$(BOOST_ROOT)/include
	LDFLAGS+=-L$(BOOST_ROOT)/lib -Wl,-rpath=$(BOOST_ROOT)/lib
endif

CXXFLAGS+=-Wall -Wextra --std=gnu++0x
LDFLAGS+=-lboost_system -lboost_chrono -lboost_timer -lboost_thread -Wl,-rpath=/usr/local

opt: opt.o
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(LIBS) -o $@ $^

all: opt geninput tourney_test

geninput: geninput.o
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(LIBS) -o $@ $^

tourney_test: tourney_test.o
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(LIBS) -o $@ $^

tourneytime: tourneytime.o
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(LIBS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

opt.o: opt.cpp assignment.h io.h objective.h parallel.h random.h solve.h types.h

geninput.o: geninput.cpp

tourney_test.o: tourney_test.cpp tourney.h

tourneytime.o: tourneytime.cpp tourney.h speedtest.h

clean:
	$(RM) opt test geninput opt.o random.o test.o geninput.o tourney_test.o
