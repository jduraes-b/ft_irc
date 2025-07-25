/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jduraes- <jduraes-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/30 14:09:53 by rcosta-c          #+#    #+#             */
/*   Updated: 2025/07/12 12:33:46 by jduraes-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "channel.hpp"
#include "server.hpp"
#include "utils/utils.hpp"

/**
 * @brief USER - User registration command
 * 
 * @param client_fd File descriptor of the client sending the command
 * @param params Username, mode, unused, and realname parameters
 * 
 * @details The USER command is used at the beginning of a connection to specify the
 *          username, hostname, servername, and realname of a new user. This command
 *          must be sent before the client is considered fully registered. In modern
 *          IRC, only username and realname are typically used.
 * 
 * @note Syntax: USER <username> <mode> <unused> :<realname>
 * @note Mode is typically "0", unused is typically "*"
 * @note Must be preceded by NICK and PASS commands
 * @note Cannot be used after registration is complete
 * 
 * @errors
 * - ERR_NEEDMOREPARAMS (461): Not enough parameters provided
 * - ERR_ALREADYREGISTRED (462): Client is already registered
 * 
 * @behavior
 * - Stores username and realname information
 * - Triggers authentication check when NICK, USER, and PASS are all set
 * - On successful authentication:
 *   - Marks client as authenticated
 *   - Sends welcome messages (001-004)
 * - On failed authentication:
 *   - ERR_PASSWDMISMATCH (464) if password is incorrect
 * 
 * @registration The registration process requires:
 * 1. PASS command (set server password)
 * 2. NICK command (set nickname)
 * 3. USER command (set user details)
 * 
 * @example
 * - USER alice 0 * :Alice Johnson
 * - USER bob 0 * :Bob Smith
 * - USER charlie 8 * :Charlie Brown
 * 
 * @welcome On successful registration, sends:
 * - 001 RPL_WELCOME: Welcome message with full client mask
 * - 002 RPL_YOURHOST: Server host information
 * - 003 RPL_CREATED: Server creation date
 * - 004 RPL_MYINFO: Server name, version, and available modes
 */

void Server::userCommand(int client_fd, const std::string &params)
{
    Client* client = getClientByFd(client_fd);
    if (!client) return;
    
    std::istringstream iss(params);
    std::string username;
    iss >> username;
    
    client->setUser(username);
    
    // Check if we can authenticate the client
    if (!client->getNick().empty() && !client->getUser().empty() && !client->isAuthenticated())
    {
        if (client->getPass() != _pass)
        {
            sendError(client_fd, ":irc.local 464 * :Password incorrect");
            return;
        }
        
        client->authenticate();
        std::cout << "Client authenticated: " << client->getNick() << std::endl;
        //client->sendMessage(":irc.local 001 " + client->getNick() + " :Welcome to the IRC server!\r\n");
		std::string prefix   = ":ft_irc ";
		std::string nick     = client->getNick();
		std::string user     = client->getUser();
		std::string host     = client->getHost();
		std::string version  = "ft_IRC-1.0";
		std::string creation = "Mon Jul 12 2025 at 11:00:00";

		client->sendMessage(prefix + "001 " + nick + " :Welcome to the Internet Relay Network " 
							+ nick + "!" + user + "@" + host + "\r\n");
		client->sendMessage(prefix + "002 " + nick + " :Your host is ft_irc, running version " + version + "\r\n");
		client->sendMessage(prefix + "003 " + nick + " :This server was created " 
							+ creation + "\r\n");
		client->sendMessage(prefix + "004 " + nick + " ft_irc " 
							+ version + " o itkol\r\n");
	}
}
