CC=g++
#CC=clang++ -fcolor-diagnostics
#OPT_FLAGS=-O3
OPT_FLAGS=-g 
#SANITIZE_FLAGS=-fsanitize=undefined -fno-sanitize=object-size -fsanitize=unsigned-integer-overflow -fsanitize=address 

WARN_FLAGS=-Wall -Wextra -Wno-switch -Wno-unused-parameter -Wno-unused-private-field

SDL_LDFLAGS=`sdl-config --libs` 
SDL_CFLAGS=`sdl-config --cflags` 
CFLAGS=$(SDL_CFLAGS) $(OPT_FLAGS) $(WARN_FLAGS) $(SANITIZE_FLAGS)
LDFLAGS=-lGL -lGLU -lSDL_image $(SDL_LDFLAGS) -lboost_program_options
INCLUDES=-Ibwapidata/include -Isource/glfont
SOURCES=$(wildcard bwapidata/include/*.cpp) $(wildcard source/*.cpp)
GLFONTSOURCE=$(wildcard source/glfont/*.cc)
OBJECTS=$(GLFONTSOURCE:.cc=.o) $(SOURCES:.cpp=.o)


all:SparCraft LibSparCraft

SparCraft:$(OBJECTS)
	$(CC) $(OBJECTS) -o $@  $(LDFLAGS) $(CFLAGS)

LibSparCraft:$(filter-out source/main.o,$(OBJECTS))
	ar rcs libSparCraft.a $(filter-out source/main.o,$(OBJECTS))

.cpp.o:
	$(CC) -MM $(CFLAGS) $(INCLUDES) -MT $@ -o $*.d $<
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@ 

.cc.o:
	$(CC) -MM $(CFLAGS) $(INCLUDES) -MT $@ -o $*.d $<
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@

.PHONY: clean doc

clean:
	rm -f $(OBJECTS) $(OBJECTS:.o=.d) SparCraft libSparCraft.a

doc:
	doxygen Doxyfile

-include $(GLFONTSOURCE:.cc=.d) $(SOURCES:.cpp=.d)

