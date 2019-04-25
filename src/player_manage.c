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
      int len = strlen(keyword);
      strcpy(player->keyword[i], keyword);

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

  // eliminate the last two char ", "
  buff[len - 2] = '\0';
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

void players_complete(struct Player* players) {
  for (int i = 0; i < NUM_PLAYER; i++) {
    if(players[i].status != QUIT && players[i].status != DISCONNECT)
      players[i].status = COMPLETE;
  }
}

bool all_players_play(struct Player* players) {
  for (int i = 0; i < NUM_PLAYER; i++) {
    if(players[i].status != PLAY)
      return false;
  }

  return true;
}

bool game_end(struct Player* players) {
  if (all_players_play(players)) {
    for (int i = 0; i < NUM_PLAYER; i++) {
      for (int j = i + 1; j < NUM_PLAYER; j++) {
        for (int i_key = 0; i_key < MAX_KEYWORD_ROUND; i_key++) {
          for (int j_key = 0; j_key < MAX_KEYWORD_ROUND; j_key++) {
            if (*(players[i].keyword[i_key]) == '\0' || *(players[j].keyword[j_key]) == '\0')
              break;

            // convert all keyword to lowercase
            char keyword_i[MAX_INPUT_LEN];
            all_to_lower(keyword_i, players[i].keyword[i_key]);
            char keyword_j[MAX_INPUT_LEN];
            all_to_lower(keyword_j, players[j].keyword[j_key]);

            if (strcmp(keyword_i, keyword_j) == 0)
              return true;
          }
        }
      }
    }
  }

  return false;
}

struct Player* get_disconnect_player(struct Player* players) {
  for (int i = 0; i < NUM_PLAYER; i++) {
    if (players[i].status == DISCONNECT) {
      return &players[i];
    }
  }

  return NULL;
}

void all_to_lower(char* new, char* origin) {
  int len = strlen(origin);

  for (int n = 0; n < len; n++)
    new[n] = tolower(origin[n]);

  new[len] = '\0';
}
