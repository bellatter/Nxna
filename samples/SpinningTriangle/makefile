COMPILER = g++
LINKER = g++
COMPILER_FLAGS = --std=c++11 -I../ -I../../src
LINKER_FLAGS =
EXECUTABLE = spinningtriangle

SOURCES = main.cpp ../Common/X.cpp
OBJECTS = $(SOURCES:.cpp=.o)

$(EXECUTABLE): $(OBJECTS)
	$(LINKER) $(LINKER_FLAGS) $(OBJECTS) -o $@ -lGL -lX11
	
.cpp.o:
	$(COMPILER) $(COMPILER_FLAGS) -c $< -o $@
	
clean:
	rm $(OBJECTS)
