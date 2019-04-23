/*
  game_msg_manage.c

  #### Description ####


  #### ATTRIBUTION ####
  Xiuge Chen
  xiugec@student.unimelb.edu.au
  Subject: COMP30023 Computer Systems
  University of Melbourne

  Some code bring ideas from lab5/6 of comp30023
*/

#include "game_msg_manage.h"

// constants
static char const * const HTTP_200_FORMAT = "HTTP/1.1 200 OK\r\n\
Content-Type: text/html\r\n\
Content-Length: %ld\r\n\
Set-Cookie: user=; Expires=Wed, 21 Oct 2020 07:28:00 GMT;\r\n\r\n";
static char const * const HTTP_400 = "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\n\r\n";
static int const HTTP_400_LENGTH = 47;
static char const * const HTTP_404 = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
static int const HTTP_404_LENGTH = 45;

bool get_method_http(char** request_p, METHOD* method_p) {
  if (strncmp(*request_p, "GET ", 4) == 0) {
      *request_p += 4;
      *method_p = GET;
      return true;
  }
  else if (strncmp(*request_p, "POST ", 5) == 0) {
      *request_p += 5;
      *method_p = POST;
      return true;
  }

  return false;
}

void get_url_http(char* request, char* url) {
  // sanitise the URI
  while (*request == '.' || *request == '/')
    ++request;

  *url = '\0';

  while (*request != ' ') {
    *url = *request;
    ++request;
    ++url;

    *url = '\0';
  }
}

void get_cookie_http(char* request, char* cookie) {
  char* curr = strstr(request, "Cookie: ");
  *cookie = '\0';

  if (curr == NULL)
    return;
  // add the length of "Cookie: "
  else
    curr += 8;

  while (*curr != '\n' && *curr != '\r' && *curr != '\0') {
    *cookie = *curr;
    ++curr;
    ++cookie;

    *cookie = '\0';
  }
}

void get_body_http(char* request, char* body) {
  char* curr = strstr(request, "\r\n\r\n");
  *body = '\0';

  if (curr == NULL)
    return;
  // add the length of "Cookie: "
  else
    curr += 4;

  while (*curr != '\0') {
    *body = *curr;
    ++curr;
    ++body;

    *body = '\0';
  }
}

bool send_404_http(int sockfd) {
  if (write(sockfd, HTTP_404, HTTP_404_LENGTH) < 0) {
    perror("write");
    return false;
  }

  return true;
}

bool send_400_http(int sockfd) {
  if (write(sockfd, HTTP_400, HTTP_400_LENGTH) < 0) {
    perror("write");
    return false;
  }

  return true;
}

bool send_html_http(int sockfd, char* html_addr, char* header_replace_str, char* header_replacement, char* body_replace_str, char* body_replacement) {
  int extra_len = 0;

  if (body_replace_str != NULL && body_replacement != NULL)
    extra_len = strlen(body_replacement) - strlen(body_replace_str);

  if (!send_header_http(sockfd, html_addr, header_replace_str, header_replacement, extra_len)){
    perror("send html header http");
    return false;
  }
  if (!send_body_http(sockfd, html_addr, body_replace_str, body_replacement)) {
    perror("send html body http");
    return false;
  }

  return true;
}

bool send_header_http(int sockfd, char* html_addr, char* header_replace_str, char* header_replacement, int body_extra_len) {
  char buff[BUFFER_SIZE];
  struct stat st;
  int n = 0;

  // get the size of the file, and increase file size to accommodate the extra content
  stat(html_addr, &st);
  long size = st.st_size + body_extra_len;

  // if there is no replacement for header
  if (header_replace_str == NULL || header_replacement == NULL)
    n = sprintf(buff, HTTP_200_FORMAT, size);
  // if there is replacement in header
  else {
    char http200_format[BUFFER_SIZE];

    strcpy(http200_format, HTTP_200_FORMAT);

    // get the size of header
    int header_size = strlen(http200_format) + strlen(header_replacement) - strlen(header_replace_str);

    http200_format[header_size] = '\0';

    if (!replace_str(http200_format, header_replace_str, header_replacement))
      return false;

    n = sprintf(buff, http200_format, size);
  }

  // send the header
  if (write(sockfd, buff, n) < 0) {
      perror("write faliure");
      return false;
  }

  return true;
}

bool send_body_http(int sockfd, char* html_addr, char* body_replace_str, char* body_replacement) {
  char buff[BUFFER_SIZE];
  struct stat st;
  int n = 0;

  // if there is no replacement, just send the html file (first header later body)
  if (body_replace_str == NULL || body_replacement == NULL) {
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
  // if there is no replacement, replace the string with replacement first, then send the html file
  else {
    // get the size of the file, and increase file size to accommodate the extra content
    stat(html_addr, &st);
    int extra_length = strlen(body_replacement) - strlen(body_replace_str);
    long size = st.st_size + extra_length;

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

    if (!replace_str(buff, body_replace_str, body_replacement))
      return false;

    if (write(sockfd, buff, size) < 0) {
        perror("write");
        return false;
    }
  }

  return true;
}

bool replace_str(char* source, char* replace_str, char* replacement) {
  int extra_len = strlen(replacement) - strlen(replace_str);
  int size = strlen(source) + extra_len;

  // get the index of replace string
  char* index_c = strstr(source, replace_str);
  if (index_c == NULL) {
    perror("replace string");
    return false;
  }
  int index = index_c - source;

  // move the trailing part backward
  int p1, p2;
  for (p1 = size - 1, p2 = p1 - extra_len; p2 != index; --p1, --p2)
      source[p1] = source[p2];

  strncpy(source + p2, replacement, strlen(replacement));

  return true;
}
