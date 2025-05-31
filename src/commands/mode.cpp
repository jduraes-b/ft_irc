/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rcosta-c <rcosta-c@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/30 14:08:21 by rcosta-c          #+#    #+#             */
/*   Updated: 2025/05/31 01:12:04 by rcosta-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "channel.hpp"
#include "server.hpp"
#include "utils/utils.hpp"

/**
 * @brief MODE - Change channel or user modes
 * 
 * @param client_fd File descriptor of the client sending the command
 * @param params Target (channel/user), mode changes, and mode parameters
 * 
 * @details Sets or removes modes on channels or users. Channel modes control channel
 *          behavior and access. This implementation focuses on channel modes.
 * 
 * @note Syntax: MODE <target> [{+|-}<modes> [<mode params>]]
 * @note Requires operator status for channel mode changes
 * 
 * @supported Channel modes:
 * - i: Invite-only channel
 * - t: Topic settable by operators only
 * - k: Channel requires key to join
 * - o: Give/take channel operator status
 * - l: Set user limit on channel
 * 
 * @errors
 * - ERR_NEEDMOREPARAMS (461): Missing required parameters
 * - ERR_NOSUCHCHANNEL (403): Channel doesn't exist
 * - ERR_CHANOPRIVSNEEDED (482): Client is not channel operator
 * - ERR_UNKNOWNMODE (472): Unknown mode character
 * 
 * @behavior
 * - Without modes: Returns current channel modes (RPL_CHANNELMODEIS)
 * - With modes: Changes modes and broadcasts to channel
 * - Mode parameters are consumed in order as needed
 * 
 * @example
 * - MODE #channel +i
 * - MODE #channel +kl password 20
 * - MODE #channel +o Alice
 * - MODE #channel -i+m
 */

void Server::modeCommand(int client_fd, const std::string &params)
{
    Client* client = getClientByFd(client_fd);
    if (!client || !client->isAuthenticated())
    {
        if (client)
            client->sendMessage(":irc.local 451 * :You have not registered\r\n");
        return;
    }
    std::istringstream iss(params);
    std::string target, modes_str;
    iss >> target >> modes_str;    
    if (target.empty())
    {
        client->sendMessage(ERR_NEEDMOREPARAMS(client->getNick(), "MODE") + "\r\n");
        return;
    }
    if (target[0] != '#' && target[0] != '&')
    {
        return;
    }
    
    std::map<std::string, Channel*>::iterator chan_it = _channels.find(target);
    if (chan_it == _channels.end())
    {
        client->sendMessage(ERR_NOSUCHCHANNEL(client->getNick(), target) + "\r\n");
        return;
    }
    Channel* channel = chan_it->second;
    if (modes_str.empty())
    {
        client->sendMessage(RPL_CHANNELMODEIS(client->getNick(), target, channel->getModes()) + "\r\n");
        return;
    }
    
    if (!channel->isOperator(client))
    {
        client->sendMessage(ERR_CHANOPRIVSNEEDED(client->getNick(), target) + "\r\n");
        return;
    }
    bool adding = true;
    std::string mode_changes;
    std::string mode_params;
    std::vector<std::string> params_list;
    std::string param;
    while (iss >> param)
    {
        params_list.push_back(param);
    }
    size_t param_index = 0;
    for (size_t i = 0; i < modes_str.length(); ++i)
    {
        char mode = modes_str[i];
        
        if (mode == '+')
        {
            adding = true;
            mode_changes += "+";
        }
        else if (mode == '-')
        {
            adding = false;
            mode_changes += "-";
        }
        else
        {
            bool mode_changed = false;
            (void) mode_changed;
            switch (mode)
            {
                case 'i': // invite-only
                case 't': // topic restricted
                    if (channel->getMode(mode) != adding)
                    {
                        channel->setMode(mode, adding);
                        mode_changes += mode;
                        mode_changed = true;
                    }
                    break;
                    
                case 'k': // channel key
                    if (adding)
                    {
                        if (param_index < params_list.size())
                        {
                            channel->setChannelKey(params_list[param_index]);
                            mode_changes += mode;
                            mode_params += " " + params_list[param_index];
                            param_index++;
                            mode_changed = true;
                        }
                    }
                    else
                    {
                        channel->removeChannelKey();
                        mode_changes += mode;
                        mode_changed = true;
                    }
                    break;
                    
                case 'o': // operator status
                    if (param_index < params_list.size())
                    {
                        Client* target_client = NULL;
                        for (std::vector<Client*>::iterator it = _clients.begin(); 
                             it != _clients.end(); ++it)
                        {
                            if ((*it)->getNick() == params_list[param_index])
                            {
                                target_client = *it;
                                break;
                            }
                        }
                        
                        if (target_client && channel->isMember(target_client))
                        {
                            if (adding)
                                channel->addOperator(target_client);
                            else
                                channel->removeOperator(target_client);
                            
                            mode_changes += mode;
                            mode_params += " " + params_list[param_index];
                            mode_changed = true;
                        }
                        param_index++;
                    }
                    break;   
                case 'l': // user limit
                    if (adding)
                    {
                        if (param_index < params_list.size())
                        {
                            std::istringstream limit_iss(params_list[param_index]);
                            size_t limit;
                            if (limit_iss >> limit)
                            {
                                channel->setUserLimit(limit);
                                mode_changes += mode;
                                mode_params += " " + params_list[param_index];
                                mode_changed = true;
                            }
                            param_index++;
                        }
                    }
                    else
                    {
                        channel->setUserLimit(0);
                        mode_changes += mode;
                        mode_changed = true;
                    }
                    break;
                    
                default:
                    client->sendMessage(ERR_UNKNOWNMODE(client->getNick(), 
                                      std::string(1, mode), target) + "\r\n");
                    break;
            }
        }
    }
    if (!mode_changes.empty() && mode_changes != "+-")
    {
        std::string mode_msg = ":" + client->getNick() + "!" + client->getUser() 
                             + "@localhost MODE " + target + " " + mode_changes + mode_params + "\r\n";
        channel->broadcastMessage(mode_msg);
        
        std::cout << client->getNick() << " set modes " << mode_changes 
                  << mode_params << " on " << target << std::endl;
    }
}