all: debug

debug:
	gcc *.c lib/*.c -o when -O1 -Wall -Wextra -g -std=c11 -pedantic

release:
	gcc *.c lib/*.c -o when -O3 -Wall -Wextra -Werror -std=c11 -pedantic

