/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rcosta-c <rcosta-c@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/30 14:58:04 by rcosta-c          #+#    #+#             */
/*   Updated: 2025/05/31 01:11:38 by rcosta-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <set>
#include <map>
#include <sstream>
#include <cstring>
#include <cctype>
#include <sys/types.h>
#include <sys/socket.h>
#include <cerrno>
#include <sys/epoll.h>
#include <memory>
#include <unistd.h>
#include <stdexcept>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

// Numeric replies
#define RPL_AWAY(client, nick, message) \
    ":irc.local 301 " + client + " " + nick + " :" + message

#define RPL_ENDOFWHO(client, name) \
    ":irc.local 315 " + client + " " + name + " :End of WHO list"

#define RPL_CHANNELMODEIS(client, channel, modes) \
    ":irc.local 324 " + client + " " + channel + " " + modes
    
#define RPL_NOTOPIC(client, channel) \
    ":irc.local 331 " + client + " " + channel + " :No topic is set"

#define RPL_TOPIC(client, channel, topic) \
    ":irc.local 332 " + client + " " + channel + " :" + topic

#define RPL_TOPICWHOTIME(client, channel, nick, time) \
    ":irc.local 333 " + client + " " + channel + " " + nick + " " + time
    
#define RPL_INVITING(client, nick, channel) \
    ":irc.local 341 " + client + " " + nick + " " + channel

#define RPL_WHOREPLY(client, channel, user, host, server, nick, flags, realname) \
    ":irc.local 352 " + client + " " + channel + " " + user + " " + host + " " + \
    server + " " + nick + " " + flags + " :0 " + realname

#define RPL_NAMREPLY(client, channel, names) \
    ":irc.local 353 " + client + " = " + channel + " :" + names

#define RPL_ENDOFNAMES(client, channel) \
    ":irc.local 366 " + client + " " + channel + " :End of /NAMES list"





// Error replies

#define ERR_NOSUCHNICK(client, nick) \
    ":irc.local 401 " + client + " " + nick + " :No such nick/channel"

#define ERR_NOSUCHCHANNEL(client, channel) \
    ":irc.local 403 " + client + " " + channel + " :No such channel"

#define ERR_CANNOTSENDTOCHAN(client, channel) \
    ":irc.local 404 " + client + " " + channel + " :Cannot send to channel"

#define ERR_TOOMANYCHANNELS(client, channel) \
    ":irc.local 405 " + client + " " + channel + " :You have joined too many channels"

#define ERR_NORECIPIENT(client, command) \
    ":irc.local 411 " + client + " :No recipient given (" + command + ")"

#define ERR_NOTEXTTOSEND(client) \
    ":irc.local 412 " + client + " :No text to send"

#define ERR_NONICKNAMEGIVEN(client) \
    ":irc.local 431 " + client + " :No nickname given"

#define ERR_ERRONEUSNICKNAME(client, nick) \
    ":irc.local 432 " + client + " " + nick + " :Erroneous nickname"

#define ERR_NICKNAMEINUSE(client, nick) \
    ":irc.local 433 " + client + " " + nick + " :Nickname is already in use"

#define ERR_USERNOTINCHANNEL(client, nick, channel) \
    ":irc.local 441 " + client + " " + nick + " " + channel + " :They aren't on that channel"

#define ERR_NOTONCHANNEL(client, channel) \
    ":irc.local 442 " + client + " " + channel + " :You're not on that channel"

#define ERR_USERONCHANNEL(client, nick, channel) \
    ":irc.local 443 " + client + " " + nick + " " + channel + " :is already on channel"

#define ERR_NOTREGISTERED(client) \
    ":irc.local 451 " + client + " :You have not registered"

#define ERR_NEEDMOREPARAMS(client, command) \
    ":irc.local 461 " + client + " " + command + " :Not enough parameters"

#define ERR_ALREADYREGISTRED(client) \
    ":irc.local 462 " + client + " :Unauthorized command (already registered)"

#define ERR_KEYSET(client, channel) \
    ":irc.local 467 " + client + " " + channel + " :Channel key already set"

#define ERR_BADCHANNELKEY(client, channel) \
    ":irc.local 475 " + client + " " + channel + " :Cannot join channel (+k)"

#define ERR_INVITEONLYCHAN(client, channel) \
    ":irc.local 473 " + client + " " + channel + " :Cannot join channel (+i)"

#define ERR_CHANNELISFULL(client, channel) \
    ":irc.local 471 " + client + " " + channel + " :Cannot join channel (+l)"

#define ERR_UNKNOWNMODE(client, mode, channel) \
    ":irc.local 472 " + client + " " + mode + " :is unknown mode char to me for " + channel

#define ERR_CHANOPRIVSNEEDED(client, channel) \
    ":irc.local 482 " + client + " " + channel + " :You're not channel operator"

    


// Utility function declarations
std::string trim(const std::string &str);
std::string toLower(const std::string &str);
std::string toUpper(const std::string &str);
void logMessage(const std::string &message);

#endif // UTILS_HPP