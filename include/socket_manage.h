/*
  socket_manage.h

  #### ATTRIBUTION ####
  Xiuge Chen
  xiugec@student.unimelb.edu.au
  Subject: COMP30023 Computer Systems
  University of Melbourne

  Some code bring ideas from lab5/6 of comp30023
*/

#ifndef SOCKET_MANAGE_H
#define SOCKET_MANAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

/*
  create a new socket based on configuration
  Parameter: server(string), port(integer)
  Return: file descriptor of new socket(integer)
*/
int create_tcp_socket(char* server, int port);

/*
  close a socket according to its file descriptors
  Parameter: socket file descriptor(integer)
  Return: none
*/
void close_socket(int sockfd);

#endif
