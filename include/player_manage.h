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
#define MAX_KEYWORD_ROUND 64
#define MAX_INPUT_LEN 64

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
  char username[MAX_INPUT_LEN];
  char keyword[MAX_KEYWORD_ROUND][MAX_INPUT_LEN];
  PLAYER_STATUS status;
};

/*
  initialize a player
*/
void init_player(struct Player* players);

/*
  initialize keyword list of a player
*/
void init_keyword_list(struct Player* player);

/*
  set the information of player
*/
void set_player_info(struct Player* player, int sockfd, char* username, PLAYER_STATUS status);

/*
  add an keyword to player's keyword list
*/
bool add_keyword(struct Player* player, char* keyword);

/*
  store the print out version of all keywords in player's keyword list into buff
*/
void sprintf_keyword(struct Player* player, char* buff);

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
