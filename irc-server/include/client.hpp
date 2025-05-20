/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jduraes- <jduraes-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 18:27:39 by jduraes-          #+#    #+#             */
/*   Updated: 2025/05/20 18:48:13 by jduraes-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# define CLIENT_HPP

#include "server.hpp"
#include <string>
#include <vector>

class	Client
{
	private:
		int	_clientFd;
		std::string	_nick;
		std::string	_user;
		std::string	_currChannel;
		std::string	_buff;
		bool	_auth;

	public:
		Client(int fd);		
		~Client();
		int getFd() const;
		void setNick(const std::string &nickname);
		std::string getNick() const;
		void setUser(const std::string &username);
		std::string getUser() const;
		void setCurrChannel(const std::string &channel);
		std::string getCurrChannel() const;
		bool isAuthenticated() const;
		void authenticate();
		void sendMessage(const std::string &message);
		std::string receiveMessage();
		void disconnect();
};

# endif