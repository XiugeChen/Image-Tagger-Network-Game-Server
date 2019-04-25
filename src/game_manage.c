/*
  game_manage.c

  #### Description ####


  #### ATTRIBUTION ####
  Xiuge Chen
  xiugec@student.unimelb.edu.au
  Subject: COMP30023 Computer Systems
  University of Melbourne

  Some code bring ideas from lab5/6 of comp30023
*/

#include "game_manage.h"

bool process_event(struct Player* players, int sockfd, int* round_num) {
  char url[BUFFER_SIZE];
  char body[BUFFER_SIZE];
  char cookie[BUFFER_SIZE];
  METHOD method = UNKNOWN;
  struct Player* player = get_player(sockfd, players);

  // get all information from recieved header
  if(!extract_info(sockfd, &method, url, body, cookie))
    return false;

  // ignore the request about favicon.ico
  if (method == GET && strcmp(url, "favicon.ico") == 0) {
    if (!send_404_http(sockfd)) {
      perror("404");
      return false;
    }
    return true;
  }

  if (player != NULL) {
    // set player information based on whether there is cookie
    if (*cookie == '\0')
      set_player_info(player, sockfd, NULL, UNDEFINE);
    else
      set_player_info(player, sockfd, cookie, UNDEFINE);

    // based on the current status of player, decide which transition will be made,
    // if anything wrong goes with one player (or one click quit), all quit
    switch(player->status) {
      // check whether player should quit game (gameover event), if so, send gameover page and close connection
      case QUIT:
        all_quit(players, player);
        return false;

      case DISCONNECT:
        if (!process_player_discon(player, method, url, cookie)) {
          all_quit(players, player);
          return false;
        }
        break;

      case INTRO:
        if (!process_player_intro(player, method, body)) {
          all_quit(players, player);
          return false;
        }
        break;

      case START:
        if (!process_player_start(player, method, body, url, *round_num)) {
          all_quit(players, player);
          return false;
        }
        break;

      case PLAY:
        if (!process_player_play(player, method, body, round_num, players)) {
          all_quit(players, player);
          return false;
        }
        break;

      case COMPLETE:
        if (!process_player_complete(player, method, body, url, *round_num)) {
          all_quit(players, player);
          return false;
        }
        break;

      default:
        perror("player status");
        all_quit(players, player);
        return false;
    }
  }
  // not allow more than NUM_PLAYER playing the game at the same time
  else if (!cant_play_event(sockfd, cookie))
    return false;

  return true;
}

bool process_player_discon(struct Player* player, METHOD method, char* url, char* cookie) {
  // when player disconnect, only will respond to empty GET request
  if (method == GET) {
    if (*url == '\0') {
      // if there is no cookie or no username inside cookie, it is a new player coming event (intro_event)
      if (*cookie == '\0' || strcmp(cookie, "user=") == 0) {
        if (!intro_event(player))
          return false;
      }
      // if there is username cookie, it is a new player using cookie to play (start_event)
      else if (strstr(cookie, "user=") != NULL) {
        if (!start_event(player))
          return false;
      }
      else
        return false;
    }
    else
      return false;
  }
  else if (method == POST)
    return false;
  else {
    // never used, just for completeness
    fprintf(stderr, "no other methods supported");
    return false;
  }

  return true;
}

bool process_player_intro(struct Player* player, METHOD method, char* body) {
  // when player at the intro page, server only respond to POST username
  if (method == POST) {
    // new player register username event (start_event)
    if (strstr(body, "user=") != NULL) {
      // not allow empty username
      if (strcmp(body, "user=") == 0)
        intro_event(player);
      else {
        strcpy(player->username, body);
        if (!start_event(player))
          return false;
      }
    }
    else
      return false;
  }
  else if (method == GET)
    return false;
  else {
    // never used, just for completeness
    fprintf(stderr, "no other methods supported");
    return false;
  }

  return true;
}

bool process_player_start(struct Player* player, METHOD method, char* body, char* url, int round_num) {
  // when player is at start page, server will respond either start GET request or quit POST request
  // quit
  if (method == POST) {
    if (strcmp(body, "quit=Quit") == 0) {
      quit_event(player);
      return false;
    }
    else
      return false;
  }
  // start playing
  else if (method == GET) {
    if (strcmp(url, "?start=Start") == 0) {
      if (!play_event(player, round_num))
        return false;
    }
    else
      return false;
  }
  else {
    // never used, just for completeness
    fprintf(stderr, "no other methods supported");
    return false;
  }

  return true;
}

bool process_player_play(struct Player* player, METHOD method, char* body, int* round_num, struct Player* players) {
  // when player at the play page, server only respond to POST keyword and quit
  if (method == POST) {
    // quit game
    if (strcmp(body, "quit=Quit") == 0) {
      quit_event(player);
      return false;
    }
    else if (strstr(body, "keyword=") != NULL) {
      if (all_players_play(players)) {
        if (!accept_event(player, body, round_num, players))
          return false;
      }
      // else discard
      else{
        if (!discard_event(player, *round_num))
          return false;
      }
    }
    else
      return false;
  }
  else if (method == GET)
    return false;
  else {
    // never used, just for completeness
    fprintf(stderr, "no other methods supported");
    return false;
  }

  return true;
}

bool process_player_complete(struct Player* player, METHOD method, char* body, char* url, int round_num) {
  // when player is at complete page, server will respond to start GET request, quit POST request
  // and keyword POST request (send end game page)
  // quit
  if (method == POST) {
    if (strcmp(body, "quit=Quit") == 0) {
      quit_event(player);
      return false;
    }
    else if (strstr(body, "keyword=") != NULL) {
      if (!endgame_event(player))
        return false;

      return true;
    }
    else
      return false;
  }
  // start playing
  else if (method == GET) {
    if (strcmp(url, "?start=Start") == 0) {
      if (!play_event(player, round_num))
        return false;
    }
    else
      return false;
  }
  else {
    // never used, just for completeness
    fprintf(stderr, "no other methods supported");
    return false;
  }

  return true;
}

bool extract_info(int sockfd, METHOD* method_p, char* url, char* content, char* cookie) {
  char buff[BUFFER_SIZE];

  int n = read(sockfd, buff, BUFFER_SIZE);
  buff[n] = '\0';

  // close connection event
  if (n <= 0) {
      if (n < 0)
        perror("read");
      else {
        printf("socket %d ask to close connection\n", sockfd);
      }
      return false;
  }

  // terminate the string
  buff[n] = '\0';
  char* curr = buff;

  // parse the method
  if (!get_method_http(&curr, method_p)) {
    if (!send_400_http(sockfd))
      return false;
  }

  // parse the url, cookie and body content
  get_url_http(curr, url);
  get_cookie_http(curr, cookie);
  get_body_http(curr, content);

  return true;
}

bool intro_event(struct Player* player) {
  // create header cookie replacement
  char username_header[MAX_INPUT_LEN];
  strcpy(username_header, player->username);

  if (!send_html_http(player->sockfd, "../resources/updated_html/1_intro.html", "user=", username_header, NULL, NULL, 0))
    return false;

  player->status = INTRO;

  return true;
}

bool start_event(struct Player* player) {
  // create username replacement in header
  char username_header[MAX_INPUT_LEN];
  strcpy(username_header, player->username);

  // get the username from "user=username" string
  char username[MAX_INPUT_LEN] = "\0";
  if (strstr(player->username, "user=") != NULL) {
    char* username_start = strstr(player->username, "user=") + 5;
    strcpy(username, username_start);
  }

  // create username replacement in body
  char username_holder[MAX_INPUT_LEN] = "<form";
  char username_replace[MAX_INPUT_LEN] = "\n\n<p>";
  strcat(username_replace, username);
  strcat(username_replace, "</p>\n\n<form");

  // assemble all body replacement into 2D array
  char* body_origin[1];
  char* body_replacement[1];
  body_origin[0] = username_holder;
  body_replacement[0] = username_replace;

  if (!send_html_http(player->sockfd, "../resources/updated_html/2_start.html", "user=", username_header, body_origin, body_replacement, 1))
    return false;

  player->status = START;

  return true;
}

bool play_event(struct Player* player, int round_num) {
  // create header cookie replacement
  char username_header[MAX_INPUT_LEN];
  strcpy(username_header, player->username);

  // get the number of image
  char image_num[4];
  sprintf(image_num, "%d", round_num % NUM_IMAGE + 1);
  // create image number body replacement
  char image_replace[16] = "image-";
  char image_origin[16] = "image-2";
  strcat(image_replace, image_num);

  char* body_origin[1];
  char* body_replacement[1];
  body_origin[0] = image_origin;
  body_replacement[0] = image_replace;

  if (!send_html_http(player->sockfd, "../resources/updated_html/3_first_turn.html", "user=", username_header, body_origin, body_replacement, 1))
    return false;

  player->status = PLAY;

  return true;
}

bool accept_event(struct Player* player, char* body, int* round_num, struct Player* players) {
  char keyword[MAX_INPUT_LEN];
  // extract keyword from body
  char* start = strstr(body, "keyword=") + 8;
  char* end = strstr(body, "&guess=Guess");
  if (end == NULL)
    return false;

  int len = end - start;
  keyword[len] = '\0';
  for (int i = 0; i < len; i++)
    keyword[i] = *(start + i);

  add_keyword(player, keyword);

  // check if game ends, if so, send endgame page
  if (game_end(players)) {
    if (!endgame_event(player))
      return false;

    players_complete(players);
    (*round_num)++;
    return true;
  }

  // if game not end, continue to accpet keyword and display all accepted keywords
  // create header cookie replacement
  char username_header[MAX_INPUT_LEN];
  strcpy(username_header, player->username);

  // get the number of image
  char image_num[4];
  sprintf(image_num, "%d", *round_num % NUM_IMAGE + 1);
  // create image number body replacement
  char image_replace[16] = "image-";
  char image_origin[16] = "image-2";
  strcat(image_replace, image_num);

  // get all keyword has inputed by player this round and display on page
  // create image number body replacement
  char keyword_replace[BUFFER_SIZE] = "\0";
  char keyword_origin[16] = "<form";
  sprintf_keyword(player, keyword_replace);
  strcat(keyword_replace, "<form");

  char* body_origin[2];
  char* body_replacement[2];
  body_origin[0] = image_origin;
  body_replacement[0] = image_replace;
  body_origin[1] = keyword_origin;
  body_replacement[1] = keyword_replace;

  if (!send_html_http(player->sockfd, "../resources/updated_html/4_accepted.html", "user=", username_header, body_origin, body_replacement, 2))
    return false;

  player->status = PLAY;

  return true;
}

bool discard_event(struct Player* player, int round_num) {
  // create header cookie replacement
  char username_header[MAX_INPUT_LEN];
  strcpy(username_header, player->username);

  // get the number of image
  char image_num[4];
  sprintf(image_num, "%d", round_num % NUM_IMAGE + 1);
  // create image number body replacement
  char image_replace[16] = "image-";
  char image_origin[16] = "image-2";
  strcat(image_replace, image_num);

  char* body_origin[1];
  char* body_replacement[1];
  body_origin[0] = image_origin;
  body_replacement[0] = image_replace;

  if (!send_html_http(player->sockfd, "../resources/updated_html/5_discarded.html", "user=", username_header, body_origin, body_replacement, 1))
    return false;

  player->status = PLAY;

  return true;
}

bool endgame_event(struct Player* player) {
  // create header cookie replacement
  char username_header[MAX_INPUT_LEN];
  strcpy(username_header, player->username);

  if (!send_html_http(player->sockfd, "../resources/updated_html/6_endgame.html", "user=", username_header, NULL, NULL, 0))
    return false;

  init_keyword_list(player);

  player->status = COMPLETE;

  return true;
}

void all_quit(struct Player* players, struct Player* player) {
  quit_event(player);
  players_quit(players);
}

void quit_event(struct Player* player) {
  // create header cookie replacement
  char username_header[MAX_INPUT_LEN];
  strcpy(username_header, player->username);

  send_html_http(player->sockfd, "../resources/updated_html/7_gameover.html", "user=", username_header, NULL, NULL, 0);

  player->status = QUIT;
}

bool cant_play_event(int sockfd, char* cookie) {
  if (!send_html_http(sockfd, "../resources/updated_html/8_cant_join.html", "user=", cookie, NULL, NULL, 0))
    return false;

  return true;
}
