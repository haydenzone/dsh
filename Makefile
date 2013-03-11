#-----------------------------------------------------------------------
# Use the GNU C/C++ compiler: 
CPP = g++

#OBJECT FILES 
OBJS = Processes.o Reroute.o helper.o dsh.o 

dsh: Processes.o Reroute.o helper.o dsh.o  
	${CPP} -lm ${OBJS} -o dsh
dsh.o: dsh.cpp
Processes.o: Processes.cpp
Reroute.o: Reroute.cpp
helper.o: helper.cpp

clean:
	rm *.o
