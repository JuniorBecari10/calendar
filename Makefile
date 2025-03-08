all:
	gcc *.c -o calendar -O1 -Wall -Wextra -Werror
	./calendar
