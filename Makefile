target := monitor
sources := $(shell find . -name "main*.cpp")
objFiles1 := $(patsubst %.cpp, %.o, $(sources))

all: $(target)

$(target): $(objFiles)
    $(CXX) $(CXXFLAGS) -o $(target) $(objFiles1)

target2 := producer
sources2 := $(shell find . -name "producer*.cpp")
objFiles2 := $(patsubst %.cpp, %.o, $(sources2))

all: $(target2)

$(target2): $(objFiles2)
    $(CXX) $CXXFLAGS) -o $(target2) $(objFiles2)
    
target3 := consumer
sources3 := $(shell find . -name "consumer*.cpp")
objFiles3 := $(patsubst %.cpp, %.o, $(sources3))

all: $(target3)

$(target3): $(objFiles3)
    $(CXX) $(CXXFLAGS) -o $(target3) $(objFiles3)

clean:
    rm -f $(objFiles1)
    rm -f $(target1)
    rm -f $(objFiles2)
    rm -f $(target2)
    rm -f $(objFiles3)
    rm -f $(target3)
    rm -f logfile
