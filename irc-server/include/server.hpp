#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <string>
#include <sys/epoll.h>
#include <memory>
#include "client.hpp"

class Client;

class Server 
{
	private:
		int _port;
		int _pass;
		int _epoll_fd;
		int _server_fd;
		std::vector<Client*> _clients;
		
	public:
		Server(int port/*, int pass = 0*/);
		void start();
		void acceptClient();
		void handleClient();
		//void handleCommand(const std::string &command, Client &client);


};

#endif // SERVER_HPP