/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jduraes- <jduraes-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 18:27:39 by jduraes-          #+#    #+#             */
/*   Updated: 2025/05/26 20:27:31 by jduraes-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "server.hpp"
#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <cerrno>

class	Client
{
	private:
		int	_clientFd;
		std::string	_nick;
		std::string	_user;
		std::string	_currChannel;
		std::string	_buff;
		std::string	_pass;
		bool	_authenticated;

	public:
		Client(int fd);		
		~Client();
		int getFd() const;
		void setNick(const std::string &nickname);
		std::string getNick() const;
		void setUser(const std::string &username);
		std::string getUser() const;
		void setPass(const std::string &password);
		std::string getPass() const;
		void setCurrChannel(const std::string &channel);
		std::string getCurrChannel() const;
		bool isAuthenticated() const;
		void authenticate();
		void sendMessage(const std::string &message);
		std::string receiveMessage();
		void disconnect();
};

# endif