/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rcosta-c <rcosta-c@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 18:27:39 by jduraes-          #+#    #+#             */
/*   Updated: 2025/07/12 12:49:12 by rcosta-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "server.hpp"


class	Client
{
	private:
		int	_clientFd;
		std::string	_nick;
		std::string	_user;
		std::vector<std::string> _channelsList;
		std::string	_buff;
		std::string	_pass;
		std::string	_host;
		bool	_authenticated;
		bool	_shouldquit;

	public:
		Client(int fd);		
		~Client();
		int getFd() const;
		void setNick(const std::string &nickname);
		std::string getNick() const;
		void setUser(const std::string &username);
		std::string getUser() const;
		void setShouldQuit(bool shouldQuit);
		bool getShouldQuit() const;
		void setPass(const std::string &password);
		std::string getPass() const;
		void setCurrChannel(const std::string &channel);
		std::string getCurrChannel() const;
		std::string& getBuffer();
		std::string getHost() const;
		void setHost(const std::string &host);
		void addChannel(const std::string &channel);
		void removeChannel(const std::string &channel);
		bool isInChannel(const std::string &channel) const;
		std::vector<std::string> getChannels() const;
		size_t getChannelCount() const;
		void clearChannels();
		bool isAuthenticated() const;
		void authenticate();
		void sendMessage(const std::string &message);
		std::string receiveMessage();
		void disconnect();
};

# endif
