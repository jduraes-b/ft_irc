/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rcosta-c <rcosta-c@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/30 14:07:52 by rcosta-c          #+#    #+#             */
/*   Updated: 2025/05/30 23:33:45 by rcosta-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "channel.hpp"
#include "server.hpp"
#include "utils/utils.hpp"

/**
 * @brief TOPIC - View or change channel topic
 * 
 * @param client_fd File descriptor of the client sending the command
 * @param params Channel name and optional new topic
 * 
 * @details Without a new topic, shows the current channel topic. With a new topic,
 *          sets the channel topic (if permitted). On +t channels, only operators
 *          can change the topic.
 * 
 * @note Syntax: TOPIC <channel> [:<new topic>]
 * 
 * @errors
 * - ERR_NEEDMOREPARAMS (461): No channel specified
 * - ERR_NOSUCHCHANNEL (403): Channel doesn't exist
 * - ERR_NOTONCHANNEL (442): Client is not on the channel
 * - ERR_CHANOPRIVSNEEDED (482): Channel is +t and client is not operator
 * 
 * @behavior
 * - Query: Returns RPL_TOPIC (332) or RPL_NOTOPIC (331)
 * - Set: Broadcasts topic change to all channel members
 * 
 * @example
 * - TOPIC #general
 * - TOPIC #general :Welcome to the general discussion channel!
 */

void Server::topicCommand(int client_fd, const std::string &params)
{
    Client* client = getClientByFd(client_fd);
    if (!client || !client->isAuthenticated())
    {
        if (client)
            client->sendMessage(":irc.local 451 * :You have not registered\r\n");
        return;
    }
    
    std::istringstream iss(params);
    std::string channel_name;
    iss >> channel_name;   
    if (channel_name.empty())
    {
        client->sendMessage(ERR_NEEDMOREPARAMS(client->getNick(), "TOPIC") + "\r\n");
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
    size_t topic_pos = params.find(" :");
    if (topic_pos == std::string::npos)
    {
        if (channel->getTopic().empty())
        {
            client->sendMessage(RPL_NOTOPIC(client->getNick(), channel_name) + "\r\n");
        }
        else
        {
            client->sendMessage(RPL_TOPIC(client->getNick(), channel_name, channel->getTopic()) + "\r\n");
        }
        return;
    }
    std::string new_topic = params.substr(topic_pos + 2);
    if (!channel->canSetTopic(client))
    {
        client->sendMessage(ERR_CHANOPRIVSNEEDED(client->getNick(), channel_name) + "\r\n");
        return;
    }
    channel->setTopic(new_topic, client);
    std::string topic_msg = ":" + client->getNick() + "!" + client->getUser() 
                          + "@localhost TOPIC " + channel_name + " :" + new_topic + "\r\n";
    channel->broadcastMessage(topic_msg);
    
    std::cout << client->getNick() << " changed topic of " << channel_name 
              << " to: " << new_topic << std::endl;
    
}