/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rcosta-c <rcosta-c@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/30 14:05:45 by rcosta-c          #+#    #+#             */
/*   Updated: 2025/05/30 23:34:52 by rcosta-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "channel.hpp"
#include "server.hpp"
#include "utils/utils.hpp"

/**
 * @brief JOIN - Join one or more channels
 * 
 * @param client_fd File descriptor of the client sending the command
 * @param params Channel names (comma-separated) and optional keys (comma-separated)
 * 
 * @details Makes the client join the specified channels. If the channel doesn't exist,
 *          it will be created and the client will become the channel operator. Multiple
 *          channels can be joined at once by separating them with commas.
 * 
 * @note Syntax: JOIN <channel>{,<channel>} [<key>{,<key>}]
 * @note Channel names must start with # or &
 * 
 * @errors
 * - ERR_NEEDMOREPARAMS (461): No channel specified
 * - ERR_INVITEONLYCHAN (473): Channel is invite-only (+i)
 * - ERR_BADCHANNELKEY (475): Wrong channel key (+k)
 * - ERR_CHANNELISFULL (471): Channel is full (+l)
 * - ERR_NOSUCHCHANNEL (403): Invalid channel name format
 * 
 * @behavior
 * - Creates channel if it doesn't exist (creator becomes operator)
 * - Sends JOIN message to all channel members
 * - Sends channel topic (332/331) to joining client
 * - Sends names list (353/366) to joining client
 * 
 * @example
 * - JOIN #general
 * - JOIN #private secretkey
 * - JOIN #chan1,#chan2,#chan3 key1,,key3
 */


void Server::joinCommand(int client_fd, const std::string &params)
{
    Client* client = getClientByFd(client_fd);
    if (!client)
        return;
        
   if (!client->isAuthenticated())
    {
        client->sendMessage(ERR_NOTREGISTERED(client->getNick()) + "\r\n");
        return;
    }
    if (params.empty())
    {
        client->sendMessage(ERR_NEEDMOREPARAMS(client->getNick(), "JOIN") + "\r\n");
        return;
    }
    
    std::istringstream iss(params);
    std::string chName, chPassword;
    iss >> chName >> chPassword;
    std::vector<std::string> channels;
    std::vector<std::string> keys;
    std::stringstream channel_stream(chName);
    std::string channel;
    
    while (std::getline(channel_stream, channel, ','))
    {
        if (!channel.empty())
            channels.push_back(channel);
    }
    
    if (!chPassword.empty())
    {
        std::stringstream key_stream(chPassword);
        std::string key;
        while (std::getline(key_stream, key, ','))
        {
            keys.push_back(key);
        }
    }
    
    for (size_t i = 0; i < channels.size(); ++i)
    {
        std::string chan_name = channels[i];
        std::string key = (i < keys.size()) ? keys[i] : "";
        if (chan_name.empty() || (chan_name[0] != '#' && chan_name[0] != '&'))
        {
            client->sendMessage(ERR_NOSUCHCHANNEL(client->getNick(), chan_name) + "\r\n");
            continue;
        }
        Channel* channel = NULL;
        std::map<std::string, Channel*>::iterator it = _channels.find(chan_name);
        
        if (it != _channels.end())
        {
            channel = it->second;
            if (channel->isMember(client))
            {
                continue;
            }
            
            if (!channel->canJoin(client, key))
            {
                if (channel->getMode('i') && !channel->isInvited(client))
                {
                    client->sendMessage(ERR_INVITEONLYCHAN(client->getNick(), chan_name) + "\r\n");
                }
                else if (channel->getMode('k') && key != channel->getKey())
                {
                    client->sendMessage(ERR_BADCHANNELKEY(client->getNick(), chan_name) + "\r\n");
                }
                else if (channel->getMode('l') && channel->getMemberCount() >= channel->getUserLimit())
                {
                    client->sendMessage(ERR_CHANNELISFULL(client->getNick(), chan_name) + "\r\n");
                }
                continue;
            }
            
            if (!channel->addMember(client, key))
            {
                continue;
            }
        }
        else
        {
            channel = new Channel(chan_name, client);
            _channels[chan_name] = channel;
            std::cout << "Created new channel: " << chan_name 
                      << " by " << client->getNick() << std::endl;
        }
        std::string join_msg = ":" + client->getNick() + "!" + client->getUser() 
                             + "@localhost JOIN " + chan_name + "\r\n";
        channel->broadcastMessage(join_msg);
        if (!channel->getTopic().empty())
        {
            client->sendMessage(RPL_TOPIC(client->getNick(), chan_name, channel->getTopic()) + "\r\n");
        }
        else
        {
            client->sendMessage(RPL_NOTOPIC(client->getNick(), chan_name) + "\r\n");
        }

        std::string member_list = channel->getMemberList();
        client->sendMessage(RPL_NAMREPLY(client->getNick(), chan_name, member_list) + "\r\n");
        client->sendMessage(RPL_ENDOFNAMES(client->getNick(), chan_name) + "\r\n");
        
        client->addChannel(chName);        
        std::cout << "Client " << client->getNick() 
                  << " joined channel " << chan_name << std::endl;
    }
}