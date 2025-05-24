/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rcosta-c <rcosta-c@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 23:01:45 by rcosta-c          #+#    #+#             */
/*   Updated: 2025/05/24 10:49:34 by rcosta-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "client.hpp"
#include <sstream>
#include <algorithm>

/*      TODO

	Implementar hierarquia dos utilizador (Op, Voice, membros normais)
		->definir gestão de dar Op/Voice, retirar Op/Voice, e verificação
	
	Modos do canal:
			+i (invite-only): Apenas convidados podem entrar
			+t (topic): Apenas ops podem mudar o tópico
			+n (no external): Apenas membros podem enviar mensagens
			+m (moderated): Apenas ops e voice podem falar
			+k (key): Canal protegido por password
			+l (limit): Limite de utilizadores definido(caso seja 0 então não há limite)
	

	Gestao de convites ?
	Gestão de limite de utilizadores
	Gestão da Password do canal

*/

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
	void setTopic(const std::string& topic, Client* setter);
	bool canSetTopic(Client* client) const;
	
	//Member management
	bool addMember(Client* client, const std::string& key = "");
	void removeMember(Client* client);
	bool isMember(Client* client) const;
	std::vector<Client*> getMembers() const;
	
	//Message
	void broadcastMessage(const std::string& message, Client* sender = NULL);
	void sendMessage(const std::string& message, Client* sender, Client* exclude = NULL);
	
	// Utility
	std::string getMemberList() const;
	std::string getChannelInfo() const;



	private:
		
		std::string _name;
		std::string _topic;
		std::vector<Client*> _members;

		Client* _creator;
		time_t _creationTime;
		
		bool _inviteOnly;           // +i invite only
		bool _topicRestricted;      // +t só ops podem mudar tópico
		bool _noExternalMessages;   // +n só membros podem enviar mensagens
		bool _moderated;            // +m só ops e voiced podem falar

		size_t _userLimit;          // +l limite de utilizadores(0 = sem limite)

};