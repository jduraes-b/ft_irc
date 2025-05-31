/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rcosta-c <rcosta-c@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/30 14:11:13 by rcosta-c          #+#    #+#             */
/*   Updated: 2025/05/30 23:34:00 by rcosta-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "channel.hpp"
#include "server.hpp"
#include "utils/utils.hpp"

/**
 * @brief PRIVMSG - Send private message to user or channel
 * 
 * @param client_fd File descriptor of the client sending the command
 * @param params Target (nickname or channel) and message text
 * 
 * @details Sends a message to a user or channel. For channels, the sender must be
 *          a member of the channel to send messages. The message is not echoed back
 *          to the sender.
 * 
 * @note Syntax: PRIVMSG <target> :<message>
 * @note Target can be a nickname or channel name
 * 
 * @errors
 * - ERR_NORECIPIENT (411): No recipient given
 * - ERR_NOTEXTTOSEND (412): No message text given
 * - ERR_NOSUCHNICK (401): Target user/channel doesn't exist
 * - ERR_CANNOTSENDTOCHAN (404): Cannot send to channel (not member)
 * 
 * @behavior
 * - User target: Message sent only to target user
 * - Channel target: Message sent to all channel members except sender
 * - Respects channel modes and permissions
 * 
 * @example
 * - PRIVMSG Alice :Hello Alice!
 * - PRIVMSG #general :Hello everyone!
 * - PRIVMSG #help :Can someone help me?
 */

void Server::privmsgCommand(int client_fd, const std::string &params)
{
    Client* client = getClientByFd(client_fd);
    if (!client || !client->isAuthenticated())
    {
        if (client)
            client->sendMessage(":irc.local 451 * :You have not registered\r\n");
        return;
    }
    if (params.empty())
    {
        client->sendMessage(ERR_NORECIPIENT(client->getNick(), "PRIVMSG") + "\r\n");
        return;
    }
    
    size_t msg_pos = params.find(" :");
    if (msg_pos == std::string::npos)
    {
        client->sendMessage(ERR_NOTEXTTOSEND(client->getNick()) + "\r\n");
        return;
    }
    
    std::string target = params.substr(0, msg_pos);
    std::string message = params.substr(msg_pos + 2);
    
    size_t first_non_space = target.find_first_not_of(" \t");
    if (first_non_space != std::string::npos)
        target = target.substr(first_non_space);
    size_t last_non_space = target.find_last_not_of(" \t");
    if (last_non_space != std::string::npos)
        target = target.substr(0, last_non_space + 1);
    if (target.empty())
    {
        client->sendMessage(ERR_NORECIPIENT(client->getNick(), "PRIVMSG") + "\r\n");
        return;
    }    
    if (message.empty())
    {
        client->sendMessage(ERR_NOTEXTTOSEND(client->getNick()) + "\r\n");
        return;
    }
    
    std::string privmsg = ":" + client->getNick() + "!" + client->getUser() 
                        + "@localhost PRIVMSG " + target + " :" + message + "\r\n";
    
    if (target[0] == '#' || target[0] == '&')
    {
        std::map<std::string, Channel*>::iterator chan_it = _channels.find(target);
        if (chan_it == _channels.end())
        {
            client->sendMessage(ERR_NOSUCHNICK(client->getNick(), target) + "\r\n");
            return;
        }
        
        Channel* channel = chan_it->second;
        if (!channel->canSendMessage(client))
        {
            client->sendMessage(ERR_CANNOTSENDTOCHAN(client->getNick(), target) + "\r\n");
            return;
        }
        channel->sendMessage(privmsg, client, client);
        
        std::cout << client->getNick() << " sent message to " << target 
                  << ": " << message << std::endl;
    }
    else
    {
        Client* target_client = NULL;
        for (std::vector<Client*>::iterator it = _clients.begin(); 
             it != _clients.end(); ++it)
        {
            if ((*it)->getNick() == target)
            {
                target_client = *it;
                break;
            }
        }
        
        if (!target_client)
        {
            client->sendMessage(ERR_NOSUCHNICK(client->getNick(), target) + "\r\n");
            return;
        }
        target_client->sendMessage(privmsg);
        
        std::cout << client->getNick() << " sent private message to " 
                  << target << ": " << message << std::endl;
    }
}