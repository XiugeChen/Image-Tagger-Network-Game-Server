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

bool process_event(struct Player* players, int sockfd) {
  char url[BUFFER_SIZE];
  char body[BUFFER_SIZE];
  char cookie[BUFFER_SIZE];
  METHOD method = UNKNOWN;
  struct Player* player = get_player(sockfd, players);

  // get all information from recieved header
  if(!extract_info(sockfd, &method, url, body, cookie))
    return false;

  // ignore the request about favicon.ico
  if (method == GET && strstr(url, "favicon.ico") != NULL) {
    if (!send_404_http(sockfd)) {
      perror("404");
      return false;
    }
    return true;
  }

  printf("cookie: %s\n", cookie);

  if (player != NULL) {
    if (*cookie == '\0')
      set_player_info(player, sockfd, NULL, UNDEFINE);
    else
      set_player_info(player, sockfd, cookie, UNDEFINE);

    switch(player->status) {
      // check whether player should quit game (gameover event), if so, send gameover page and close connection
      case QUIT:
        if (!quit_event(player))
          perror("gameover");

        players_quit(players);
        return false;

      case DISCONNECT:
        if (!process_player_discon(player, method, url, cookie)) {
          perror("player_discon");
          players_quit(players);
          return false;
        }
        return true;

      case INTRO:
        if (!process_player_intro(player, method, body)) {
          perror("player_intro");
          players_quit(players);
          return false;
        }
        return true;

      case START:
        if (!process_player_start(player, method, body)) {
          perror("player_start");
          players_quit(players);
          return false;
        }
        return true;

      default:
        perror("player status");
        players_quit(players);
        return false;
    }
  }
  else if (!cant_play_event(sockfd, cookie)) {
    perror("cant_play");
    return false;
  }

  return true;
}

bool process_player_discon(struct Player* player, METHOD method, char* url, char* cookie) {
  if (method == GET) {
    if (*url == '\0') {
      // a new player coming event (intro_event)
      if (*cookie == '\0' || strcmp(cookie, "user=") == 0) {
        if (!intro_event(player)) {
          perror("intro");
          return false;
        }
      }
      // a new player using cookie to play (start_event)
      else if (strstr(cookie, "user=") != NULL) {
        if (!start_event(player)) {
          perror("intro");
          return false;
        }
      }
      else {
        perror("cookie");
        return false;
      }
    }
  }
  else if (method == POST) {

  }
  else {
    // never used, just for completeness
    fprintf(stderr, "no other methods supported");
    return false;
  }

  return true;
}

bool process_player_intro(struct Player* player, METHOD method, char* body) {
  if (method == POST) {
    // new player register username event (start_event)
    if (strstr(body, "user=") != NULL) {
      if (strcmp(body, "user=") == 0) {
        perror("username can't be empty");
        intro_event(player);
      }
      else {
        strcpy(player->username, body);
        if (!start_event(player)) {
          perror("intro");
          return false;
        }
      }
    }
  }
  else if (method == GET) {
  }
  else {
    // never used, just for completeness
    fprintf(stderr, "no other methods supported");
    return false;
  }

  return true;
}

bool process_player_start(struct Player* player, METHOD method, char* body) {
  if (method == POST) {
    if (strstr(body, "quit=") != NULL) {
      if (!quit_event(player))
        perror("gameover");

      return false;
    }
  }
  else if (method == GET) {

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
        printf("socket %d close the connection\n", sockfd);
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
  char username_header[256];
  strcpy(username_header, player->username);

  if (!send_html_http(player->sockfd, "../resources/updated_html/1_intro.html", "user=", username_header, NULL, NULL)) {
    perror("send_html");
    return false;
  }

  player->status = INTRO;

  return true;
}

bool start_event(struct Player* player){
  char username_body[256] = "<p>";
  char username_header[256];
  char username[256] = "\0";

  strcpy(username_header, player->username);

  if (strstr(player->username, "user=") != NULL) {
    char* username_start = strstr(player->username, "user=") + 5;
    strcpy(username, username_start);
  }

  strcat(username_body, username);
  strcat(username_body, "</p>");

  if (!send_html_http(player->sockfd, "../resources/updated_html/2_start.html", "user=", username_header, "<p></p>", username_body))
    return false;

  player->status = START;

  return true;
}

bool quit_event(struct Player* player) {
  char username_header[256];
  strcpy(username_header, player->username);

  if (!send_html_http(player->sockfd, "../resources/updated_html/7_gameover.html", "user=", username_header, NULL, NULL))
    return false;

  player->status = QUIT;

  return true;
}

bool cant_play_event(int sockfd, char* cookie) {
  if (!send_html_http(sockfd, "../resources/updated_html/8_cant_join.html", "user=", cookie, NULL, NULL))
    return false;

  return true;
}
