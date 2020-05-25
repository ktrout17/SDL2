NAME = game.exe
FLAGS = -lSDL2main -lSDL2
LINKSDL2 = I./include -L./lib
SRC = main.c
EXEC = $(SRC:.c=exe)

all: $(NAME)

$(NAME):
	@gcc $(FLAGS) -exe $(SRC)
	@ar rc $(NAME) $(EXEC)
	ranlib $(NAME)
	@echo "___________________________________"
	@echo "\ngame.exe compiled successfully"
	@echo "___________________________________"

clean:
	@/bin/rm -f $(EXEC)
	@echo "___________________________________"
	@echo "\ngame.exe removed successfully"
	@echo "___________________________________"

fclean: clean
	@/bin/rm -f $(NAME)
	@echo "___________________________________"
	@echo "\ngame.exe removed successfully"
	@echo "___________________________________"

re: fclean all