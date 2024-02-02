none:
	gcc src/*.c -o cub -lncurses -lm
run:
	gcc src/*.c -o cub -lncurses -lm
	./cub
