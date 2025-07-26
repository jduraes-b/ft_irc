/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jduraes- <jduraes-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 20:11:08 by jduraes-          #+#    #+#             */
/*   Updated: 2025/07/23 19:19:53 by jduraes-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"
#include "utils/utils.hpp"
#include <csignal>

volatile std::sig_atomic_t g_running = 1;

void handle_sigint(int) 
{
    g_running = 0;
}

void	parser(int port, std::string pass)
{
	if (port <= 0 || port > 65535)
        	throw std::runtime_error("port must be in [0..65535].");
	/*if ((port < 6660 || port > 6669) && port != 6697 && port != 7000)
		throw std::runtime_error("port must be in [6660..6669, 6697, 7000].");*/
	if (port < 1024)
        	throw std::runtime_error("ports <1024 require root privileges");
    if (pass.empty() || pass.find_first_of(" \t\r\n") != std::string::npos || pass.size() > 64)
        throw std::runtime_error("password must be 1-64 printable chars, no spaces");
}

int main(int ac, char **av)
{    
    if (ac != 3)
    {
        std::cout << "./ircserver <port> <serverpassword>" << std::endl;
        return(1);
    }
    try
	{
		parser(std::atoi(av[1]), std::string(av[2]));
        // Initialize the server with a port and optional password
        //int port = 6667; // Default IRC port   
		Server server(std::atoi(av[1]), av[2]);
		std::signal(SIGINT, handle_sigint);
        // Start the server (includes the main event loop)
        server.start();
    } 
	catch (const std::exception &e)
	{
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
