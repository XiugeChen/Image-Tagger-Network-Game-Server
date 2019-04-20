/*
  game_event_handler.c

  #### Description ####


  #### ATTRIBUTION ####
  Xiuge Chen
  xiugec@student.unimelb.edu.au
  Subject: COMP30023 Computer Systems
  University of Melbourne

  Some code bring ideas from lab5/6 of comp30023
*/

#include "game_event_handler.h"

// constants
static char const * const HTTP_200_FORMAT = "HTTP/1.1 200 OK\r\n\
Content-Type: text/html\r\n\
Content-Length: %ld\r\n\r\n";
static char const * const HTTP_400 = "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\n\r\n";
static int const HTTP_400_LENGTH = 47;
static char const * const HTTP_404 = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
static int const HTTP_404_LENGTH = 45;


bool process_event(int sockfd, struct Player* players) {
  char url[BUFFER_SIZE];
  char content[BUFFER_SIZE];
  METHOD method = UNKNOWN;

  // get all information from recieved header
  if(!extract_event(sockfd, &method, url, content))
    return false;

  // different strategy based on differnet method
  if (method == GET) {
    printf("GET: %s\n", url);

    // a new player coming event (intro_event)
    if (*url == '\0') {
      if (!intro_event(sockfd, players))
        return false;
      return true;
    }

    // ignore the request about favicon.ico
    if (strstr(url, "favicon.ico") != NULL) {
      if (write(sockfd, HTTP_404, HTTP_404_LENGTH) < 0) {
        perror("write");
        return false;
      }
    }

    //struct Player* player = get_player(sockfd, players);
  }
  else if (method == POST) {
    printf("POST: %s\n", content);

    // new player register username event(register_event)
    if (strstr(content, "user=") != NULL) {
      if (!register_event(sockfd, content, players))
        return false;
    }
  }
  else {
    // never used, just for completeness
    fprintf(stderr, "no other methods supported");
  }

  return true;
}

bool extract_event(int sockfd, METHOD* method_p, char* url, char* content) {
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
  if (!get_method_http(method_p, &curr)) {
    if (write(sockfd, HTTP_400, HTTP_400_LENGTH) < 0) {
      perror("write");
      return false;
    }
  }

  // parse the url and content
  get_url_http(curr, url);
  get_content_http(curr, content);

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
    else
      return true;
  }

  if (!send_html(sockfd, "../resources/updated_html/1_intro.html", NULL))
    return false;

  return true;
}

bool register_event(int sockfd, char* content, struct Player* players) {
  struct Player* player = get_player(sockfd, players);
  char* username = strstr(content, "user=") + 5;

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
    else if (!intro_event(sockfd, players))
      return false;
  }

  return true;
}

bool start_event(int sockfd, struct Player* player) {
  if (!send_html(sockfd, "../resources/updated_html/2_start.html", player->username))
    return false;

  return true;
}

bool cant_play_event(int sockfd) {
  if (!send_html(sockfd, "../resources/updated_html/8_cant_join.html", NULL))
    return false;

  return true;
}

bool send_html(int sockfd, char* html_addr, char* extra_content) {
  char buff[BUFFER_SIZE];
  struct stat st;
  int n = 0;

  if (extra_content == NULL) {
    // get the size of the file
    stat(html_addr, &st);
    n = sprintf(buff, HTTP_200_FORMAT, st.st_size);
    // send the header first
    if (write(sockfd, buff, n) < 0) {
        perror("write faliure");
        return false;
    }
    // send the file
    int filefd = open(html_addr, O_RDONLY);
    do {
      n = sendfile(sockfd, filefd, NULL, BUFFER_SIZE);
    } while (n > 0);

    if (n < 0) {
      perror("sendfile");
      close(filefd);
      return false;
    }
    close(filefd);
  }
  else {
    int extra_length = strlen(extra_content);
    // get the size of the file
    stat(html_addr, &st);
    // increase file size to accommodate the extra content (need -1 to subtract the replacement symbol R)
    long size = st.st_size + extra_length - 1;
    n = sprintf(buff, HTTP_200_FORMAT, size);

    // send the header first
    if (write(sockfd, buff, n) < 0) {
        perror("write");
        return false;
    }
    // read the content of the HTML file
    int filefd = open(html_addr, O_RDONLY);
    n = read(filefd, buff, BUFFER_SIZE);
    if (n < 0) {
        perror("read");
        close(filefd);
        return false;
    }
    close(filefd);

    buff[size] = '\0';

    // move the trailing part backward
    int p1, p2;
    for (p1 = size - 1, p2 = p1 - (extra_length - 1); buff[p2] != 'R'; --p1, --p2)
        buff[p1] = buff[p2];

    strncpy(buff + p2, extra_content, extra_length);

    if (write(sockfd, buff, size) < 0) {
        perror("write");
        return false;
    }
  }

  return true;
}

bool get_method_http(METHOD* method_p, char** curr_p) {
  if (strncmp(*curr_p, "GET ", 4) == 0) {
      *curr_p += 4;
      *method_p = GET;
      return true;
  }
  else if (strncmp(*curr_p, "POST ", 5) == 0) {
      *curr_p += 5;
      *method_p = POST;
      return true;
  }

  return false;
}

void get_url_http(char* curr, char* url) {
  // sanitise the URI
  while (*curr == '.' || *curr == '/')
    ++curr;

  *url = '\0';

  while (*curr != ' ') {
    *url = *curr;
    ++curr;
    ++url;

    *url = '\0';
  }
}

void get_content_http(char* curr, char* content) {
  // sanitise the content
  char* body = strstr(curr, "\r\n\r\n") + 4;

  *content = '\0';

  while (*body != '\n' && *body != '\r') {
    *content = *body;
    ++body;
    ++content;

    *content = '\0';
  }
}
