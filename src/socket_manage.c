/*
  socket_manage.c

  #### Description ####


  #### ATTRIBUTION ####
  Xiuge Chen
  xiugec@student.unimelb.edu.au
  Subject: COMP30023 Computer Systems
  University of Melbourne

  Some code bring ideas from lab5/6 of comp30023
*/

#include "socket_manage.h"

int create_tcp_socket(char* server, int port) {
  int sockfd = 0;

  // create socket based on configuration
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd <= 0) {
    perror("socket creation failure");
    exit(EXIT_FAILURE);
  }

  // reuse the socket if possible
  int const reuse = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0) {
      perror("set socket option to reuse address failure");
      exit(EXIT_FAILURE);
  }

  return sockfd;
}

void close_socket(int sockfd) {
  close(sockfd);
}
