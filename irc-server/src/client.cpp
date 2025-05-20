/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jduraes- <jduraes-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 18:34:02 by jduraes-          #+#    #+#             */
/*   Updated: 2025/05/20 18:51:44 by jduraes-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"
#include <unistd.h>
#include <stdexcept>
#include <cstring>
#include <iostream>

Client::Client(int fd) : _clientFd(fd), _authenticated(false)
{
}

Client::~Client()
{
    disconnect();
}

int Client::getFd() const
{
    return _clientFd;
}

void Client::setNickname(const std::string &nickname)
{
    _nick = nickname;
}

std::string Client::getNickname() const {
    return _nick;
}

void Client::setUsername(const std::string &username) 
{
    _user = username;
}

std::string Client::getUsername() const
{
    return _user;
}

void Client::setCurrentChannel(const std::string &channel)
{
    _currentChannel = channel;
}

std::string Client::getCurrentChannel() const
{
    return _currChannel;
}

bool Client::isAuthenticated() const
{
    return _auth;
}

void Client::authenticate() {
    _auth = true;
}

void Client::sendMessage(const std::string &message)
{
	//the flags stop the client from sending SIGPIPE and make the function non-blocking, respectively
	//i am not sure how this will behave but it sounds like what the subject demands
    if (send(_clientFd, message.c_str(), message.size(), MSG_NOSIGNAL | MSG_DONTWAIT) == -1)
	{
        if (errno == EAGAIN || errno == EWOULDBLOCK)
			throw std::runtime_error("Socket send buffer is full, message could not be sent");
        else
        	throw std::runtime_error("Failed to send message to client: " + std::string(strerror(errno)));
    }
}

std::string Client::receiveMessage() {
    char buffer[512];
    memset(buffer, 0, sizeof(buffer));
    // Using MSG_DONTWAIT to make the recv call non-blocking, same situation as above
    int bytesRead = recv(_clientFd, buffer, sizeof(buffer) - 1, MSG_DONTWAIT);
    if (bytesRead == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            throw std::runtime_error("No data available to read (non-blocking mode)");
        } else {
            throw std::runtime_error("Failed to receive message from client: " + std::string(strerror(errno)));
        }
    } else if (bytesRead == 0) {
        // Client has closed the connection
        throw std::runtime_error("Client disconnected");
    }
    return std::string(buffer, bytesRead); // Use bytesRead to avoid including uninitialized data
}

void Client::disconnect() {
    if (_clientFd != -1) {
        close(_clientFd);
        _clientFd = -1;
    }
}
