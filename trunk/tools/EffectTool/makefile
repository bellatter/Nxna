COMPILER = g++
LINKER = g++
COMPILER_FLAGS = -std=c++11 -g
LINKER_FLAGS = 
EXECUTABLE = effecttool

SOURCES = main.cpp EffectXml.cpp EffectWriter.cpp EffectCompiler.cpp Utils.cpp XnbWriter.cpp FilePaths.cpp tinyxml2/tinyxml2.cpp
OBJECTS = $(SOURCES:.cpp=.o)

$(EXECUTABLE): $(OBJECTS)
	$(LINKER) $(LINKER_FLAGS) $(OBJECTS) -o $@
	
.cpp.o:
	$(COMPILER) $(COMPILER_FLAGS) -c $< -o $@
	
clean:
	rm $(OBJECTS)