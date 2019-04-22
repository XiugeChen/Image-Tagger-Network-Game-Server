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

bool process_event(int sockfd, struct Player* players) {
  char url[BUFFER_SIZE];
  char body[BUFFER_SIZE];
  char cookie[BUFFER_SIZE];
  METHOD method = UNKNOWN;

  // get all information from recieved header
  if(!extract_info(sockfd, &method, url, body, cookie))
    return false;

  printf("cookie: %s\n", cookie);

  // different strategy based on differnet method
  if (method == GET) {
    printf("GET: %s\n", url);

    if (*url == '\0') {
      // a new player coming event (intro_event)
      if (*cookie == '\0') {
        if (!intro_event(sockfd, players))
          return false;
      }
      // a new player using cookie to play (register_event)
      else if (strstr(cookie, "user=") != NULL) {
        if (!register_event(sockfd, cookie, players))
          return false;
      }
      return true;
    }

    // ignore the request about favicon.ico
    if (strstr(url, "favicon.ico") != NULL) {
      if (!send_404_http(sockfd))
        return false;
    }

    //struct Player* player = get_player(sockfd, players);
  }
  else if (method == POST) {
    printf("POST: %s\n", body);

    // new player register username event(register_event)
    if (strstr(body, "user=") != NULL) {
      if (!register_event(sockfd, body, players))
        return false;
    }
  }
  else {
    // never used, just for completeness
    fprintf(stderr, "no other methods supported");
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

bool intro_event(int sockfd, struct Player* players) {
  // check whether have avaliable position for new players (has disconnect player)
  struct Player* player = get_disconnect_player(players);

  if (player != NULL) {
    player->sockfd = sockfd;
    player->status = UNREGISTER;
  }
  else {
    if (!cant_play_event(sockfd))
      return false;
    return true;
  }

  if (!send_html_http(sockfd, "../resources/updated_html/1_intro.html", NULL, NULL, NULL, NULL))
    return false;

  return true;
}

bool register_event(int sockfd, char* info, struct Player* players) {
  struct Player* player = get_player(sockfd, players);
  char* username = strstr(info, "user=") + 5;

  // if nothing in the username, invalid and return to intro page
  if (*username == '\0' || *username == '\r' || *username == '\n') {
    if (!intro_event(sockfd, players))
      return false;
    return true;
  }

  if (player != NULL) {
    strcpy(player->username, username);

    if (!start_event(sockfd, player))
      return false;
  }
  else {
    // if no player associated with this client(client might close connnection accidentally)
    // try to continue client request if there is avaliable player(disconnect player)
    struct Player* new_player = get_disconnect_player(players);

    if (new_player != NULL) {
      strcpy(new_player->username, username);

      if (!start_event(sockfd, new_player))
        return false;
    }
    else if (!cant_play_event(sockfd))
      return false;
  }

  return true;
}

bool start_event(int sockfd, struct Player* player) {
  char username_body[256] = "<p>";
  char username_header[256] = "user=";
  char username[256];

  strcpy(username, player->username);

  strcat(username_body, username);
  strcat(username_body, "</p>");
  strcat(username_header, username);

  if (!send_html_http(sockfd, "../resources/updated_html/2_start.html", "user=", username_header, "<p></p>", username_body))
    return false;

  return true;
}

bool cant_play_event(int sockfd) {
  if (!send_html_http(sockfd, "../resources/updated_html/8_cant_join.html", NULL, NULL, NULL, NULL))
    return false;

  return true;
}
