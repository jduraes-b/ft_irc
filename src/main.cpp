/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jduraes- <jduraes-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 20:11:08 by jduraes-          #+#    #+#             */
/*   Updated: 2025/05/31 15:57:24 by jduraes-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"
#include "utils/utils.hpp"
#include <csignal>

volatile std::sig_atomic_t g_running = 1;

void handle_sigint(int) {
    g_running = 0;
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