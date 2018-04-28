.DEFAULT_GOAL := all
SHELL = /bin/sh
CXX = g++
CXXFLAGS = -lX11 \
		   -lXtst \
		   -lopencv_core \
		   -lopencv_imgproc \
		   -lopencv_videoio \
		   -lopencv_objdetect \
		   -lopencv_highgui \
		   -std=c++11 \
		   -O2 \
		   -Wall \
		   -pedantic \
		   $(shell pkg-config --libs gtk+-2.0)
 
EXECUTABLE = handpointer
XORG_OBJ = xorg.o
PREFIX = /usr

OBJS = $(XORG_OBJ)

all: $(OBJS)
	$(CXX) $(OBJS) $(CXXFLAGS) -o $(EXECUTABLE) 

clean:
	rm -rf $(XORG_OBJ)

install:
	cp $(EXECUTABLE) $(PREFIX)/bin

.PHONY: all,clean,install
