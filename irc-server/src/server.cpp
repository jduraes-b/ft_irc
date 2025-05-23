/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jduraes- <jduraes-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 18:56:57 by jduraes-          #+#    #+#             */
/*   Updated: 2025/05/22 21:02:57 by jduraes-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"
#include "client.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdexcept> // For std::runtime_error
#include <iostream>
#include <cstring> // For strerror

Server::Server(int port/*, int pass*/): _port(port), _epoll_fd(-1), _server_fd(-1)
{
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
				handleClient(); //TODO
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
    _clients.push_back(Client(client_fd));
    std::cout << "New client connected: " << client_fd << std::endl;
}

void Server::handleClient() //PLACEHOLDER FOR TESTING
{
    for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ) {
        try {
            std::cout << "Handling client with fd: " << it->getFd() << std::endl;

            // Attempt to receive a message from the client
            std::string message = it->receiveMessage();
            if (message.empty()) {
                std::cout << "No data received from client " << it->getFd() << std::endl;
                ++it;
                continue; // No data to process, move to the next client
            }

            std::cout << "Received from client " << it->getFd() << ": " << message << std::endl;

            // Echo the message back to the client
            it->sendMessage("Echo: " + message);
            ++it;
        } catch (const std::runtime_error &e) {
            std::cerr << "Client " << it->getFd() << " error: " << e.what() << std::endl;

            // If the client disconnected, remove it from the list
            if (std::string(e.what()) == "Client disconnected") {
                std::cout << "Removing client " << it->getFd() << " from the list" << std::endl;
                it = _clients.erase(it);
            } else {
                ++it;
            }
        }
    }
}
