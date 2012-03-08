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

all: opt test geninput tourney_test

test: test.o random.o
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(LIBS) -o $@ $^

geninput: geninput.o
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(LIBS) -o $@ $^

tourney_test: tourney_test.o
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(LIBS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

opt.o: opt.cpp assignment.h io.h objective.h parallel.h random.h solve.h types.h

geninput.o: geninput.cpp

test.o: test.cpp solve.h types.h random.h objective.h assignment.h

tourney_test.o: tourney_test.cpp tourney.h

clean:
	$(RM) opt test geninput opt.o random.o test.o geninput.o tourney_test.o
