CXX:=g++
CC:=gcc
LIBRARY_PATH=$(shell echo "/usr/lib/$(gcc -print-multiarch)")
C_INCLUDE_PATH=$(shell echo "/usr/include/$(gcc -print-multiarch)")
CPLUS_INCLUDE_PATH=$(shell echo "/usr/include/$(gcc -print-multiarch)")
LDFLAGS := -L/home/osada/progs/gtest-1.7.0/lib/.libs/ -Wl,-rpath -Wl,/home/osada/progs/gtest-1.7.0/lib/.libs/ -lgtest -lpthread \
		   -Wl,-rpath -Wl,/usr/local/lib
LD = g++
CFLAGS = -g -Wall -I./
CPPFLAGS = -g -O0 -I./ -w -fpermissive -fno-inline-functions --std=c++0x -I/home/osada/progs/boost_1_55_0 -I/home/osada/progs/gtest-1.7.0/include \
		   
RM = /bin/rm -f
SRCS = $(wildcard *.cpp) $(wildcard tests/*.cpp)  
OBJS = $(patsubst %.cpp,%.o,$(patsubst %.c,%.o ,$(SRCS)))
DEPS = $(OBJS:.o=.d)
PROG = tests_main
-include $(DEPS)

all: $(PROG)

$(PROG): $(OBJS)
	$(LD) $(OBJS) -o $(PROG) $(LDFLAGS)

%.o: %.cpp
	$(CXX) -c $(CPPFLAGS) $< -o $*.o
	$(CXX) -MM $(CPPFLAGS) $< > $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp
	
%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $*.o
	$(CC) -MM $(CFLAGS) $< > $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp

tests: $(PROG)
	./$(PROG)

clean:
	$(RM) $(PROG) $(OBJS) $(DEPS)
