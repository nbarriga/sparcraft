CC=g++ -g -rdynamic
#CC=g++ -O3
SDL_LDFLAGS=`sdl-config --libs` 
SDL_CFLAGS=`sdl-config --cflags` 
CFLAGS=$(SDL_CFLAGS)
LDFLAGS=-lGL -lGLU -lSDL_image $(SDL_LDFLAGS) -lboost_program_options
GALIB_DIR=source/galib
INCLUDES=-Ibwapidata/include -Isource/glfont -I$(GALIB_DIR)
SOURCES=$(wildcard bwapidata/include/*.cpp) $(wildcard source/*.cpp)
GLFONTSOURCE=$(wildcard source/glfont/*.cc)
OBJECTS=$(GLFONTSOURCE:.cc=.o) $(SOURCES:.cpp=.o)


all:SparCraft 

SparCraft:$(OBJECTS) galib
	$(CC) $(OBJECTS) -o $@  $(LDFLAGS) -L$(GALIB_DIR)/ga -lga

.cpp.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@ 
	$(CC) -MM $(CPPFLAGS) $(INCLUDES) -MT $@ -o $*.d $<

.cc.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@
	$(CC) -MM $(CPPFLAGS) $(INCLUDES) -MT $@ -o $*.d $<

galib:
	make -C $(GALIB_DIR) lib

clean:
	rm -f $(OBJECTS) $(OBJECTS:.o=.d) SparCraft

-include $(GLFONTSOURCE:.cc=.d) $(SOURCES:.cpp=.d)

