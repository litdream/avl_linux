OBJ=avl.o simple.o

all: $(OBJ)
	gcc -g -Wall $(OBJ)

clean:
	rm -f $(OBJ)
