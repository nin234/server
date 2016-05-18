CXX = g++

CPPFLAGS = -I. -std=c++14 -mcmodel=large -fpic -Wall

LDFLAGS = -lpthread -lrt -L/usr/lib64/capn -lcapn

objects := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

all: $(objects)
	$(CXX) -o sharesrv $(LDFLAGS) $(objects)

#$(objects): %.cpp
	#$(CC) -c $(CFLAGS) $< $@
clean:
	rm -f *.o sharesrv
