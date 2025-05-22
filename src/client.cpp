/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rcosta-c <rcosta-c@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 18:34:02 by jduraes-          #+#    #+#             */
/*   Updated: 2025/05/23 00:18:23 by rcosta-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"
#include <unistd.h>
#include <stdexcept>
#include <cstring>
#include <iostream>
#include <sys/socket.h>

Client::Client(int fd) : _clientFd(fd), _auth(false)
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
    _currChannel = channel;
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
    
    if (bytesRead == -1)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            return (""); //não há dados, retornar vazio?!? <--duvida
            //throw std::runtime_error("No data available to read (non-blocking mode)");
        }
        throw std::runtime_error("Failed to receive message from client: " + std::string(strerror(errno)));
    }
    else if (bytesRead == 0)
    {
        // Client has closed the connection
        throw std::runtime_error("Client disconnected");
    }

    appendToBuffer(std::string(buffer, bytesRead));
    if(hasCompleteMessage())
        return(extractMessage());
    
    return ("");
    //return std::string(buffer, bytesRead); // Use bytesRead to avoid including uninitialized data
}

void Client::disconnect() {
    if (_clientFd != -1) {
        close(_clientFd);
        _clientFd = -1;
    }
}

void Client::appendToBuffer(const std::string &data)
{
    _buff += data;
}

bool Client::hasCompleteMessage()
{
    return (_buff.find("\r\n") != std::string::npos);
}

std::string Client::extractMessage()
{
    size_t pos = _buff.find("\r\n");
    if(pos == std::string::npos)
    {
        return ("");
    }
    std::string message = _buff.substr(0, pos);
    _buff.erase(0, pos + 2);
    return (message);
    
}

std::string Client::getRealname() const
{
    return (_realname);
}

void Client::setRealname(const std::string &name)
{
    _realname = name;    
}