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

  init_addr(&serv_addr, server, port);

  bind_addr_sock(sockfd, &serv_addr, sizeof(serv_addr));

  execute_server(sockfd);

  close_socket(sockfd);
}

void execute_server(int sockfd) {
  if (listen(sockfd, BACKLOG) == -1) {
        perror("listen to socket failure");
        exit(1);
  }

  // initialise an active file descriptors set
  fd_set masterfds;
  init_file_des(sockfd, &masterfds);
  int maxfd = sockfd;

  while(1) {
    // monitor file descriptors
    fd_set readfds = masterfds;
    if (select(FD_SETSIZE, &readfds, NULL, NULL, NULL) < 0) {
        perror("select failure");
        exit(EXIT_FAILURE);
    }

    // loop all possible descriptor
    for (int i = 0; i <= maxfd; ++i) {
      // determine if the current file descriptor is active
      if (FD_ISSET(i, &readfds)) {
        // create new socket if there is new incoming connection request
        if (i == sockfd) {
          struct sockaddr_in cliaddr;
          socklen_t clilen = sizeof(cliaddr);
          int newsockfd = accept(sockfd, (struct sockaddr *)&cliaddr, &clilen);

          if (newsockfd < 0)
              perror("accept faliure, keep running");
          else {
            // add the socket to the set
            FD_SET(newsockfd, &masterfds);
            // update the maximum tracker
            if (newsockfd > maxfd)
                maxfd = newsockfd;

            // print out the IP and the socket number
            char ip[INET_ADDRSTRLEN];
            printf(
                "new connection from %s on socket %d\n",
                // convert to human readable string
                inet_ntop(cliaddr.sin_family, &cliaddr.sin_addr, ip, INET_ADDRSTRLEN),
                newsockfd
            );
          }
        } else if (!handle_http_request(i)) {
          close(i);
          FD_CLR(i, &masterfds);
        }
      }
    }
  }
}

static bool handle_http_request(int sockfd) {
  // try to read the request
  char buff[2049];

  int n = read(sockfd, buff, 2049);
  if (n <= 0) {
      if (n < 0)
          perror("read");
      else
          printf("socket %d close the connection\n", sockfd);
      return false;
  }

  return true;
}

void init_addr(struct sockaddr_in* addr_p, char* server, int port) {
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

void init_file_des(int sockfd, fd_set* masterfds) {
    FD_ZERO(masterfds);
    FD_SET(sockfd, masterfds);
}
