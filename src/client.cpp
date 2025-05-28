/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rcosta-c <rcosta-c@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 18:34:02 by jduraes-          #+#    #+#             */
/*   Updated: 2025/05/28 11:00:06 by rcosta-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"


Client::Client(int fd) : _clientFd(fd), _currChannel(""), _authenticated(false)
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

void Client::setNick(const std::string &nickname)
{
    _nick = nickname;
}

std::string Client::getNick() const {
    return _nick;
}

void Client::setUser(const std::string &username) 
{
    _user = username;
}

std::string Client::getUser() const
{
    return _user;
}

void Client::setPass(const std::string &password) 
{
    _pass = password;
}

std::string Client::getPass() const
{
    return _pass;
}

void Client::setCurrChannel(const std::string &channel)
{
    _currChannel = channel;
}

std::string Client::getCurrChannel() const
{
    return _currChannel;
}

bool Client::isAuthenticated() const
{
    return _authenticated;
}

void Client::authenticate() {
    _authenticated = true;
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

std::string Client::receiveMessage()
{
    char buffer[512];
    memset(buffer, 0, sizeof(buffer));
    int bytesRead = recv(_clientFd, buffer, sizeof(buffer) - 1, MSG_DONTWAIT);
    if (bytesRead == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // No data available to read, return an empty string
            return "";
        } else {
            throw std::runtime_error("Failed to receive message from client: " + std::string(strerror(errno)));
        }
    } else if (bytesRead == 0) {
        throw std::runtime_error("Client disconnected");
    }
    return std::string(buffer, bytesRead);
}

void Client::disconnect() {
    if (_clientFd != -1) {
        close(_clientFd);
        _clientFd = -1;
    }
}
