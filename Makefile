DOTC = .cpp
DOTH = .h

PROG = main
LINK = neural_network commands
SRC = ${PROG}${DOTC} $(addsuffix ${DOTC}, $(LINK)) 
OBJ = ${SRC:.cpp=.o}

BINDIR = /usr/bin

CC = g++
INCS = 
LIBS = 

LDFLAGS = ${LIBS}
CFLAGS = -Wall -Wextra ${INCS}


${PROG}: ${OBJ} 
	${CC} ${OBJ} -o ${PROG} ${LDFLAGS}

${PROG}.o: ${PROG}${DOTC} config${DOTH}
	${CC} ${CFLAGS} -c $<

%.o: %${DOTC} %${DOTH}
	${CC} ${CFLAGS} -c $<

clean:
	rm *.o ${PROG}

.PHONY: clean
