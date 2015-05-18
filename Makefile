#Adapted from http://stackoverflow.com/questions/2481269/how-to-make-simple-c-makefile
CC=clang
CXX=clang++ -std=c++11
RM=rm -f
CPPFLAGS=-g
LDFLAGS=-g
LDLIBS=-lm -lboost_system -lboost_timer

SRCS= qset.cc test.cc NaiveMemo.cc BasicMemo.cc DepthBoundedMemo.cc AbstractMemo.cc
OBJS=$(subst .cc,.o,$(SRCS))

all: test


test: $(OBJS)
	$(CXX) $(LDFLAGS) -o test $(OBJS) $(LDLIBS)

depend: .depend

.depend: $(SRCS)
	rm -f ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)

dist-clean: clean
	$(RM) *~ .depend

check-syntax:
	$(CXX) $(CPPFLAGS) -o nul -S ${CHK_SOURCES}

include .depend


