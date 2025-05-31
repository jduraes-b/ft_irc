/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rcosta-c <rcosta-c@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/30 14:07:29 by rcosta-c          #+#    #+#             */
/*   Updated: 2025/05/30 23:33:39 by rcosta-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"
#include "channel.hpp"
#include "utils/utils.hpp"

/**
 * @brief INVITE - Invite a user to a channel
 * 
 * @param client_fd File descriptor of the client sending the command
 * @param params Target nickname and channel name
 * 
 * @details Invites a user to a channel. If the channel is invite-only (+i), only
 *          channel operators can send invites. The invited user will be able to
 *          join the channel even if it's invite-only.
 * 
 * @note Syntax: INVITE <nickname> <channel>
 * @note On +i channels, requires operator status
 * 
 * @errors
 * - ERR_NEEDMOREPARAMS (461): Missing required parameters
 * - ERR_NOSUCHNICK (401): Target user doesn't exist
 * - ERR_NOSUCHCHANNEL (403): Channel doesn't exist
 * - ERR_NOTONCHANNEL (442): Inviter is not on the channel
 * - ERR_CHANOPRIVSNEEDED (482): Channel is +i and inviter is not operator
 * - ERR_USERONCHANNEL (443): Target user is already on the channel
 * 
 * @behavior
 * - Adds user to channel invite list
 * - Sends INVITE message to target user
 * - Sends RPL_INVITING (341) to inviter
 * 
 * @example
 * - INVITE Alice #private
 * - INVITE Bob #meeting
 */

void Server::inviteCommand(int client_fd, const std::string& params)
{
    Client* client = getClientByFd(client_fd);
    if (!client || !client->isAuthenticated())
    {
        if (client)
            client->sendMessage(":irc.local 451 * :You have not registered\r\n");
        return;
    }
    
    std::istringstream iss(params);
    std::string target_nick, channel_name;
    iss >> target_nick >> channel_name;
    if (target_nick.empty() || channel_name.empty())
    {
        client->sendMessage(ERR_NEEDMOREPARAMS(client->getNick(), "INVITE") + "\r\n");
        return;
    }
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
    if (channel->getMode('i') && !channel->isOperator(client))
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
    if (!target)
    {
        client->sendMessage(ERR_NOSUCHNICK(client->getNick(), target_nick) + "\r\n");
        return;
    }
    if (channel->isMember(target))
    {
        client->sendMessage(ERR_USERONCHANNEL(client->getNick(), target_nick, channel_name) + "\r\n");
        return;
    }
    channel->addInvite(target);
    client->sendMessage(RPL_INVITING(client->getNick(), target_nick, channel_name) + "\r\n");
    std::string invite_msg = ":" + client->getNick() + "!" + client->getUser() 
                           + "@localhost INVITE " + target_nick + " " + channel_name + "\r\n";
    target->sendMessage(invite_msg);
    std::cout << client->getNick() << " invited " << target_nick 
              << " to " << channel_name << std::endl;
}