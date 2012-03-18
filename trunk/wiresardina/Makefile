COMPILER = g++
CCFLAGS = -g
LINK	= -lpcap -lpthread
OBJECTS		= packetheader.o core.o flow.o pcapwrapper.o stdoutputreader.o graph.o
MAINFILE	= main2.cpp

ejecutable : $(MAINFILE) $(OBJECTS)
	${COMPILER} ${CCFLAGS} -Wall $(MAINFILE) $(OBJECTS) $(LINK) -o ejecutable

core.o: core.cpp
	${COMPILER} ${CCFLAGS} -c core.cpp

flow.o:flow.cpp
	${COMPILER} ${CCFLAGS} -c flow.cpp

packetheader.o:packetheader.cpp
	${COMPILER} ${CCFLAGS} -c packetheader.cpp

pcapwrapper.o:pcapwrapper.cpp
	${COMPILER} ${CCFLAGS} -c pcapwrapper.cpp

stdoutputreader.o:stdoutputreader.cpp
	${COMPILER} ${CCFLAGS} -c stdoutputreader.cpp
		
graph.o:graph.cpp
	${COMPILER} ${CCFLAGS} -c graph.cpp

all : ejecutable

clean:
	rm -f *.o
	rm -f *~
	rm -f ${target}