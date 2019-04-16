/*
  http_server.c

  #### Description ####


  #### ATTRIBUTION ####
  Xiuge Chen
  xiugec@student.unimelb.edu.au
  Subject: COMP30023 Computer Systems
  University of Melbourne

  Some code bring ideas from lab5/6 of comp30023
*/

#include "http_server.h"

void runHttpServer(char* server, int port) {
  int sockfd;

  sockfd = create_socket(server, port);

  close_socket(sockfd);
}
