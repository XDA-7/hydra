OBJS = main.cpp

INCLUDE_PATHS = -Iinclude -Isrc/*

LIBRARY_PATHS = -Llib

all : $(OBJS)
	g++ $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) -Wall -g -lmingw32 -lSDL2main -lSDL2 -o main