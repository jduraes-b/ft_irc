#ifndef SERVER_HPP
#define SERVER_HPP

#include "utils/utils.hpp"
#include "client.hpp"

class Client;
class Channel;

class Server 
{
	private:
		int _port;
		std::string _pass;
		int _epoll_fd;
		int _server_fd;
		std::vector<Client*> _clients;
	    std::map<std::string, Channel*> _channels;

	    Client* getClientByFd(int fd);
    	std::string cleanInput(const std::string& input, const std::string& toRemove);

	public:
		Server(int port, const std::string &pass);
		~Server();

		void start();
		void acceptClient();
		void handleClient(int client_fd);
		void cleanup();

		void parseCommand(int client_fd, const std::string &command);
		void joinCommand(int client_fd, const std::string &params);
		void partCommand(int client_fd, const std::string &params);
		void kickCommand(int client_fd, const std::string &params);
		void inviteCommand(int client_fd, const std::string &params);
		void topicCommand(int client_fd, const std::string &params);
		void modeCommand(int client_fd, const std::string &params);
		void passCommand(int client_fd, const std::string &params);
		void nickCommand(int client_fd, const std::string &params);
		void userCommand(int client_fd, const std::string &params);
		void privmsgCommand(int client_fd, const std::string &params);
		void quitCommand(int client_fd, const std::string &params);
		void whoCommand(int client_fd, const std::string &params);

		void sendError(int client_fd, const std::string &error);

};


#endif // SERVER_HPP