# Variables
OBJ := main.o file_handler.o optimization_model.o
CC := gcc
LD := gcc
CFLAGS := -g -lm
OUTFILE := assignment1.bin
VPATH := .

# Main program
all: ${OBJ}
	${LD} $^ -o ${OUTFILE} ${CFLAGS}

${OBJ}: %.o: %.c
	${CC} -c $< -o $@ ${CFLAGS}

clean:
	rm -rf *.gch ${OBJ} ${OUTFILE}
