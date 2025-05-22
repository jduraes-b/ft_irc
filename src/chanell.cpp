#include <channel.hpp>

Channel::Channel(const std::string &name, Client *creator)
{}
Channel::~Channel()
{}

    //Getters básicos
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
    int count;

    return (this->_members.size());

}

time_t Channel::getCreationTime() const
{
    return (this->_creationTime);

}
    
    //Topic
void Channel::setTopic(const std::string& topic, Client* setter)
{
    if(canSetTopic(setter));
        this->_topic = topic;
}


bool Channel::canSetTopic(Client* client) const
{

    //verificar se pode -> +i  

    /*
    if(pode sim)
        retrn true
    else
        nao.
    
    */


}
    
    //Member management

bool Channel::addMember(Client* client, const std::string& key = "")
{

    //if(fazer veficações)
    //se falhar return (false);

    //add member to container _members
    //_members.push_back(client);

    //se não 
    //return (true);

    //
}


void Channel::removeMember(Client* client)
{
 
    //TOTHINK and TODO

}

    
bool Channel::isMember(Client* client) const
{
    return std::find(_members.begin(), _members.end(), client) != _members.end();

}

    
std::vector<Client*> Channel::getMembers() const
{
    return (this->_members);
}
    

//Message

void Channel::broadcastMessage(const std::string& message, Client* sender = NULL)
{
    //TOTHINK and TODO
}


void Channel::sendMessage(const std::string& message, Client* sender, Client* exclude = NULL)
{
    //TOTHINK and TODO
}



    // Utility
std::string Channel::getMemberList() const
{
    std::string list;
    for (std::vector<Client*>::const_iterator it = _members.begin(); it != _members.end(); ++it)
    {
        if (!list.empty())
            list += " ";
            
        // Verificar prefixos: @ para op, + para voice
        //e escrever o prefixo antes do nome
            
        list += (*it)->getNickname();
    }
    return list;
}

std::string Channel::getChannelInfo() const
{
    std::stringstream channelInfo;
    channelInfo << "Channel: " << _name << " (" << _members.size() << " members)";
    if (!_topic.empty())
        channelInfo << " - Topic: " << _topic;
    return channelInfo.str();
}