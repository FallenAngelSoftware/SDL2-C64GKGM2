# "C64GKGM2[TM]" - MAKEFILE by JeZ+Lee & mattmatteh...
# (C)opyright 2019 By Team www.FallenAngelSoftware.com

TARGET = c64gkgm2

VERSION = 0.1

DEL_FILE = rm -f

CC      = g++
CFLAGS = -pipe -Wall -g #-"ggdb"
SDL_CFLAGS = $(shell sdl2-config --cflags)
SDL_LIBS = $(shell sdl2-config --libs) 

OBJECTS = src/main.o \
          src/audio.o \
          src/input.o \
          src/interface.o \
          src/logicCode.o \
          src/logicSprite.o \
          src/screens.o \
          src/visuals.o

SOURCES = src/main.cpp \
          src/audio.cpp \
          src/input.cpp \
          src/interface.cpp \
          src/logicCode.cpp \
          src/logicSprite \
          src/screens.cpp \
          src/visuals.cpp

HEADERS = src/audio.h \
          src/input.h \
          src/interface.h \
          src/logicCode \
          src/logicSprite \
          src/screens.h \
          src/visuals.h

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(SDL_LIBS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $(SDL_CFLAGS) -c $< -o $@

clean:
	rm $(OBJECTS) $(TARGET)

