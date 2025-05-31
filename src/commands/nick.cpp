/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rcosta-c <rcosta-c@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/30 14:09:05 by rcosta-c          #+#    #+#             */
/*   Updated: 2025/05/30 23:34:12 by rcosta-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"
#include "server.hpp"
#include "utils/utils.hpp"
#include "channel.hpp"


/**
 * @brief NICK - Nickname command to set or change client nickname
 * 
 * @param client_fd File descriptor of the client sending the command
 * @param params Desired nickname (max 9 characters)
 * 
 * @details Used to give the client a nickname or change the previous one. The nickname
 *          must be unique on the server. If the client is already registered, changing
 *          the nickname will notify all channels the client is in.
 * 
 * @note Syntax: NICK <nickname>
 * @note Valid characters: a-z, A-Z, 0-9, -, _, [, ], {, }, \, |, ^
 * @note First character cannot be a digit
 * 
 * @errors
 * - ERR_NONICKNAMEGIVEN (431): No nickname parameter provided
 * - ERR_ERRONEUSNICKNAME (432): Invalid nickname format
 * - ERR_NICKNAMEINUSE (433): Nickname is already taken by another client
 * 
 * @behavior
 * - Pre-registration: Sets initial nickname
 * - Post-registration: Changes nickname and notifies all relevant channels
 * 
 * @example
 * - NICK Alice
 * - NICK [Bob]
 * - NICK User|AFK
 */

 
bool isValidNickChar(char c, bool first_char)
{
    if (first_char)
    {
        return std::isalpha(c) || c == '[' || c == ']' || c == '{' || 
               c == '}' || c == '\\' || c == '|' || c == '_' || c == '^';
    }
    return std::isalnum(c) || c == '-' || c == '[' || c == ']' || 
           c == '{' || c == '}' || c == '\\' || c == '|' || c == '_' || c == '^';
}

void Server::nickCommand(int client_fd, const std::string& params)
{
    Client* client = getClientByFd(client_fd);
    if (!client)
        return;
    if (params.empty())
    {
        std::string current = client->getNick().empty() ? "*" : client->getNick();
        client->sendMessage(ERR_NONICKNAMEGIVEN(current) + "\r\n");
        return;
    }
    std::istringstream iss(params);
    std::string new_nick;
    iss >> new_nick;
    
    std::string old_nick = client->getNick();
    std::string display_nick = old_nick.empty() ? "*" : old_nick;
    if (new_nick.length() > 9)
    {
        client->sendMessage(ERR_ERRONEUSNICKNAME(display_nick, new_nick) + "\r\n");
        return;
    }
    
    for (size_t i = 0; i < new_nick.length(); ++i)
    {
        if (!isValidNickChar(new_nick[i], i == 0))
        {
            client->sendMessage(ERR_ERRONEUSNICKNAME(display_nick, new_nick) + "\r\n");
            return;
        }
    }
    if (std::isdigit(new_nick[0]))
    {
        client->sendMessage(ERR_ERRONEUSNICKNAME(display_nick, new_nick) + "\r\n");
        return;
    }
    if (new_nick == old_nick)
    {
        return;
    }
    for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (*it != client && (*it)->getNick() == new_nick)
        {
            client->sendMessage(ERR_NICKNAMEINUSE(display_nick, new_nick) + "\r\n");
            return;
        }
    }
    
    if (client->isAuthenticated() && !old_nick.empty())
    {
        std::string nick_change_msg = ":" + old_nick + "!" + client->getUser() 
                                    + "@localhost NICK :" + new_nick + "\r\n";
        
        client->sendMessage(nick_change_msg);
        
        std::set<Client*> clients_to_notify;
        std::vector<std::string> channels = client->getChannels();
        for (std::vector<std::string>::const_iterator chan_it = channels.begin();
             chan_it != channels.end(); ++chan_it)
        {
            std::map<std::string, Channel*>::iterator channel_it = _channels.find(*chan_it);
            if (channel_it != _channels.end())
            {
                std::vector<Client*> members = channel_it->second->getMembers();
                for (std::vector<Client*>::const_iterator member_it = members.begin();
                     member_it != members.end(); ++member_it)
                {
                    if (*member_it != client)
                    {
                        clients_to_notify.insert(*member_it);
                    }
                }
            }
        }
        for (std::set<Client*>::iterator it = clients_to_notify.begin();
             it != clients_to_notify.end(); ++it)
        {
            (*it)->sendMessage(nick_change_msg);
        }
        
        std::cout << "Client " << old_nick << " changed nickname to " << new_nick << std::endl;
    }
    
    client->setNick(new_nick);
    if (!client->isAuthenticated())
    {
        if (!client->getNick().empty() && !client->getUser().empty())
        {
            if (client->getPass().empty())
            {
                std::cout << "Client " << client->getNick() 
                          << " missing password for authentication" << std::endl;
            }
            else if (client->getPass() != _pass)
            {
                std::cout << "Client " << client->getNick() 
                          << " provided incorrect password" << std::endl;
                client->sendMessage(":irc.local 464 " + client->getNick() 
                                  + " :Password incorrect\r\n");
            }
            else
            {
                client->authenticate();
                std::string welcome = ":irc.local 001 " + client->getNick() 
                                    + " :Welcome to the IRC Network " + client->getNick() 
                                    + "!" + client->getUser() + "@localhost\r\n";
                client->sendMessage(welcome);              
                std::cout << "Client authenticated successfully: " << client->getNick() 
                          << " (" << client->getUser() << ") from fd " << client_fd << std::endl;
            }
        }
    }
}
