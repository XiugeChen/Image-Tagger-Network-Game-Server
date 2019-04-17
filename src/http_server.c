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

void run_http_server(char* server, int port) {
  // socket
  int sockfd;
  // address we will listen on
  struct sockaddr_in serv_addr;

  sockfd = create_tcp_socket(server, port);

  initialise_addr(&serv_addr, server, port);

  bind_addr_sock(sockfd, &serv_addr, sizeof(serv_addr));

  execute_server(sockfd);

  close_socket(sockfd);
}

void execute_server(int sockfd) {
  listen(sockfd, 1);

  while(1) {
    break;
  }
}

void initialise_addr(struct sockaddr_in* addr_p, char* server, int port) {
  memset(addr_p, 0, sizeof(*addr_p));

  addr_p->sin_family = AF_INET;
  addr_p->sin_port = htons(port);

  // transform IP address into binary representation and check whether success
  if (inet_pton(AF_INET, server, &(addr_p->sin_addr)) <= 0) {
    perror("transform IP address into binary representations failure");
    exit(EXIT_FAILURE);
  }
}

void bind_addr_sock(int sockfd, struct sockaddr_in* addr_p, int size_addr) {
  if (bind(sockfd, (struct sockaddr *)addr_p, size_addr) < 0) {
      perror("bind socket to server address failure");
      exit(EXIT_FAILURE);
  }
}
