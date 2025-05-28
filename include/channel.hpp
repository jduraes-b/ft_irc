/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rcosta-c <rcosta-c@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 23:01:45 by rcosta-c          #+#    #+#             */
/*   Updated: 2025/05/28 12:01:02 by rcosta-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "utils/utils.hpp"
#include "client.hpp"


class Channel
{
	public:
	Channel(const std::string &name, Client *creator);
	~Channel();

	//Getters básicos
	std::string getName() const;
	std::string getTopic() const;
	size_t getMemberCount() const;
	time_t getCreationTime() const;
	
	//Topic
	void setTopic(const std::string &topic, Client *setter);
	bool canSetTopic(Client *client) const;
	
	//Member management
	bool addMember(Client *client, const std::string &key = "");
	void removeMember(Client *client);
	bool isMember(Client *client) const;
	std::vector<Client*> getMembers() const;
	
	//Message
	void broadcastMessage(const std::string &message, Client *sender = NULL);
	void sendMessage(const std::string &message, Client *sender, Client *exclude = NULL);
	
	// Utility
	std::string getMemberList() const;
	std::string getChannelInfo() const;

    // User's Type
    bool isOperator(Client *client) const;
    void addOperator(Client *client);
    void removeOperator(Client *client);

    // Channel modes
    void setMode(char mode, bool value);
    bool getMode(char mode) const;
    void setChannelKey(const std::string &key);
    void removeChannelKey();
    void setUserLimit(size_t limit);
    size_t getUserLimit() const;
    std::string getModes() const;


    // Invite management
    void addInvite(Client *client);
    void removeInvite(Client *client);
    bool isInvited(Client *client) const;
    
    // Permissions
    bool canSendMessage(Client *client) const;
    bool canJoin(Client *client, const std::string &key = "") const;


    
	private:
		
		std::string _name;
		std::string _topic;
        std::string _topicSetter;
            
        std::vector<Client*> _members;
        std::set<Client*> _operators;
        std::set<Client*> _invitedUsers;
        
		Client* _creator;
		time_t _creationTime;

		bool _inviteOnly;           // +i invite only
		bool _topicRestricted;      // +t só ops podem mudar tópico
		bool _channelKey;		  // +k canal protegido por password
		bool _operatorPrivilege;  // +o canal com privilégios de operador
		size_t _userLimit;          // +l limite de utilizadores(0 = sem limite)

        time_t _topicTime;
        std::string _key;           // Password do canal (quando +k está ativo)


};