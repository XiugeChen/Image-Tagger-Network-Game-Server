/*
  game_msg_manage.h

  #### ATTRIBUTION ####
  Xiuge Chen
  xiugec@student.unimelb.edu.au
  Subject: COMP30023 Computer Systems
  University of Melbourne

  Some code bring ideas from lab5/6 of comp30023
*/

#ifndef GAME_MSG_MANAGE_H
#define GAME_MSG_MANAGE_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <unistd.h>

#define BUFFER_SIZE 2049

// represents the types of method
typedef enum
{
    GET,
    POST,
    UNKNOWN
} METHOD;

/*
  get http request method from http request header
*/
bool get_method_http(char** request_p, METHOD* method_p);

/*
  get request url from http request header
*/
void get_url_http(char* request, char* url);

/*
  get recieve content from http request header
*/
void get_body_http(char* request, char* content);

/*
  get cookies from http request header
*/
void get_cookie_http(char* request, char* cookie);

/*
  send 404 http to client
*/
bool send_404_http(int sockfd);

/*
  send 400 http to client
*/
bool send_400_http(int sockfd);

/*
  send 200 http to client based on html address and any replacement in header and body
*/
bool send_html_http(int sockfd, char* html_addr, char* header_replace_str, char* header_replacement, char* body_replace_str, char* body_replacement);

// HELPER FUNCTIONS
/*
  send the header of http with or without replacement
*/
bool send_header_http(int sockfd, char* html_addr, char* header_replace_str, char* header_replacement, int body_extra_len);

/*
  send the body of http with or without replacement
*/
bool send_body_http(int sockfd, char* html_addr, char* body_replace_str, char* body_replacement);

/*
  replace substring inside source with replacement
*/
bool replace_str(char* source, char* replace_str, char* replacement);

#endif
