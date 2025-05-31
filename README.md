# IRC Server Project

This project implements a basic IRC server in C++98. The server is designed to manage client connections and handle various IRC commands. Below is an overview of the project's structure and components.

## Project Structure

```
irc-server
├── src
│   ├── main.cpp          # Entry point of the IRC server application
│   ├── server.cpp        # Implementation of the Server class
│   ├── client.cpp        # Implementation of the Client class
│   ├── channel.cpp       # Implementation of the Channel class
│   ├── commands          # Directory for command implementations
│   │   ├── invite.cpp    # INVITE command functionality
│   │   ├── join.cpp      # JOIN command functionality
│   │   ├── kick.cpp      # KICK command fuctionality
│   │   ├── mode.cpp      # MODE command fuctionality
│   │   ├── nick.cpp      # NICK command fuctionality
│   │   ├── part.cpp      # PART command fuctionality
│   │   ├── pass.cpp      # PASS command fuctionality
│   │   ├── privmsg.cpp   # PRIVMSG command functionality
│   │   ├── quit.cpp      # QUIT command functionality
│   │   ├── topic.cpp     # TOPIC command fuctionality
│   │   ├── user.cpp      # USER command fuctionality
│   │   └── who.cpp       # WHO command functionality
│   └── utils             # Directory for utility functions
│       └── utils.cpp     # Utility functions implementation
├── include
│   ├── server.hpp        # Header for the Server class
│   ├── client.hpp        # Header for the Client class
│   ├── channel.hpp       # Header for the Channel class
│   └── utils             # Directory for utility headers
│       └── utils.hpp     # Utility functions header
├── Makefile              # Build instructions for the project
└── README.md             # Project documentation
```

## Setup Instructions

1. **Clone the repository**: 
   ```
   git clone <repository-url>
   cd irc-server
   ```

2. **Build the project**: 
   Use the provided Makefile to compile the project.
   ```
   make
   ```

3. **Run the server**: 
   After building, you can run the server executable.
   ```
   ./irc-server
   ```

## Usage

- The server listens for incoming client connections and processes commands such as JOIN, PRIVMSG, and QUIT.
- Clients can connect to the server using an IRC client application.

## Contributing

Feel free to submit issues or pull requests if you would like to contribute to the project. 

## License

This project is licensed under the MIT License. See the LICENSE file for more details.