/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rcosta-c <rcosta-c@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 18:56:57 by jduraes-          #+#    #+#             */
/*   Updated: 2025/05/31 00:51:04 by rcosta-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils/utils.hpp"
#include "server.hpp"
#include "client.hpp"
#include "channel.hpp"

Server::Server(int port, const std::string &pass): _port(port), _pass(pass),_epoll_fd(-1), _server_fd(-1)
{
}

Server::~Server()
{}

void	Server::start()
{
	_server_fd = socket(AF_INET, SOCK_STREAM, 0);//creates socket
	if (_server_fd == -1)
		throw std::runtime_error("Failed to create socket");
	fcntl(_server_fd, F_SETFL, O_NONBLOCK); //sets non-blocking mode
	//
	int optval = 1;
	if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)))
		throw std::runtime_error("Error while setting socket options");
	//this sets option for the socket to reuse local adress
	sockaddr_in server_addr = {}; //initializes struct with zero values
	server_addr.sin_family = AF_INET; //sets adress family to use IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY; //accepts connections to any network interface
	server_addr.sin_port = htons(_port);//converts port to network format and sets it
	if (bind(_server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
        throw std::runtime_error("Failed to bind socket");
	if (listen(_server_fd, SOMAXCONN) == -1)//constant that specifies max allowed queued connections
        throw std::runtime_error("Failed to listen on socket");
	    _epoll_fd = epoll_create1(0);
    if (_epoll_fd == -1)
		throw std::runtime_error("Failed to create epoll instance");
	epoll_event event = {};
	event.events = EPOLLIN; // Monitor for incoming connections
	event.data.fd = _server_fd;
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _server_fd, &event) == -1)
		throw std::runtime_error("Failed to add server socket to epoll");
	std::cout << "Server started on port " << _port << std::endl;	

	const int MAX_EVENTS = 10;
	epoll_event events[MAX_EVENTS];
	while (true)
	{
		int	num_events = epoll_wait(_epoll_fd, events, MAX_EVENTS, -1);
		if (num_events == -1)
			throw std::runtime_error("epoll_wait failed");
		for (int i = 0; i < num_events; i++)
		{
			if (events[i].data.fd == _server_fd)
				acceptClient();
			else
				handleClient(events[i].data.fd);
		}
	}
}

void Server::acceptClient() {
    sockaddr_in client_addr = {};
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(_server_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd == -1) {
        std::cerr << "Failed to accept client: " << strerror(errno) << std::endl;
        return;
    }

    // Log the accepted client
    std::cout << "Accepted client with fd: " << client_fd << std::endl;

    // Set the client socket to non-blocking mode
    if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1) {
        std::cerr << "Failed to set non-blocking mode for client: " << strerror(errno) << std::endl;
        close(client_fd);
        return;
    }

    // Add the client socket to the epoll instance
    epoll_event event = {};
    event.events = EPOLLIN; // Monitor for incoming data
    event.data.fd = client_fd;
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1) {
        std::cerr << "Failed to add client socket to epoll: " << strerror(errno) << std::endl;
        close(client_fd);
        return;
    }

    // Create a new Client object and add it to the list
    _clients.push_back(new Client(client_fd));
    std::cout << "New client connected: " << client_fd << std::endl;
	//attempting to avoid instant disconnection
    try {
        _clients.back()->sendMessage(":irc.local NOTICE * :Welcome!\r\n");
    } catch (const std::exception &e) {
        std::cerr << "Failed to send welcome message: " << e.what() << std::endl;
    }
}

void Server::handleClient(int client_fd)
{
    Client* client = getClientByFd(client_fd);
    if (!client)
        return;
    
    try
    {
        std::string msg = client->receiveMessage();
        if (msg.empty())
            return;
        
        size_t start = 0, end;
        while ((end = msg.find("\r\n", start)) != std::string::npos)
        {
            std::string line = msg.substr(start, end - start);
            start = end + 2;
            parseCommand(client_fd, line);
        }
        
        // Handle partial command (no \r\n yet)
        if (start < msg.length())
        {
            // TODO
            // Store partial command for next receive
            // add a buffer to Client class for this
        }
    }
    catch (const std::runtime_error &e)
    {
        std::cout << "Client disconnected: " << e.what() << std::endl;
        
        // Remove from epoll
        epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
        
        // Remove client from vector
        for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
        {
            if ((*it)->getFd() == client_fd)
            {
                delete *it;
                _clients.erase(it);
                break;
            }
        }
    }
}


Client* Server::getClientByFd(int fd)
{
    for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if ((*it)->getFd() == fd)
            return *it;
    }
    return NULL;
}

std::string Server::cleanInput(const std::string &input, const std::string &toRemove)
{
    std::string result = input;
    
    size_t start = result.find_first_not_of(" \t\r\n");
    if (start != std::string::npos)
        result = result.substr(start);
    size_t end = result.find_last_not_of(toRemove + " \t\r\n");
    if (end != std::string::npos)
        result = result.substr(0, end + 1);
    
    return result;
}

void Server::parseCommand(int client_fd, const std::string &command)
{
    if (command.empty() || command == "\r\n")
        return;
    
    const char* commands[] = {
        "JOIN", "PART", "KICK", "INVITE", "TOPIC", "MODE",
        "PASS", "NICK", "USER", "PRIVMSG", "QUIT", "WHO"
    };
    const int numCommands = 12;
    
    Client* client = getClientByFd(client_fd);
    if (!client)
        return;
    
    std::string upperCommand = command;
    std::transform(upperCommand.begin(), upperCommand.end(), upperCommand.begin(), ::toupper);
    int cmdIndex = -1;
    std::string foundCommand;
    std::string restOfCommand;
    
    for (int i = 0; i < numCommands; i++)
    {
        size_t pos = upperCommand.find(commands[i]);
        if (pos != std::string::npos)
        {
            // Check if it's a complete command (followed by space or end of string)
            size_t cmdLen = strlen(commands[i]);
            if (pos + cmdLen == upperCommand.length() || 
                upperCommand[pos + cmdLen] == ' ' || 
                upperCommand[pos + cmdLen] == '\r' ||
                upperCommand[pos + cmdLen] == '\n')
            {
                cmdIndex = i;
                foundCommand = commands[i];
                // Extract parameters after the command
                if (pos + cmdLen < command.length())
                    restOfCommand = command.substr(pos + cmdLen);
                break;
            }
        }
    }
    
    if (cmdIndex != -1)
    {
        if (foundCommand == "PASS" || foundCommand == "QUIT" || foundCommand == "NICK")
            restOfCommand = cleanInput(restOfCommand, " \t");
        else
            restOfCommand = cleanInput(restOfCommand, "\r\n");
    }
    std::cout << "Command: [" << foundCommand << "]" << std::endl;
    std::cout << "Parameters: [" << restOfCommand << "]" << std::endl;
    switch (cmdIndex)
    {
        case 0:
            joinCommand(client_fd, restOfCommand);
            break;
        case 1:
            partCommand(client_fd, restOfCommand);
            break;
        case 2:
            kickCommand(client_fd, restOfCommand);
            break;
        case 3:
            inviteCommand(client_fd, restOfCommand);
            break;
        case 4:
            topicCommand(client_fd, restOfCommand);
            break;
        case 5:
            modeCommand(client_fd, restOfCommand);
            break;
        case 6:
            passCommand(client_fd, restOfCommand);
            break;
        case 7:
            nickCommand(client_fd, restOfCommand);
            break;
        case 8:
            userCommand(client_fd, restOfCommand);
            break;
        case 9:
            privmsgCommand(client_fd, restOfCommand);
            break;
        case 10:
            quitCommand(client_fd, restOfCommand);
            break;
        case 11:
            whoCommand(client_fd, restOfCommand);
            break;
        default:
            // Unknown command
            sendError(client_fd, "421 " + client->getNick() + " " + foundCommand + " :Unknown command");
            break;
    }
}

void Server::sendError(int client_fd, const std::string &error)
{
    Client* client = getClientByFd(client_fd);
    if (client)
        client->sendMessage(error + "\r\n");
}