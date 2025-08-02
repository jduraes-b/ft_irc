/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rcosta-c <rcosta-c@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 18:56:57 by jduraes-          #+#    #+#             */
/*   Updated: 2025/08/02 11:11:01 by rcosta-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils/utils.hpp"
#include <arpa/inet.h>
#include "server.hpp"
#include "client.hpp"
#include "channel.hpp"

Server::Server(int port, const std::string &pass): _port(port), _pass(pass), _server_fd(-1)
{
}

Server::~Server()
{
}

void Server::cleanup()
{
    // cleanup of all clients
    for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (*it != NULL)
        {
            // close socket client
            close((*it)->getFd());
            
            // delete client object
            delete *it;
        }
    }
    
    // clean vector
    _clients.clear();
    
    // channels cleanup
    for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
    {
        delete it->second;
    }
    _channels.clear();
    
    // close socket server
    if (_server_fd != -1)
    {
        close(_server_fd);
        _server_fd = -1;
    }
}

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
	sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET; //sets adress family to use IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY; //accepts connections to any network interface
	server_addr.sin_port = htons(_port);//converts port to network format and sets it
	if (bind(_server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
        throw std::runtime_error("Failed to bind socket");
	if (listen(_server_fd, SOMAXCONN) == -1)//constant that specifies max allowed queued connections
        throw std::runtime_error("Failed to listen on socket");
	
	std::cout << "Server started on port " << _port << std::endl;	

	// Vector for pollfd structures
	std::vector<struct pollfd> pollfds;
	
	// add server socket to vector of pollfds
	struct pollfd server_pfd;
	server_pfd.fd = _server_fd;
	server_pfd.events = POLLIN; // Monitor for incoming connections
	server_pfd.revents = 0;
	pollfds.push_back(server_pfd);
	
	while (g_running)
	{
		// we call poll() only here.
		int ret = poll(pollfds.data(), pollfds.size(), -1); // -1 = espera indefinidamente
		
		if (ret == -1)
		{
			if (errno == EINTR)
				continue;
			throw std::runtime_error("poll failed");
		}
		
		// working on file descriptors
		for (size_t i = 0; i < pollfds.size(); i++)
		{
			if (pollfds[i].revents & POLLIN)
			{
				if (pollfds[i].fd == _server_fd)
				{
					// new connection in server socket
					acceptClient();
					
					// refresh vector of pollfds with new client
					// (made inside acceptClient)
					rebuildPollFds(pollfds);
				}
				else
				{
					// data received from a client
					handleClient(pollfds[i].fd);
					
					// if client removed, reconstruct vector
					if (getClientByFd(pollfds[i].fd) == NULL)
					{
						rebuildPollFds(pollfds);
						break; // exit loop for and restart
					}
				}
			}
			else if (pollfds[i].revents & (POLLHUP | POLLERR | POLLNVAL))
			{
				// client disconnected or error
				std::cout << "Client disconnected or error on fd: " << pollfds[i].fd << std::endl;
				
				// remove client
				for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
				{
					if ((*it)->getFd() == pollfds[i].fd)
					{
                        removeClientFromAllChannels(*it);
						close(pollfds[i].fd);
						delete *it;
						_clients.erase(it);
						break;
					}
				}
				
				// reconstruct vector of pollfds
				rebuildPollFds(pollfds);
				break; // exit loop and restart
			}
		}
	}
	
    // Clean up: close sockets, free memory, etc.
    cleanup();
}

void Server::removeClientFromAllChannels(Client* client)
{
    if (!client) return;
    
    std::vector<std::string> channels = client->getChannels();
    
    for (std::vector<std::string>::iterator it = channels.begin(); it != channels.end(); ++it)
    {
        std::string channelName = *it;
        
        std::map<std::string, Channel*>::iterator channelIt = _channels.find(channelName);
        if (channelIt != _channels.end())
        {
            Channel* channel = channelIt->second;
            
            std::string quitMsg = ":" + client->getNick() + "!" + client->getUser() + "@" + client->getHost() + " QUIT :Client disconnected\r\n";
            channel->broadcastMessage(quitMsg, client);
            
            channel->removeMember(client);
            
            if (channel->getMemberCount() == 0)
            {
                delete channel;
                _channels.erase(channelIt);
            }
        }
    }
    
    client->clearChannels();
}

// new function to reconstruct vector of pollfds
void Server::rebuildPollFds(std::vector<struct pollfd>& pollfds)
{
	pollfds.clear();
	
	// add server socket
	struct pollfd server_pfd;
	server_pfd.fd = _server_fd;
	server_pfd.events = POLLIN;
	server_pfd.revents = 0;
	pollfds.push_back(server_pfd);
	
	// add all the socket client
	for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		struct pollfd client_pfd;
		client_pfd.fd = (*it)->getFd();
		client_pfd.events = POLLIN;
		client_pfd.revents = 0;
		pollfds.push_back(client_pfd);
	}
}

void Server::acceptClient() {
    sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
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

    // Create a new Client object and add it to the list
	char ipstr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, ipstr, sizeof(ipstr));
	Client* client = new Client(client_fd);
	client->setHost(std::string(ipstr));
    _clients.push_back(client);
    std::cout << "New client connected: " << client_fd << std::endl;
	//attempting to avoid instant disconnection
    try {
        _clients.back()->sendMessage(":irc.local NOTICE * :Hello! Make sure you're registered and authenticated to use the server.\r\n");
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

        // Accumulate data in the client's buffer
		client->getBuffer() += msg;

		size_t pos;
		// Process all complete commands in the buffer
		while ((pos = client->getBuffer().find("\r\n")) != std::string::npos)
		{
			std::string line = client->getBuffer().substr(0, pos);
			client->getBuffer().erase(0, pos + 2); // Remove processed command
			parseCommand(client_fd, line);
            if (client->getShouldQuit())
            {
                close(client_fd);
				// Remover do vector e apagar
				for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
				{
					if ((*it)->getFd() == client_fd)
					{
						close(client_fd);
						delete *it;
						_clients.erase(it);
						break;
					}
				}
				return; // Sair da função
			}
		}
        // Any leftover in _buff is a partial command, keep it for next time
    }
    catch (const std::runtime_error &e)
    {
        std::cout << "Client disconnected: " << e.what() << std::endl;
        
        // Remove client from vector
        for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
        {
            if ((*it)->getFd() == client_fd)
            {
                removeClientFromAllChannels(*it);
                close(client_fd);
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
        "PASS", "NICK", "USER", "PRIVMSG", "QUIT", "WHO", "CAP"
    };
    const int numCommands = 13;
    
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
		case 12:
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
