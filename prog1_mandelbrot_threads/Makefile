CXX       = g++ -m64
DEBUG    ?= 0

BASE_CXXFLAGS = -I../common -Iobjs/ -std=c++11 -Wall -fPIC
RELEASE_FLAGS = -O3
DEBUG_FLAGS   = -Og -g -fno-omit-frame-pointer

CXXFLAGS = $(BASE_CXXFLAGS) $(if $(filter 1,$(DEBUG)),$(DEBUG_FLAGS),$(RELEASE_FLAGS))
LDFLAGS  = $(if $(filter 1,$(DEBUG)),-g,)

APP_NAME = mandelbrot
OBJDIR   = objs
COMMONDIR= ../common

PPM_CXX  = $(COMMONDIR)/ppm.cpp
PPM_OBJ  = $(addprefix $(OBJDIR)/,$(subst $(COMMONDIR)/,,$(PPM_CXX:.cpp=.o)))

default: $(APP_NAME)

.PHONY: dirs clean

dirs:
	/bin/mkdir -p $(OBJDIR)/

clean:
	/bin/rm -rf $(OBJDIR) *.ppm *~ $(APP_NAME)

OBJS = $(OBJDIR)/main.o $(OBJDIR)/mandelbrotSerial.o $(OBJDIR)/mandelbrotThread.o $(PPM_OBJ)

$(APP_NAME): dirs $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS) -lm -lpthread

$(OBJDIR)/%.o: %.cpp
	$(CXX) $< $(CXXFLAGS) -c -o $@

$(OBJDIR)/%.o: $(COMMONDIR)/%.cpp
	$(CXX) $< $(CXXFLAGS) -c -o $@

$(OBJDIR)/main.o: $(COMMONDIR)/CycleTimer.h

