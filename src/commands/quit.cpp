/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rcosta-c <rcosta-c@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/30 14:10:13 by rcosta-c          #+#    #+#             */
/*   Updated: 2025/07/23 00:13:08 by rcosta-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "channel.hpp"
#include "server.hpp"
#include "utils/utils.hpp"


/**
 * @brief QUIT - Disconnect from the IRC server
 * 
 * @param client_fd File descriptor of the client sending the command
 * @param params Optional quit message
 * 
 * @details Closes the client's connection to the server. Removes the client from all
 *          channels and notifies other users in shared channels about the departure.
 *          Also handles unexpected disconnections.
 * 
 * @note Syntax: QUIT [:<quit message>]
 * @note Always succeeds (no errors)
 * 
 * @behavior
 * - Sends QUIT message to all users in shared channels (only once per user)
 * - Removes client from all channels
 * - Deletes empty channels
 * - Sends ERROR message to quitting client
 * - Frees all client resources
 * 
 * @example
 * - QUIT
 * - QUIT :Going to sleep
 * - QUIT :See you tomorrow!
 * 
 */

void Server::quitCommand(int client_fd, const std::string &params)
{
    Client* client = getClientByFd(client_fd);
    if (!client) return;
    
    std::string quit_message = params;
    if (!quit_message.empty() && quit_message[0] == ':')
        quit_message = quit_message.substr(1);
    if (quit_message.empty())
        quit_message = "Client Quit";
    
    std::cout << "Client " << client->getNick() 
              << " (fd: " << client_fd << ") is quitting: " 
              << quit_message << std::endl;
    
    std::string quit_msg = ":" + client->getNick() + "!" + client->getUser() 
                         + "@localhost QUIT :" + quit_message + "\r\n";
    
    std::vector<std::string> client_channels = client->getChannels();
    std::set<Client*> notified_clients; 
    for (std::vector<std::string>::const_iterator it = client_channels.begin();
         it != client_channels.end(); ++it)
    {
        std::map<std::string, Channel*>::iterator chan_it = _channels.find(*it);
        if (chan_it != _channels.end())
        {
            Channel* channel = chan_it->second;
            
            std::vector<Client*> members = channel->getMembers();
            for (std::vector<Client*>::const_iterator member_it = members.begin();
                 member_it != members.end(); ++member_it)
            {
                if (*member_it != client)
                {
                    notified_clients.insert(*member_it);
                }
            }
            channel->removeMember(client);
            if (channel->getMemberCount() == 0)
            {
                std::cout << "Channel " << *it << " is now empty, removing..." << std::endl;
                delete channel;
                _channels.erase(chan_it);
            }
        }
    }
    
    for (std::set<Client*>::iterator it = notified_clients.begin();
         it != notified_clients.end(); ++it)
    {
        try
        {
            (*it)->sendMessage(quit_msg);
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error sending QUIT message to client: " << e.what() << std::endl;
        }
    }
    client->clearChannels();
    try
    {
        client->sendMessage("ERROR :Closing Link: " + client->getNick() 
                          + " (Quit: " + quit_message + ")\r\n");
    }
    catch (const std::exception& e)
    {
    }

//    shutdown(client_fd, SHUT_WR);
//    usleep(10000);
//    close(client_fd);
    client->setShouldQuit(true);
    std::cout << "Client disconnected and cleaned up successfully" << std::endl;
}