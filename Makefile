CC = g++
CFLAGS = -Wall -Wextra -Werror -I include -std=c++98

SRC = src/main.cpp src/server.cpp src/client.cpp src/channel.cpp \
		src/utils/utils.cpp src/commands/join.cpp src/commands/privmsg.cpp \
		src/commands/quit.cpp src/commands/invite.cpp src/commands/kick.cpp \
		src/commands/mode.cpp src/commands/nick.cpp src/commands/part.cpp \
		src/commands/pass.cpp src/commands/topic.cpp src/commands/user.cpp \
		src/commands/who.cpp

OBJ = $(SRC:.cpp=.o)

NAME = ircserv

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
