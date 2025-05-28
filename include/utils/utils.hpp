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

// Utility function declarations
std::string trim(const std::string &str);
std::string toLower(const std::string &str);
std::string toUpper(const std::string &str);
void logMessage(const std::string &message);

#endif // UTILS_HPP