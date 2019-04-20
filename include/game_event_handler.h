/*
  game_event_handler.h

  #### ATTRIBUTION ####
  Xiuge Chen
  xiugec@student.unimelb.edu.au
  Subject: COMP30023 Computer Systems
  University of Melbourne

  Some code bring ideas from lab5/6 of comp30023
*/

#ifndef GAME_EVENT_HANDLER_H
#define GAME_EVENT_HANDLER_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <unistd.h>

#include "player_manage.h"

#define BUFFER_SIZE 2049

// represents the types of method
typedef enum
{
    GET,
    POST,
    UNKNOWN
} METHOD;

/*
  process an event read from a specific socket
*/
bool process_event(int sockfd, struct Player* players);

/*
  extract game event based on read content from client
*/
bool extract_event(int sockfd, METHOD* method_p, char* url, char* content);

/*
  intro page display
*/
bool intro_event(int sockfd, struct Player* players);

/*
  register player by the input username
*/
bool register_event(int sockfd, char* content, struct Player* player);

/*
  start page display
*/
bool start_event(int sockfd, struct Player* player);

/*
  cant play(join game) page display
*/
bool cant_play_event(int sockfd);

/*
  send html to client based on address and any extra_content(will replace R in <p>R<\p>)
*/
bool send_html(int sockfd, char* html_addr, char* extra_content);

/*
  get http request method from http header
*/
bool get_method_http(METHOD* method_p, char** curr_p);

/*
  get request url from http header
*/
void get_url_http(char* curr, char* url);

/*
  get recieve content from http header
*/
void get_content_http(char* curr, char* content);

#endif
