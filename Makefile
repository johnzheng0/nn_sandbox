PROG = main
LINK = neural_network commands
HED = $(addsuffix .h, $(LINK)) 
SRC = ${PROG}.cpp $(addsuffix .cpp, $(LINK)) 
OBJ = ${SRC:.cpp=.o}

BINDIR = /usr/bin

CC = g++
INCS = 
LIBS = 

LDFLAGS = ${LIBS}
CFLAGS = -Wall -Wextra ${INCS}


${PROG}: ${OBJ} 
	${CC} ${OBJ} -o ${PROG} ${LDFLAGS}

${PROG}.o: ${PROG}.cpp config.h
	${CC} ${CFLAGS} -c $<

%.o: %.cpp %.h
	${CC} ${CFLAGS} -c $<

clean:
	rm *.o ${PROG}

.PHONY: clean