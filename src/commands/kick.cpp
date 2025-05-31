/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rcosta-c <rcosta-c@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/30 14:06:56 by rcosta-c          #+#    #+#             */
/*   Updated: 2025/05/30 23:34:21 by rcosta-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "channel.hpp"
#include "server.hpp"
#include "utils/utils.hpp"

/**
 * @brief KICK - Forcibly remove a user from a channel
 * 
 * @param client_fd File descriptor of the client sending the command (must be channel op)
 * @param params Channel name, target nickname, and optional kick reason
 * 
 * @details Forcibly removes a user from a channel. This command can only be used by
 *          channel operators. The kicked user will receive a KICK message and be
 *          removed from the channel.
 * 
 * @note Syntax: KICK <channel> <user> [:<reason>]
 * @note Requires channel operator status
 * 
 * @errors
 * - ERR_NEEDMOREPARAMS (461): Missing required parameters
 * - ERR_NOSUCHCHANNEL (403): Channel doesn't exist
 * - ERR_NOTONCHANNEL (442): Kicker is not on the channel
 * - ERR_CHANOPRIVSNEEDED (482): Kicker is not a channel operator
 * - ERR_USERNOTINCHANNEL (441): Target user is not on the channel
 * 
 * @behavior
 * - Sends KICK message to all channel members (including the kicked user)
 * - Removes target from channel
 * - Uses kicker's nick as reason if none provided
 * 
 * @example
 * - KICK #general troublemaker
 * - KICK #chat spammer :Spamming is not allowed
 */

void Server::kickCommand(int client_fd, const std::string &params)
{
    Client* client = getClientByFd(client_fd);
    if (!client || !client->isAuthenticated())
    {
        if (client)
            client->sendMessage(":irc.local 451 * :You have not registered\r\n");
        return;
    }
    
    std::istringstream iss(params);
    std::string channel_name, target_nick, reason;
    
    iss >> channel_name >> target_nick;
    if (channel_name.empty() || target_nick.empty())
    {
        client->sendMessage(ERR_NEEDMOREPARAMS(client->getNick(), "KICK") + "\r\n");
        return;
    }
    
    size_t reason_pos = params.find(" :");
    if (reason_pos != std::string::npos)
    {
        reason = params.substr(reason_pos + 2);
    }
    else
    {
        std::string temp;
        std::getline(iss, temp);
        if (!temp.empty() && temp[0] == ' ')
            reason = temp.substr(1);
    }
    
    if (reason.empty())
        reason = client->getNick();
    std::map<std::string, Channel*>::iterator chan_it = _channels.find(channel_name);
    if (chan_it == _channels.end())
    {
        client->sendMessage(ERR_NOSUCHCHANNEL(client->getNick(), channel_name) + "\r\n");
        return;
    }
    
    Channel* channel = chan_it->second;
    if (!channel->isMember(client))
    {
        client->sendMessage(ERR_NOTONCHANNEL(client->getNick(), channel_name) + "\r\n");
        return;
    }
    if (!channel->isOperator(client))
    {
        client->sendMessage(ERR_CHANOPRIVSNEEDED(client->getNick(), channel_name) + "\r\n");
        return;
    }
    
    Client* target = NULL;
    for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if ((*it)->getNick() == target_nick)
        {
            target = *it;
            break;
        }
    }
    if (!target || !channel->isMember(target))
    {
        client->sendMessage(ERR_USERNOTINCHANNEL(client->getNick(), target_nick, channel_name) + "\r\n");
        return;
    }
    
    std::string kick_msg = ":" + client->getNick() + "!" + client->getUser() 
                         + "@localhost KICK " + channel_name + " " + target_nick 
                         + " :" + reason + "\r\n";
    
    channel->broadcastMessage(kick_msg);
    channel->removeMember(target);
    target->removeChannel(channel_name);
    std::cout << client->getNick() << " kicked " << target_nick 
              << " from " << channel_name << " (" << reason << ")" << std::endl;
    
    if (channel->getMemberCount() == 0)
    {
        delete channel;
        _channels.erase(chan_it);
    }
}
