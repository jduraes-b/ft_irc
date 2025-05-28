/*// This file implements the functionality for the PRIVMSG command, enabling clients to send private messages to each other.

#include "privmsg.hpp"
#include "../client.hpp"
#include "../server.hpp"

// Function to handle the PRIVMSG command
void handlePrivmsg(Client &sender, const std::string &recipient, const std::string &message, Server &server) {
    Client *recipientClient = server.getClientByNickname(recipient);
    if (recipientClient) {
        recipientClient->sendMessage(sender.getNickname() + ": " + message);
    } else {
        sender.sendMessage("No such nick/channel: " + recipient);
    }
}*/