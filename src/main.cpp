/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rcosta-c <rcosta-c@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 20:11:08 by jduraes-          #+#    #+#             */
/*   Updated: 2025/05/28 12:32:00 by rcosta-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"
#include "utils/utils.hpp"

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