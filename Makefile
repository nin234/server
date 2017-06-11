CXX = g++

CPPFLAGS = -I. -g -std=c++14 -mcmodel=large -fpic -Wall -Werror
#CPPFLAGS = -I. -g -std=c++14

LDFLAGS = -lpthread -lrt -L/usr/lib64/capn -lcapn -Wl,-rpath=/usr/local/lib  -L/usr/local/lib -lstrophe

objects := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

all: $(objects)
	$(CXX) -o sharesrv $(LDFLAGS) $(objects)

clean:
	rm -f *.o sharesrv
