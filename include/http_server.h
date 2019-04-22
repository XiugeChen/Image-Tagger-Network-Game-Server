/*
  http_server.h

  #### ATTRIBUTION ####
  Xiuge Chen
  xiugec@student.unimelb.edu.au
  Subject: COMP30023 Computer Systems
  University of Melbourne

  Some code bring ideas from lab5/6 of comp30023
*/

#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>

#include "socket_manage.h"
#include "player_manage.h"
#include "game_manage.h"

#define BACKLOG 2

/*
  run the server on specific server and port
*/
void run_http_server(char* server, int port);

// HELPER FUNCTIONS
/*
  execute a server on listening a specific socket
*/
void execute_server(int sockfd);

/*
  create and initialise address we will listen on
  Input: server address information(pointer of struct sockaddr_in), server address(1D char array), port number(int)
*/
void init_addr(struct sockaddr_in* serv_addr_p, char* server, int port);

/*
  bind address to socket
*/
void bind_addr_sock(int sockfd, struct sockaddr_in* addr_p, int size_addr);

/*
  initialise an active file descriptors set
*/
void init_file_des(int sockfd, fd_set* masterfds);

#endif
