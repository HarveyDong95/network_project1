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


#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */
//init function
void distinguish_command(char cmd[128]);
int connect_to_host(char *server_ip, int server_port, int clientport);
void client_command(char cmd[20]);
void sortClient();
void addtoList(int sock, char* ip, int port, char* hostname);
//define
#define BACKLOG 5
#define STDIN 0
#define TRUE 1
#define CMD_SIZE 100
#define BUFFER_SIZE 256
#define DNS_SERVER "8.8.8.8"
#define DNS_SERVER_PORT 53
#define TCP 1
#define UDP 2
#define MSG_SIZE 256
//param
int port;
int clientport;
int clientNum;
struct Node{
	//use getnameinfo to get hostname
	char * ip;
	int port;
	char hostname[128];
	int sock;
	int status;
	int messageCount;
};
struct Node clientList[5];
struct addrinfo *myaddress;

//client!!!!
int client(char argv[20])
{

	int server1;
	//server = connect_to_host(argv[1], atoi(argv[2]));

	while(TRUE){
						printf("\n[PA1-Client@CSE489/589]$ ");
						fflush(stdout);

						char *msg = (char*) malloc(sizeof(char)*MSG_SIZE);
				    	memset(msg, '\0', MSG_SIZE);
						if(fgets(msg, MSG_SIZE-1, stdin) == NULL) //Mind the newline character that will be written to msg
							exit(-1);

						printf("I got: %s\n", msg);
						distinguish_command(msg);
						client_command(msg);

						fflush(stdout);

						/* Initialize buffer to receieve response */
				        char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
				        memset(buffer, '\0', BUFFER_SIZE);

						if(recv(server1, buffer, BUFFER_SIZE, 0) >= 0){
							printf("Server responded: %s", buffer);
							fflush(stdout);
						}
					}
					return 0;
}

int connect_to_host(char *server_ip, int server_port, int clientport)
{
    int fdsocket, len;
    struct sockaddr_in remote_server_addr,client;

    fdsocket = socket(AF_INET, SOCK_STREAM, 0);
    if(fdsocket < 0)
       perror("Failed to create socket");
		client.sin_family = AF_INET;
		client.sin_addr.s_addr = inet_addr("127.0.0.1");
		client.sin_port = htons(clientport);

		if(bind(fdsocket,(struct sockaddr *)&client, sizeof(client)) < 0)
				perror("Bind failed!");

    bzero(&remote_server_addr, sizeof(remote_server_addr));
    remote_server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, server_ip, &remote_server_addr.sin_addr);
    remote_server_addr.sin_port = htons(server_port);

    if(connect(fdsocket, (struct sockaddr*)&remote_server_addr, sizeof(remote_server_addr)) < 0)
        perror("Connect failed");

    return fdsocket;
}



//server!!!!!
int server(int argv)
{
	int  server_socket, head_socket, selret, sock_index, fdaccept=0, caddr_len;
	struct sockaddr_in server_addr, client_addr;
	fd_set master_list, watch_list;
	//socket
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(server_socket < 0)
		perror("Cannot create socket\n");

	//fill up sockaddr
	port = argv;
	bzero(&server_addr, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(port);

	//bind
	if(bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr))<0)
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


	while(TRUE){
			memcpy(&watch_list, &master_list, sizeof(master_list));

			//printf("\n[PA1-Server@CSE489/589]$ ");
			//fflush(stdout);

			/* select() system call. This will BLOCK */
			selret = select(head_socket + 1, &watch_list, NULL, NULL, NULL);
			if(selret < 0)
					perror("select failed.");

			/* Check if we have sockets/STDIN to process */
			if(selret > 0){
			/* Loop through socket descriptors to check which ones are ready */
					for(sock_index=0; sock_index<=head_socket; sock_index+=1){

							if(FD_ISSET(sock_index, &watch_list)){

									/* Check if new command on STDIN */
									if (sock_index == STDIN){
										char *cmd = (char*) malloc(sizeof(char)*CMD_SIZE);

										memset(cmd, '\0', CMD_SIZE);
					if(fgets(cmd, CMD_SIZE-1, stdin) == NULL) //Mind the newline character that will be written to cmd
						exit(-1);

					printf("\nI got: %s\n", cmd);

					//Process PA1 commands here ...
					//Use a function to distinguish command and run them
					distinguish_command(cmd);
					//printf("ip is %s\n",get_host_ip_addr());
					free(cmd);
									}
									/* Check if new client is requesting connection */
									else if(sock_index == server_socket){
											caddr_len = sizeof(client_addr);
											fdaccept = accept(server_socket, (struct sockaddr *)&client_addr, &caddr_len);
											if(fdaccept < 0)
													perror("Accept failed.");

					printf("\nRemote Host connected!\n");
					//addtoList();
						
											/* Add to watched socket list */
											FD_SET(fdaccept, &master_list);
											if(fdaccept > head_socket) head_socket = fdaccept;
									}
									/* Read from existing clients */
									else{
											/* Initialize buffer to receieve response */
											char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
											memset(buffer, '\0', BUFFER_SIZE);

											if(recv(sock_index, buffer, BUFFER_SIZE, 0) <= 0){
													close(sock_index);
													printf("Remote Host terminated connection!\n");

													/* Remove from watched list */
													FD_CLR(sock_index, &master_list);
											}
											else {
												//Process incoming data from existing clients here ...

												printf("\nClient sent me: %s\n", buffer);
						printf("ECHOing it back to the remote host ... ");
						if(send(sock_index, buffer, strlen(buffer), 0) == strlen(buffer))
							printf("Done!\n");
						fflush(stdout);
											}

											free(buffer);
									}
							}
					}
			}
	}

	printf("this is server\n");
	return 0;
}




int main(int argc, char **argv)
{
	/*Init. Logger*/
	cse4589_init_log(argv[2]);

	/*Clear LOGFILE*/
	fclose(fopen(LOGFILE, "w"));
	if(argc != 3) {
		printf("Usage:%s [s/c] [port]\n",argv[0]);
		exit(1);
	}
	//Judgh the second para is server or client
	if((strcmp(argv[1],"s")==0)||(strcmp(argv[1],"c")==0)){
		port = atoi(argv[2]);
		clientport = atoi(argv[2]);
		//jump into server or client
		if((strcmp(argv[1],"s")==0)){
			//jump into server
			server(port);
		}
		else{
			//jump into client
			client(port);
		}
	}
	else{
		printf("Usage2:%s [s/c] [port]\n",argv[0]);
		exit(1);
	}

	/*Start Here*/

	return 0;
}


//get ip address
char * get_host_ip_addr(){
	int sockfd;
	struct sockaddr_in myAddress;
	struct sockaddr_in serverAddress;
	//Create UDP Socket
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0){
		perror("Failed to create socket");
	}

	bzero(&serverAddress, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	inet_pton(AF_INET, DNS_SERVER, &serverAddress.sin_addr.s_addr);
	//serverAddress.sin_addr.s_addr = htonl(DNS_SERVER);
	serverAddress.sin_port = htons(DNS_SERVER_PORT);

	if(connect(sockfd, (struct sockaddr*) &serverAddress, sizeof(serverAddress))<0){
		perror("Failed to connect to DNS_SERVER");
	}

	socklen_t l = INET_ADDRSTRLEN;

	getsockname(sockfd, (struct sockaddr*)&myAddress, &l);
	char *ip;
	ip = inet_ntoa(myAddress.sin_addr);
	//printf("IP....%s /n",ip);
	close(sockfd);
	return ip;
}

//this is a function to distinguish cmd and run them
void distinguish_command(char cmd[20])
{
	char ip[128];
	char your_ubit_name[20];
	int l;
	strcpy(your_ubit_name,"haoruido");
	l = strlen(cmd);
	if(cmd[l-1] == '\n'){
			cmd[l-1] = '\0';
	}
	if(!strncmp(cmd,"AUTHOR",6)){
		cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
		cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n", your_ubit_name);
		cse4589_print_and_log("[%s:END]\n", cmd);
	}
	else if(!strncmp(cmd,"IP",2)) {
		cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
		cse4589_print_and_log("IP:%s\n", get_host_ip_addr());
		cse4589_print_and_log("[%s:END]\n", cmd);
	}
	else if(!strncmp(cmd,"PORT",4)){
		cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
		cse4589_print_and_log("PORT:%d\n", port);
		cse4589_print_and_log("[%s:END]\n", cmd);
	}

}

void sortClient(){
	struct Node clientnode;
	int i = 1;
	for(i; i < clientNum; i++){
		int j = 0;
		while(j < i){
			if(clientList[i].port < clientList[j].port){
				clientnode = clientList[i];
				clientList[i] = clientList[j];
				clientList[j] = clientnode;
			}
			j++;
		}
	}
}

void addtoList(int sock, char* ip, int port, char* hostname){
	clientList[clientNum].sock = sock;
	strcpy(clientList[clientNum].ip, ip);
	clientList[clientNum].port = port;
	clientList[clientNum].status = 1;
	if(hostname == NULL){
		strcpy(clientList[clientNum].hostname, "host");
	}else{
		strcpy(clientList[clientNum].hostname, hostname);
	}
	clientList[clientNum].messageCount = 0;
	clientNum++;
	sortClient();
}

void client_command(char cmd[20]){
	int fdsocket;

	if(strstr(cmd,"LOGIN")){
		int count = 0;
		char *d_array[10];
		char *arg = strtok(cmd," ");
		while(arg){
			d_array[count] = arg;
			count++;
			arg = strtok(NULL," ");
		}

		char * serverIPaddr;
		serverIPaddr = d_array[1];
		int serverport = atoi(d_array[2]);

		fdsocket = connect_to_host(serverIPaddr, serverport, clientport);
		printf("connect success\n");
	}
	//printf("this is client_command\n");
}

/*void printList(){
	while(node!=NULL){
		int count = 1;
		for(count; count<5; count++){

			cse4589cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", count, node->hostname, node->ip, node->port);
		}

	}*/
