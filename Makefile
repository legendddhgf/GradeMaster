MAINSRC = GradeMaster.cpp
COMPONENTS = BitVec GraderInfo StudentInfo AssignmentInfo ConfigInfo
COMPONENTS = ExtraUtil
COMPONENTSSRC = $(COMPONENTS:%=%.cpp)
COMPONENTSOBJ = $(COMPONENTS:%=%.o)
COMPILESRC = $(MAINSRC) $(COMPONENTSSRC)
COMPILEOBJ = $(COMPILESRC:%.cpp=%.o)


COMPILER = g++ -c
COMPILERFLAGS = -Wall -Werror -ansi -pedantic -std=gnu++14
LINKER = g++ -o
LINKERFLAGS = -lm

EXE = GradeMaster

GradeMaster: $(COMPILESRC)
	$(COMPILER) $(COMPILERFLAGS) $(COMPILESRC)
	$(LINKER) $(EXE) $(LINKERFLAGS) $(COMPILEOBJ)

clean:
	rm -f *.o

spotless: clean
	rm -f $(EXE)
