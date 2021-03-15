target1 := monitor
source1 := $(shell find . -name "monitor*.cpp") ./semaphores.cpp

LDLIBS := libmonitor.a
objFiles1  := $(patsubst %.cpp, %.o, $(source1))

all: $(target1)

$(target1): $(objFiles1) $(LDLIBS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(target1) $(objFiles1) $(LDLIBS)

$(LDLIBS): libmonitor.o
	ar $(ARFLAGS) $@ $^

target2 := producer
source2 := $(shell find . -name "producer*.cpp") ./semaphores.cpp
objFiles2  := $(patsubst %.cpp, %.o, $(source2))

all: $(target2)

$(target2): $(objFiles2)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(target2) $(objFiles2) $(LDLIBS)

# App 3 - builds the consumer program
target3 := consumer
source3 := $(shell find . -name "consumer*.cpp") ./semaphores.cpp
objFiles3  := $(patsubst %.cpp, %.o, $(source3))

all: $(target3)

$(target3): $(objFiles3)
	$(CXX) $(CXXLAGS) $(LDFLAGS) -o $(target3) $(objFiles3) $(LDLIBS)

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
