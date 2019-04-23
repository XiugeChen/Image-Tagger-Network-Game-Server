/*
  player_manage.c

  #### ATTRIBUTION ####
  Xiuge Chen
  xiugec@student.unimelb.edu.au
  Subject: COMP30023 Computer Systems
  University of Melbourne

  Some code bring ideas from lab5/6 of comp30023
*/

#include "player_manage.h"

void init_player(struct Player* player) {
  player->sockfd = 0;
  player->status = DISCONNECT;
  strcpy(player->username, "\0");
}

struct Player* get_player(int sockfd, struct Player* players) {
  for (int i = 0; i < NUM_PLAYER; i++) {
    if (players[i].sockfd == sockfd) {
      return &players[i];
    }
  }

  if (get_quit_player(players) != NULL)
    return get_quit_player(players);

  return get_disconnect_player(players);
}

void players_quit(struct Player* players) {
  for (int i = 0; i < NUM_PLAYER; i++)
    players[i].status = QUIT;
}

struct Player* get_disconnect_player(struct Player* players) {
  for (int i = 0; i < NUM_PLAYER; i++) {
    if (players[i].status == DISCONNECT) {
      return &players[i];
    }
  }

  return NULL;
}

struct Player* get_quit_player(struct Player* players) {
  for (int i = 0; i < NUM_PLAYER; i++) {
    if (players[i].status == QUIT) {
      return &players[i];
    }
  }

  return NULL;
}
