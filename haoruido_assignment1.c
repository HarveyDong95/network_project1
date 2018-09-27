/**
 * @haoruido_assignment1
 * @author  Haorui Dong <haoruido@buffalo.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This contains the main function. Add further description here....
 */
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "../include/global.h"
#include "../include/logger.h"

/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */


//client 
int client()
{
	printf("this is client\n");	
	return 0;
}


//server
int server()
{
	int port, server_socket, head_socket;
	struct sockaddr_in server_addr, client_addr;
	fd_set master_list, watch_list;
	//socket
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(server_socket < 0)
		perror("Cannot create socket\n");
	
	//fill up sockaddr 
	port = atoi(argv[2]);
	bzero(&server_addr, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(port);
	
	//bind
	if(bind(server_socket. (struct sockaddr *)&server_addr, sizeof(server_addr))<0)
		perror("Bind failed");
	
	//listen
	if(listen(server_socket, BACKLOG) < 0)
		perror("Unable to listen on port");
	////////////////////////////////////////////////////////////////////////
	//zero select FD sets
	FD_ZERO(&master_list);
	FD_ZERO(&watch_list);

	//Add a socket
	FD_SET(server_socket, &master_list);
	//Add STDIN
	FD_SET(STDIN, &master_list);

	head_socket = server_socket;

	printf("this is server\n");
	return 0;
}






int main(int argc, char **argv)
{
	if(argc != 3) {
		printf("Usage:%s [server/client] [port]\n",argv[0]);
		exit(1);
	}
	//Judgh the second para is server or client
	if((strcmp(argv[1],"server")==0)||(strcmp(argv[1],"client")==0)){
		//jump into server or client
		if((strcmp(argv[1],"server")==0)){
			//jump into server
			server();
		}
		else{
			//jump into client
			client();
		}	
	}
	else{
		printf("Usage2:%s [server/client] [port]\n",argv[0]);
		exit(1);
	}
	/*Init. Logger*/
	//cse4589_init_log(argv[2]);

	/*Clear LOGFILE*/
	//fclose(fopen(LOGFILE, "w"));

	/*Start Here*/

	return 0;
}
