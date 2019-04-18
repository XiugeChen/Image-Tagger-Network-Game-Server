/*
  image_tagger.c

  #### DESCRIPTION ###
  Project 1
  a network game server that tags images as people play. (The idea was invented
  by the inventor of re-Captcha and Duo-lingo)

  The game consists of two players and one server, where each of the players can
  only communicate with the server (but the other player).
  At the beginning, when the two players log on to the server, the server sends
  both of them a same image. he game then starts;
  the players submit words or phrases to the server, one at a time, without being
  told what the other player has inputed. The goal for both the players is to
  enter a word or a phrase that has been submitted by the other as soon as possible.
  Once the goal is achieved, the game ends; the server sends a web page indicating
  that the game is completed and prompting the players to play again.
  If both the players agree to play again, the process repeats with a new image.

  for more information, please refer to resources/project_description.pdf

  #### RUNNING COMMAND ####
  Run server: ./image_tagger <server_ip> <port_number>
  where server_ip and port_number are configuration for server IP address and
  the port number, server automatically quit if configured incorrectly.

  All html files provided in upldated_html

  #### ATTRIBUTION ####
  Xiuge Chen
  xiugec@student.unimelb.edu.au
  Subject: COMP30023 Computer Systems
  University of Melbourne

  Some code bring ideas from lab5/6 of comp30023
  and Beej's Guide to Network Programming http://www.beej.us/guide/bgnet/html/multi/index.html
*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "http_server.h"

// Declarations
bool checkArgs(int argc, char * argv[]);

/*
  main control
  Parameter: number of command line arguments(integer), command line arguments(1D string array)
  Return: execution status(integer)
*/
int main(int argc, char * argv[]) {
  if (!checkArgs(argc, argv)) {
    return 0;
  }

  int port = atoi(argv[2]);
  run_http_server(argv[1], port);

  return 0;
}

/*
  validate command line arguments
  Parameter: number of command line arguments(integer), command line arguments(1D string array)
  Return: arguments is valid(bool)
*/
bool checkArgs(int argc, char * argv[]) {
  if (argc < 3) {
      fprintf(stderr, "Wrong server configuration, please follow the usage\n");
      fprintf(stderr, "usage: %s ip port\n", argv[0]);
      return false;
  }

  return true;
}
