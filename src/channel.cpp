/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rcosta-c <rcosta-c@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 11:04:06 by rcosta-c          #+#    #+#             */
/*   Updated: 2025/05/29 12:32:56 by rcosta-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "channel.hpp"
#include "client.hpp"
#include "utils/utils.hpp"


Channel::Channel(const std::string &name, Client *creator) 
    : _name(name), 
      _creator(creator),
      _creationTime(time(NULL)),
      _inviteOnly(false),
      _topicRestricted(true),
      _channelKey(false),
      _operatorPrivilege(false),
      _userLimit(0),
      _topicTime(0)
{
    // Adicionar o criador como membro e operador
    _members.push_back(creator);
    _operators.insert(creator);
}

Channel::~Channel()
{}


// Getters básicos
std::string Channel::getName() const
{
    return (this->_name);
}

std::string Channel::getTopic() const
{
    return (this->_topic);
}

size_t Channel::getMemberCount() const
{
    return (this->_members.size());
}

time_t Channel::getCreationTime() const
{
    return (this->_creationTime);
}

std::string Channel::getKey() const
{
    return (this->_key);
}

// Topic
void Channel::setTopic(const std::string& topic, Client* setter)
{
    if(canSetTopic(setter))
    {
        this->_topic = topic;
        this->_topicSetter = setter->getNick();
        this->_topicTime = time(NULL);
    }
}

bool Channel::canSetTopic(Client* client) const
{
    // Se o modo +t está ativo, apenas operadores podem mudar o tópico
    if (_topicRestricted)
    {
        return isOperator(client);
    }
    // Caso contrário, qualquer membro pode mudar
    return isMember(client);
}

// Member management
bool Channel::addMember(Client* client, const std::string& key)
{
    // Verificar se já é membro
    if (isMember(client))
        return false;
    
    // Verificar se o canal tem limite de utilizadores
    if (_userLimit > 0 && _members.size() >= _userLimit)
        return false;
    
    // Verificar se o canal é invite-only
    if (_inviteOnly && !isInvited(client))
        return false;
    
    // Verificar a password do canal
    if (_channelKey && key != _key)
        return false;
    
    // Adicionar o membro
    _members.push_back(client);
    
    // Remover dos convites se estava convidado
    if (isInvited(client))
        removeInvite(client);
    
    return true;
}
void Channel::removeMember(Client* client)
{
    // Remover dos membros
    std::vector<Client*>::iterator it = std::find(_members.begin(), _members.end(), client);
    if (it != _members.end())
        _members.erase(it);
    
    // Remover dos operadores se for operador
    _operators.erase(client);
    
    // Remover dos convidados se estiver convidado
    _invitedUsers.erase(client);
}

bool Channel::isMember(Client* client) const
{
    return std::find(_members.begin(), _members.end(), client) != _members.end();
}

std::vector<Client*> Channel::getMembers() const
{
    return (this->_members);
}

// Hierarquia de utilizadores
bool Channel::isOperator(Client *client) const
{
    return _operators.find(client) != _operators.end();
}

void Channel::addOperator(Client *client)
{
    if (isMember(client))
        _operators.insert(client);
}

void Channel::removeOperator(Client *client)
{
    _operators.erase(client);
}

// Channel modes
void Channel::setMode(char mode, bool value)
{
    switch(mode)
    {
        case 'i':
            _inviteOnly = value;
            break;
        case 't':
            _topicRestricted = value;
            break;
        case 'k':
            _channelKey = value;
            if (!value)
                _key.clear();
            break;
        case 'o':
            _operatorPrivilege = value;
            break;
        case 'l':
            if (!value)
                _userLimit = 0;
            break;
    }
}

bool Channel::getMode(char mode) const
{
    switch(mode)
    {
        case 'i': return _inviteOnly;
        case 't': return _topicRestricted;
        case 'k': return _channelKey;
        case 'o': return _operatorPrivilege;
        case 'l': return (_userLimit > 0);
        default: return false;
    }
}

void Channel::setChannelKey(const std::string &key)
{
    _key = key;
    _channelKey = true;
}

void Channel::removeChannelKey()
{
    _key.clear();
    _channelKey = false;
}

void Channel::setUserLimit(size_t limit)
{
    _userLimit = limit;
}

size_t Channel::getUserLimit() const
{
    return _userLimit;
}

std::string Channel::getModes() const
{
    std::string modes = "+";
    if (_inviteOnly) modes += "i";
    if (_topicRestricted) modes += "t";
    if (_channelKey) modes += "k";
    if (_operatorPrivilege) modes += "o";
    if (_userLimit > 0) modes += "l";
    
    // Adicionar parâmetros dos modos
    if (_channelKey) modes += " " + _key;
    if (_userLimit > 0)
    {
        std::stringstream ss;
        ss << " " << _userLimit;
        modes += ss.str();
    }
    
    return modes;
}


// Invite management
void Channel::addInvite(Client *client)
{
    _invitedUsers.insert(client);
}

void Channel::removeInvite(Client *client)
{
    _invitedUsers.erase(client);
}

bool Channel::isInvited(Client *client) const
{
    return _invitedUsers.find(client) != _invitedUsers.end();
}

// Permissions
bool Channel::canSendMessage(Client *client) const
{
   return isMember(client);
}

bool Channel::canJoin(Client *client, const std::string &key) const
{
    // Já é membro
    if (isMember(client))
        return true;
    
    // Verificar limite de utilizadores
    if (_userLimit > 0 && _members.size() >= _userLimit)
        return false;
    
    // Verificar invite-only
    if (_inviteOnly && !isInvited(client))
        return false;
    
    // Verificar password
    if (_channelKey && key != _key)
        return false;
    
    return true;
}

// Message
void Channel::broadcastMessage(const std::string& message, Client* sender)
{
    for (std::vector<Client*>::iterator it = _members.begin(); it != _members.end(); ++it)
    {
        if (*it != sender)
        {
            (*it)->sendMessage(message);
        }
    }
}

void Channel::sendMessage(const std::string& message, Client* sender, Client* exclude)
{
    // Verificar se o sender pode enviar mensagens
    if (!canSendMessage(sender))
        return;
    
    // Enviar mensagem para todos os membros exceto o exclude
    for (std::vector<Client*>::iterator it = _members.begin(); it != _members.end(); ++it)
    {
        if (*it != exclude)
        {
            (*it)->sendMessage(message);
        }
    }
}

// Utility
std::string Channel::getMemberList() const
{
    std::string list;
    for (std::vector<Client*>::const_iterator it = _members.begin(); it != _members.end(); ++it)
    {
        if (!list.empty())
            list += " ";
        
        // Adicionar prefixo @ para operadores
        if (isOperator(*it))
            list += "@";
        
        list += (*it)->getNick();
    }
    return list;
}

std::string Channel::getChannelInfo() const
{
    std::stringstream channelInfo;
    channelInfo << "Channel: " << _name << " (" << _members.size() << " members)";
    if (!_topic.empty())
        channelInfo << " - Topic: " << _topic;
    channelInfo << " - Modes: " << getModes();
    return channelInfo.str();
}