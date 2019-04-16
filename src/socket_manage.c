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

int create_socket(char* server, int port) {
  int sockfd = 0;

  sockfd = socket(SOCKET_DOMAIN, SOCKET_TYPE, SOCKET_PROTOCOL);

  if (sockfd < 0) {
    perror("socket creation failure");
    exit(EXIT_FAILURE);
  }

  printf("socketfd: %d\n", sockfd);

  return sockfd;
}

void close_socket(int sockfd) {
  close(sockfd);
}
