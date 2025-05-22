/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rcosta-c <rcosta-c@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 18:27:39 by jduraes-          #+#    #+#             */
/*   Updated: 2025/05/23 00:17:15 by rcosta-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "server.hpp"
#include <string>
#include <vector>

class	Client
{
	private:
		int	_clientFd;
		std::string _hostname;
		std::string _realname;
		std::string	_nick;
		std::string	_user;
		std::string	_currChannel;
		//std::vector<std::string> _channels;    Nao faria mais sentido uma lista de canais?
		std::string	_buff;
		bool	_auth;

	public:
		Client(int fd);		
		~Client();
		int getFd() const;
		void setNickname(const std::string &nickname);
		std::string getNickname() const;
		void setUsername(const std::string &username);
		std::string getUsername() const;
		void setCurrentChannel(const std::string &channel);
		std::string getCurrentChannel() const;
		bool isAuthenticated() const;
		void authenticate();
		void sendMessage(const std::string &message);
		std::string receiveMessage();
		void disconnect();

		std::string getRealname() const;
		void setRealname(const std::string &name);
		void appendToBuffer(const std::string &data);
		bool hasCompleteMessage();
		std::string extractMessage();
		void joinChannel(const std::string &channel);
		void leaveChannel(const std::string &channel);
		bool inChannel(const std::string &channel) const;
		std::vector<std::string> getChannels() const;
};
