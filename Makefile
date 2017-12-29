MAINSRC = GradeMaster.cpp
COMPONENTS = BitVec GraderInfo StudentInfo AssignmentInfo ConfigInfo
COMPONENTS += ExtraUtil Interface
COMPONENTSSRC = $(COMPONENTS:%=%.cpp)
COMPONENTSOBJ = $(COMPONENTS:%=%.o)
COMPILESRC = $(MAINSRC) $(COMPONENTSSRC)
COMPILEOBJ = $(COMPILESRC:%.cpp=%.o)


COMPILER = g++ -c
INC =
DEF = -D OVERRIDE_PRINTF
COMPILERFLAGS = -Wall -Werror -ansi -pedantic -std=gnu++14 $(INC) $(DEF)
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

.PHONY: clean spotless
