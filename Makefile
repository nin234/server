CXX = g++

CPPFLAGS = -I. -g -std=c++14 -mcmodel=large -fpic -Wall -Werror

LDFLAGS = -lpthread -lrt -L/usr/lib64/capn -lcapn -lstrophe

objects := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

all: $(objects)
	$(CXX) -o sharesrv $(LDFLAGS) $(objects)

clean:
	rm -f *.o sharesrv
