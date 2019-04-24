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

  printf("round: %d\n", *round_num);
  printf("cookie: %s\n", cookie);
  printf("url: %s\n", url);
  printf("body: %s\n", body);
  if (method == GET)
    printf("GET\n");
  else if (method == POST)
    printf("POST\n");

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
        break;

      case INTRO:
        if (!process_player_intro(player, method, body)) {
          perror("player_intro");
          players_quit(players);
          return false;
        }
        break;

      case START:
        if (!process_player_start(player, method, body, url, *round_num)) {
          perror("player_start");
          players_quit(players);
          return false;
        }
        break;

      case PLAY:
        if (!process_player_play(player, method, body, *round_num, all_players_play(players))) {
          perror("player_start");
          players_quit(players);
          return false;
        }
        break;

      default:
        perror("player status");
        players_quit(players);
        return false;
    }
  }
  // not allow more than NUM_PLAYER playing the game at the same time
  else if (!cant_play_event(sockfd, cookie)) {
    perror("cant_play");
    return false;
  }

  return true;
}

bool process_player_discon(struct Player* player, METHOD method, char* url, char* cookie) {
  // when player disconnect, only will respond to empty GET request
  if (method == GET) {
    if (*url == '\0') {
      // if there is no cookie or no username inside cookie, it is a new player coming event (intro_event)
      if (*cookie == '\0' || strcmp(cookie, "user=") == 0) {
        if (!intro_event(player)) {
          perror("intro");
          return false;
        }
      }
      // if there is username cookie, it is a new player using cookie to play (start_event)
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
    else {
      perror("GET url");
      return false;
    }
  }
  else if (method == POST) {
    perror("method POST");
    return false;
  }
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
    else {
      perror("POST body");
      return false;
    }
  }
  else if (method == GET) {
    perror("method GET");
    return false;
  }
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
      if (!quit_event(player))
        perror("gameover");
      return false;
    }
    else {
      perror("POST body");
      return false;
    }
  }
  // start playing
  else if (method == GET) {
    if (strcmp(url, "?start=Start") == 0) {
      if (!play_event(player, round_num)) {
        perror("play");
        return false;
      }
    }
    else {
      perror("GET url");
      return false;
    }
  }
  else {
    // never used, just for completeness
    fprintf(stderr, "no other methods supported");
    return false;
  }

  return true;
}

bool process_player_play(struct Player* player, METHOD method, char* body, int round_num, bool all_players_play) {
  // when player at the play page, server only respond to POST keyword and quit
  if (method == POST) {
    // quit game
    if (strcmp(body, "quit=Quit") == 0) {
      if (!quit_event(player))
        perror("gameover");
      return false;
    }
    else if (strstr(body, "keyword=") != NULL) {
      if (all_players_play) {

      }
      // else discard
      else{
        if (!discard_event(player, round_num)) {
          perror("discard");
          return false;
        }
      }
    }
    else {
      perror("POST body");
      return false;
    }
  }
  else if (method == GET) {
    perror("method GET");
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

  // get the username from "user=username" string
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

bool play_event(struct Player* player, int round_num) {
  char username_header[256];
  strcpy(username_header, player->username);

  if (!send_html_http(player->sockfd, "../resources/updated_html/3_first_turn.html", "user=", username_header, NULL, NULL))
    return false;

  player->status = PLAY;

  return true;
}

bool discard_event(struct Player* player, int round_num) {
  char username_header[256];
  strcpy(username_header, player->username);

  if (!send_html_http(player->sockfd, "../resources/updated_html/5_discarded.html", "user=", username_header, NULL, NULL))
    return false;

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
