
NAME = codexion

CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread

SRC = src/main.c \
      src/parsing.c \
      src/init.c \
      src/heap.c \
      src/request.c \
      src/dongle.c \
      src/coder.c \
      src/threads.c \
      src/monitor.c \
      src/stop.c \
      src/time.c \
      src/log.c

OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.c src/codexion.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
