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
#include <stdbool.h>

#define NUM_PLAYER 2

// represents the status of player
typedef enum
{
    UNDEFINE,
    DISCONNECT,
    INTRO,
    START,
    PLAY,
    QUIT
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
  set the information of player
*/
void set_player_info(struct Player* player, int sockfd, char* username, PLAYER_STATUS status);

/*
  get a player from the players array based on a specific socket fd, NULL if nothing found
  if no player associated with this client(client might close connnection accidentally)
  try to continue client request if there is avaliable player(disconnect player)
*/
struct Player* get_player(int sockfd, struct Player* players);

/*
  once one player quit the game, also make all other players quit
*/
void players_quit(struct Player* players);

/*
  check whether all players are ready to play
*/
bool all_players_play(struct Player* players);

// HELPER FUNCTION
/*
  get any disconnect player from the players array, NULL if nothing found
*/
struct Player* get_disconnect_player(struct Player* players);

#endif
