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
bool process_event(struct Player* players, int sockfd);

// HELPER FUNCTIONS
/*

*/
bool process_player_discon(struct Player* player, METHOD method, char* url, char* cookie);

/*

*/
bool process_player_intro(struct Player* player, METHOD method, char* body);

/*

*/
bool process_player_start(struct Player* player, METHOD method, char* body);

/*
  extract game event based on read content from client
*/
bool extract_info(int sockfd, METHOD* method_p, char* url, char* content, char* cookie);

/*
  intro page display
*/
bool intro_event(struct Player* player);

/*
  start page display
*/
bool start_event(struct Player* player);

/*
  player quit, gameover page display
*/
bool quit_event(struct Player* player);

/*
  cant play(join game) page display
*/
bool cant_play_event(int sockfd, char* cookie);

#endif
