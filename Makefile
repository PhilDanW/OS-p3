CC := g++
CFLAGS := -Wall -g
target1 := monitor
source1 := $(shell find . -name "monitor*.cpp") ./semaphores.cpp

LDLIBS := monitor.a
objFiles1  := $(patsubst %.cpp, %.o, $(source1))

all: $(target1)

$(target1): $(objFiles1) $(LDLIBS)
	$(C) $(CFLAGS) $(LDFLAGS) -o $(target1) $(objFiles1) $(LDLIBS)

$(LDLIBS): monitor.o
	ar $(ARFLAGS) $@ $^

target2 := producer
source2 := $(shell find . -name "producer*.cpp") ./semaphores.cpp
objFiles2  := $(patsubst %.cpp, %.o, $(source2))

all: $(target2)

$(target2): $(objFiles2)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(appname2) $(objects2) $(LDLIBS)

# App 3 - builds the consumer program
target3 := consumer
source3 := $(shell find . -name "consumer*.cpp") ./semaphores.cpp
objFiles3  := $(patsubst %.cpp, %.o, $(srcfiles))

all: $(target3)

$(target3): $(objFiles3)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(target3) $(objFiles3) $(LDLIBS)

clean:
	rm -f $(objects1)
	rm -f $(appname1)
	rm -f $(objects2)
	rm -f $(appname2)
	rm -f $(objects3)
	rm -f $(appname3)
	rm -f libmonitor.a
	rm -f libmonitor.o
	rm -f logfile
