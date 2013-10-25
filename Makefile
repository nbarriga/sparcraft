CC=g++ -g -rdynamic -Wall -Wno-switch
#CC=g++ -O3
SDL_LDFLAGS=`sdl-config --libs` 
SDL_CFLAGS=`sdl-config --cflags` 
CFLAGS=$(SDL_CFLAGS)
LDFLAGS=-lGL -lGLU -lSDL_image $(SDL_LDFLAGS) -lboost_program_options
INCLUDES=-Ibwapidata/include -Isource/glfont
SOURCES=$(wildcard bwapidata/include/*.cpp) $(wildcard source/*.cpp)
GLFONTSOURCE=$(wildcard source/glfont/*.cc)
OBJECTS=$(GLFONTSOURCE:.cc=.o) $(SOURCES:.cpp=.o)


all:SparCraft LibSparCraft

SparCraft:$(OBJECTS)
	$(CC) $(OBJECTS) -o $@  $(LDFLAGS)

LibSparCraft:$(filter-out source/main.o,$(OBJECTS))
	ar rcs libSparCraft.a $(filter-out source/main.o,$(OBJECTS))

.cpp.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@ 
	$(CC) -MM $(CPPFLAGS) $(INCLUDES) -MT $@ -o $*.d $<

.cc.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@
	$(CC) -MM $(CPPFLAGS) $(INCLUDES) -MT $@ -o $*.d $<

clean:
	rm -f $(OBJECTS) $(OBJECTS:.o=.d) SparCraft

-include $(GLFONTSOURCE:.cc=.d) $(SOURCES:.cpp=.d)

