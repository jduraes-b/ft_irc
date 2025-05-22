CC = g++
CFLAGS = -Wall -Wextra -Werror -I include -std=c++98
SRC = src/main.cpp src/server.cpp src/client.cpp src/utils/utils.cpp \
      src/commands/join.cpp src/commands/privmsg.cpp src/commands/quit.cpp
OBJ = $(SRC:.cpp=.o)
NAME = irc_server

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(OBJ) -o $(NAME)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re