/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pass.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rcosta-c <rcosta-c@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/30 14:08:46 by rcosta-c          #+#    #+#             */
/*   Updated: 2025/05/30 23:34:05 by rcosta-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "channel.hpp"
#include "server.hpp"
#include "utils/utils.hpp"

/**
 * @brief PASS - Password command for server authentication
 * 
 * @param client_fd File descriptor of the client sending the command
 * @param params Password string to authenticate with the server
 * 
 * @details This command is used to set a connection password. The password must be
 *          set before the NICK/USER registration is completed. Once a client is
 *          registered, the PASS command is no longer available.
 * 
 * @note Syntax: PASS <password>
 * 
 * @errors
 * - ERR_NEEDMOREPARAMS (461): Not enough parameters provided
 * - ERR_ALREADYREGISTRED (462): Client is already registered
 * 
 * @example
 * - PASS secretpassword
 * - PASS myserverpass123
 */

void Server::passCommand(int client_fd, const std::string &params)
{
    Client* client = getClientByFd(client_fd);
    if (!client)
        return;
    if (client->isAuthenticated())
    {
        client->sendMessage(ERR_ALREADYREGISTRED(client->getNick()) + "\r\n");
        return;
    }
    if (params.empty())
    {
        client->sendMessage(ERR_NEEDMOREPARAMS(client->getNick(), "PASS") + "\r\n");
        return;
    }
    client->setPass(params);
    std::cout << "Password set for client fd: " << client_fd << std::endl;
}