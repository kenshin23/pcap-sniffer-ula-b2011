COMPILER = g++
CCFLAGS = -g
graph : graph.o
	${COMPILER} ${CCFLAGS} -o graph	

graph.o : graph.cpp
	${COMPILER} ${CCFLAGS} -c graph.cpp

all : graph
				
clean:
	rm -f *.o
	rm -f *~
	rm -f ${target}
