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
LDFLAGS+=-lboost_system -lboost_chrono -lboost_timer -lboost_thread -lboost_regex

opt: opt.o
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(LIBS) -o $@ $^

all: opt geninput tourney_test tourneytime

geninput: geninput.o
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(LIBS) -o $@ $^

stat: stat.o
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(LIBS) -o $@ $^

tourney_test: tourney_test.o
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(LIBS) -o $@ $^

tourneytime: tourneytime.o
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(LIBS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

opt.o: opt.cpp assignment.h csv.h io.h objective.h random.h types.h

solve_bridge.o: solve_bridge.cpp assignment.h csv.h io.h objective.h parallel.h random.h solve.h tourney.h types.h

geninput.o: geninput.cpp

tourney_test.o: tourney_test.cpp tourney.h

tourneytime.o: tourneytime.cpp tourney.h speedtest.h

stat.o: stat.cpp stat.h

clean:
	$(RM) opt test geninput tourney_test tourneytime \
		opt.o solve_bridge.o random.o test.o geninput.o tourney_test.o tourneytime.o \
		personer.aux personer.log personer.pdf prioriteter.aux prioriteter.log prioriteter.pdf \
		opt-*.csv
