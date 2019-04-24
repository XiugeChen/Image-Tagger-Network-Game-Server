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
  set_player_info(player, 1, "user=\0", DISCONNECT);
  init_keyword_list(player);
}

void init_keyword_list(struct Player* player) {
  for (int i = 0; i < MAX_KEYWORD_ROUND; i++)
    *(player->keyword[i]) = '\0';
}

void set_player_info(struct Player* player, int sockfd, char* username, PLAYER_STATUS status) {
  if (sockfd != 0)
    player->sockfd = sockfd;

  if (username != NULL)
    strcpy(player->username, username);

  if (status != UNDEFINE)
    player->status = status;
}

bool add_keyword(struct Player* player, char* keyword) {
  for (int i = 0; i < MAX_KEYWORD_ROUND; i++) {
    if (*(player->keyword[i]) == '\0') {
      strcpy(player->keyword[i], keyword);
      int len = strlen(keyword);
      player->keyword[i][len] = '\0';
      return true;
    }
  }

  return false;
}

void sprintf_keyword(struct Player* player, char* buff) {
  int len = 0;

  for (int i = 0; i < MAX_KEYWORD_ROUND; i++) {
    if (*(player->keyword[i]) == '\0')
      break;

    strcat(buff, player->keyword[i]);
    strcat(buff, ", ");

    len += strlen(player->keyword[i]) + 2;
  }

  buff[len] = '\0';
}

struct Player* get_player(int sockfd, struct Player* players) {
  for (int i = 0; i < NUM_PLAYER; i++) {
    if (players[i].sockfd == sockfd) {
      return &players[i];
    }
  }

  return get_disconnect_player(players);
}

void players_quit(struct Player* players) {
  for (int i = 0; i < NUM_PLAYER; i++) {
    if(players[i].status != QUIT && players[i].status != DISCONNECT)
      players[i].status = QUIT;
  }
}

bool all_players_play(struct Player* players) {
  for (int i = 0; i < NUM_PLAYER; i++) {
    if(players[i].status != PLAY)
      return false;
  }

  return true;
}

struct Player* get_disconnect_player(struct Player* players) {
  for (int i = 0; i < NUM_PLAYER; i++) {
    if (players[i].status == DISCONNECT) {
      return &players[i];
    }
  }

  return NULL;
}
