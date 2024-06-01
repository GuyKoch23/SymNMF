COMMAND=gcc

FLAGS=-ansi -Wall -Wextra -Werror -pedantic-errors
LMFLAGS=-lm

symnmf: symnmf.c
	$(COMMAND) $(FLAGS) -o symnmf symnmf.c $(LMFLAGS)
