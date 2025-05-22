/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jduraes- <jduraes-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 20:11:08 by jduraes-          #+#    #+#             */
/*   Updated: 2025/05/13 20:13:22 by jduraes-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

int main() {
    Server server;

    // Initialize the server
    server.start();

    // Main event loop
    while (true) {
        server.acceptClient();
        // Additional event handling can be added here
    }

    return 0;
}