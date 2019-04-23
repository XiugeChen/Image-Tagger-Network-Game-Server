/*
  game_manage.h

  #### ATTRIBUTION ####
  Xiuge Chen
  xiugec@student.unimelb.edu.au
  Subject: COMP30023 Computer Systems
  University of Melbourne

  Some code bring ideas from lab5/6 of comp30023
*/

#ifndef GAME_MANAGE_H
#define GAME_MANAGE_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>

#include "player_manage.h"
#include "game_msg_manage.h"

/*
  process an event read from a specific socket
*/
bool process_event(int sockfd, struct Player* players);

// HELPER FUNCTIONS
/*
  extract game event based on read content from client
*/
bool extract_info(int sockfd, METHOD* method_p, char* url, char* content, char* cookie);

/*
  intro page display
*/
bool intro_event(int sockfd, struct Player* players);

/*
  register player by the input username
*/
bool register_event(int sockfd, char* info, struct Player* players);

/*
  start page display
*/
bool start_event(int sockfd, struct Player* players);

/*
  player quit, gameover page display
*/
bool quit_event(int sockfd, struct Player* players);

/*
  cant play(join game) page display
*/
bool cant_play_event(int sockfd, struct Player* players);

#endif
