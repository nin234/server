CXX = g++

CPPFLAGS = -I. -g -std=c++17 -mcmodel=large -fpic -Wall -Werror
#CPPFLAGS = -I. -g -std=c++14

#LDFLAGS = -Wl,-rpath,/usr/local/lib64 -L/usr/local/lib64 -lssl -lcrypto -lpthread -lrt -Wl,-rpath,/usr/lib64/capn -L/usr/lib64/capn -lcapn -Wl,-rpath,/usr/local/lib  -L/usr/local/lib -lstrophe -lcurl
LDFLAGS = -lssl -lcrypto -lpthread -lrt -Wl,-rpath,/usr/lib64/capn -L/usr/lib64/capn -lcapn -Wl,-rpath,/usr/local/lib  -L/usr/local/lib -lstrophe -lcurl

objects := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

all: $(objects)
	$(CXX) -o sharesrv $(LDFLAGS) $(objects)

clean:
	rm -f *.o sharesrv
