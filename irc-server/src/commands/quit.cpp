/*// This file implements the functionality for the QUIT command, allowing clients to disconnect from the server.

#include "quit.hpp"
#include "../client.hpp"
#include "../server.hpp"

void handleQuit(Client &client, const std::string &reason) {
    // Notify the server that the client is disconnecting
    client.disconnect(reason);
    
    // Additional logic for handling the QUIT command can be added here
}*/