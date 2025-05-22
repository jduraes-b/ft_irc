#ifndef PRIVMSG_HPP
#define PRIVMSG_HPP

#include <string>

// Function to handle the PRIVMSG command
void handlePrivmsg(const std::string& sender, const std::string& recipient, const std::string& message);

#endif // PRIVMSG_HPP