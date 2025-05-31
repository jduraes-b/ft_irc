/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rcosta-c <rcosta-c@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/30 14:06:27 by rcosta-c          #+#    #+#             */
/*   Updated: 2025/05/31 01:11:23 by rcosta-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "channel.hpp"
#include "server.hpp"
#include "utils/utils.hpp"

/**
 * @brief PART - Leave one or more channels
 * 
 * @param client_fd File descriptor of the client sending the command
 * @param params Channel names (comma-separated) and optional part message
 * 
 * @details Removes the client from the specified channels. An optional part message
 *          can be included which will be sent to all channel members. If the channel
 *          becomes empty after the client leaves, it will be deleted.
 * 
 * @note Syntax: PART <channel>{,<channel>} [:<part message>]
 * 
 * @errors
 * - ERR_NEEDMOREPARAMS (461): No channel specified
 * - ERR_NOSUCHCHANNEL (403): Channel doesn't exist
 * - ERR_NOTONCHANNEL (442): Client is not on the specified channel
 * 
 * @behavior
 * - Sends PART message to all channel members (including the leaving client)
 * - Removes client from channel member list
 * - Deletes channel if it becomes empty
 * 
 * @example
 * - PART #general
 * - PART #chat :Going to lunch
 * - PART #chan1,#chan2 :Goodbye everyone!
 */

void Server::partCommand(int client_fd, const std::string &params)
{
    Client* client = getClientByFd(client_fd);
    if (!client) return;
    
    if (!client->isAuthenticated())
    {
        client->sendMessage(ERR_NOTREGISTERED(client->getNick()) + "\r\n");
        return;
    }
    if (params.empty())
    {
        client->sendMessage(ERR_NEEDMOREPARAMS(client->getNick(), "PART") + "\r\n");
        return;
    }
    
    std::istringstream iss(params);
    std::string channels_str;
    std::string part_message;
    
    iss >> channels_str;
    
    // Procura se tem mensagem de saída (:) (opcional)
    size_t msg_pos = params.find(" :");
    if (msg_pos != std::string::npos)
    {
        part_message = params.substr(msg_pos + 2);
    }
    else
    {
        std::getline(iss, part_message);
        size_t first_char = part_message.find_first_not_of(" \t");
        if (first_char != std::string::npos)
            part_message = part_message.substr(first_char);
    }
    
    if (part_message.empty())
        part_message = client->getNick();
    
    std::stringstream channel_stream(channels_str);
    std::string channel_name;
    
    while (std::getline(channel_stream, channel_name, ','))
    {
        if (channel_name.empty())
            continue;
        
        std::map<std::string, Channel*>::iterator it = _channels.find(channel_name);
        if (it == _channels.end())
        {
            client->sendMessage(ERR_NOSUCHCHANNEL(client->getNick(), channel_name) + "\r\n");
            continue;
        }
        Channel* channel = it->second;
    
        if (!channel->isMember(client))
        {
            client->sendMessage(ERR_NOTONCHANNEL(client->getNick(), channel_name) + "\r\n");
            continue;
        }
        
        std::string part_msg = ":" + client->getNick() + "!" + client->getUser() 
                             + "@localhost PART " + channel_name;
        if (!part_message.empty())
            part_msg += " :" + part_message;
        part_msg += "\r\n";
        
        channel->broadcastMessage(part_msg);   
        channel->removeMember(client);
        client->removeChannel(channel_name);
        
        std::cout << "Client " << client->getNick() 
                  << " left channel " << channel_name 
                  << " (" << part_message << ")" << std::endl;
        
        if (channel->getMemberCount() == 0)
        {
            std::cout << "Channel " << channel_name << " is now empty, removing..." << std::endl;
            delete channel;
            _channels.erase(it);
        }
    }
}