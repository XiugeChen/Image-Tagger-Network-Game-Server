/*
  player_manage.h

  #### ATTRIBUTION ####
  Xiuge Chen
  xiugec@student.unimelb.edu.au
  Subject: COMP30023 Computer Systems
  University of Melbourne

  Some code bring ideas from lab5/6 of comp30023
*/

#ifndef PLAYER_MANAGE_H
#define PLAYER_MANAGE_H

#include <stdio.h>
#include <string.h>

#define NUM_PLAYER 2

// represents the status of player
typedef enum
{
    DISCONNECT,
    UNREGISTER,
    UNREADY,
    READY
} PLAYER_STATUS;

struct Player {
  int sockfd;
  char username[128];
  PLAYER_STATUS status;
};

/*
  initialize a player
*/
void init_player(struct Player* players);

/*
  get a player from the players array based on a specific socket fd, NULL if nothing found
*/
struct Player* get_player(int sockfd, struct Player* players);

/*
  get any disconnect player from the players array, NULL if nothing found
*/
struct Player* get_disconnect_player(struct Player* players);

#endif
