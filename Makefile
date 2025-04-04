all: debug

debug:
	gcc *.c lib/*.c -o calendar -O1 -Wall -Wextra -g

release:
	gcc *.c lib/*.c -o calendar -O3 -Wall -Wextra -Werror

