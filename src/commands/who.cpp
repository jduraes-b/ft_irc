/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   who.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rcosta-c <rcosta-c@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/30 14:10:48 by rcosta-c          #+#    #+#             */
/*   Updated: 2025/05/31 13:20:37 by rcosta-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "channel.hpp"
#include "server.hpp"
#include "utils/utils.hpp"

/**
 * @brief WHO - Query information about users
 * 
 * @param client_fd File descriptor of the client sending the command
 * @param params Optional mask (channel name, nickname, or empty)
 * 
 * @details The WHO command is used to query a list of users who match the provided
 *          mask. The mask can be a channel name to list all users in that channel,
 *          a nickname to get information about a specific user, or omitted to list
 *          all visible users.
 * 
 * @note Syntax: WHO [<mask>]
 * @note Mask can be:
 *       - Channel name (#channel): Lists all users in the channel
 *       - Nickname: Shows information about that specific user
 *       - Empty: Lists all users in channels shared with the requester
 * 
 * @errors
 * - ERR_NOSUCHNICK (401): Specified nickname doesn't exist
 * - ERR_NOSUCHCHANNEL (403): Specified channel doesn't exist
 * 
 * @behavior
 * - WHO #channel: Lists all members of the specified channel
 * - WHO nickname: Shows information about the specified user
 * - WHO (no params): Lists all users visible to the requester
 *   - "Visible" means users who share at least one channel with requester
 *   - Prevents users from seeing the entire server user list
 * 
 * @replies
 * - RPL_WHOREPLY (352): One per matching user
 *   Format: <channel> <user> <host> <server> <nick> <flags> :<hopcount> <realname>
 *   - channel: Channel where user was found (* if no shared channel)
 *   - user: Username
 *   - host: Hostname (typically "localhost")
 *   - server: Server name (typically "irc.local")
 *   - nick: Nickname
 *   - flags: Status flags (H = Here, @ = Operator)
 *   - hopcount: Distance from server (always 0 for local server)
 *   - realname: Real name (using username as fallback)
 * - RPL_ENDOFWHO (315): Marks end of WHO reply list
 * 
 * @flags User status flags:
 * - H: Here (not away) - always set in this implementation
 * - G: Gone (away) - not implemented
 * - @: Channel operator (only shown if user is op in displayed channel)
 * - +: Has voice - not implemented
 * - *: IRC operator - not implemented
 * 
 * @privacy
 * - Users can only see others who share at least one channel
 * - Prevents information leakage about users in private channels
 * - Channel operators shown with @ only in relevant channel context
 * 
 * @example
 * - WHO
 *   Lists all users in your channels
 */ 

void Server::whoCommand(int client_fd, const std::string &params)
{
    Client* client = getClientByFd(client_fd);
    if (!client) return;
    
    if (!client->isAuthenticated())
    {
        client->sendMessage(ERR_NOTREGISTERED(client->getNick()) + "\r\n");
        return;
    }
    
    std::istringstream iss(params);
    std::string target;
    iss >> target;
    
    if (target.empty())
    {
        std::set<Client*> visible_clients;
        std::vector<std::string> my_channels = client->getChannels();
        for (std::vector<std::string>::const_iterator it = my_channels.begin();
             it != my_channels.end(); ++it)
        {
            std::map<std::string, Channel*>::iterator chan_it = _channels.find(*it);
            if (chan_it != _channels.end() && chan_it->second != NULL)
            {
                std::vector<Client*> members = chan_it->second->getMembers();
                for (std::vector<Client*>::const_iterator member_it = members.begin();
                     member_it != members.end(); ++member_it)
                {
                    visible_clients.insert(*member_it);
                }
            }
        }
        for (std::set<Client*>::iterator it = visible_clients.begin();
             it != visible_clients.end(); ++it)
        {
            Client* target_client = *it;
            std::string flags = "H"; // H = Here (not away)
            
            bool is_op = false;
            for (std::vector<std::string>::const_iterator chan_it = my_channels.begin();
                 chan_it != my_channels.end(); ++chan_it)
            {
                std::map<std::string, Channel*>::iterator channel_it = _channels.find(*chan_it);
                if (channel_it != _channels.end() && channel_it->second != NULL &&
                    channel_it->second->isMember(target_client) &&
                    channel_it->second->isOperator(target_client))
                {
                    is_op = true;
                    break;
                }
            }
            
            if (is_op)
                flags = "@" + flags;
            std::string who_reply = RPL_WHOREPLY(
                client->getNick(),
                "*",
                target_client->getUser(),
                "localhost",
                "irc.local",
                target_client->getNick(),
                flags,
                target_client->getUser()
            );
            client->sendMessage(who_reply + "\r\n");
        }
        client->sendMessage(RPL_ENDOFWHO(client->getNick(), "*") + "\r\n");
        return;
    }
    if (target[0] == '#' || target[0] == '&')
    {
        std::map<std::string, Channel*>::iterator chan_it = _channels.find(target);
        if (chan_it == _channels.end())
        {
            client->sendMessage(ERR_NOSUCHCHANNEL(client->getNick(), target) + "\r\n");
            return;
        }   
        Channel* channel = chan_it->second;
        std::vector<Client*> members = channel->getMembers();
        for (std::vector<Client*>::const_iterator it = members.begin();
             it != members.end(); ++it)
        {
            Client* member = *it;
            std::string flags = "H"; // H = Here (not away)
            if (channel->isOperator(member))
                flags = "@" + flags;
            std::string who_reply = RPL_WHOREPLY(
                client->getNick(),
                target,
                member->getUser(),
                "localhost",
                "irc.local",
                member->getNick(),
                flags,
                member->getUser()
            );
            client->sendMessage(who_reply + "\r\n");
        }        
        client->sendMessage(RPL_ENDOFWHO(client->getNick(), target) + "\r\n");
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
        std::string show_channel = "*";
        std::string flags = "H";
        
        std::vector<std::string> my_channels = client->getChannels();
        std::vector<std::string> target_channels = target_client->getChannels();
        
        for (std::vector<std::string>::const_iterator my_it = my_channels.begin();
             my_it != my_channels.end(); ++my_it)
        {
            for (std::vector<std::string>::const_iterator target_it = target_channels.begin();
                 target_it != target_channels.end(); ++target_it)
            {
                if (*my_it == *target_it)
                {
                    show_channel = *my_it;
                    std::map<std::string, Channel*>::iterator chan_it = _channels.find(*my_it);
                    if (chan_it != _channels.end() && chan_it->second->isOperator(target_client))
                    {
                        flags = "@" + flags;
                    }
                    break;
                }
            }
            if (show_channel != "*")
                break;
        }
        std::string who_reply = RPL_WHOREPLY(
            client->getNick(),
            show_channel,
            target_client->getUser(),
            "localhost",
            "irc.local",
            target_client->getNick(),
            flags,
            target_client->getUser()
        );
        client->sendMessage(who_reply + "\r\n");
        client->sendMessage(RPL_ENDOFWHO(client->getNick(), target) + "\r\n");
    }
}