MAINSRC = GradeMaster.cpp
COMPONENTS = BitVec GraderLogin
COMPONENTSSRC = ${COMPONENTS: %=%.c}
COMPONENTSOBJ = ${COMPONENTS: %=%.o}
COMPILESRC = ${MAINSRC} #TODO: add COMPONENTSRC when ready
COMPILEOBJ = ${MAINSRC: %.c=%.o}


COMPILER = g++ -c
COMPILERFLAGS = -Wall -Werror -ansi -pedantic -std=C++14
LINKER = g++ -o
LINKERFLAGS = -lm

EXE = GradeMaster

GradeMaster: ${COMPILESRC}
	$(COMPILER) $(COMPILESRC)
	$(LINKER) $(EXE) $(LINKERFLAGS) $(COMPILEOBJ)

clean:
	rm -f *.o

spotless: clean
	rm -f $(EXE)
