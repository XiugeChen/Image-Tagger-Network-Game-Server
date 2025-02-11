[Beej's Guide to Network Programming]: http://www.beej.us/guide/bgnet/html/multi/index.html

# Image Tagger, A Network Game Server

## Introduction

* Project 1 for [COMP30023](https://handbook.unimelb.edu.au/2020/subjects/comp30023) (Computer System) at the University of Melbourne, 2019 Sem1.

* A network game server that tags images as people play. (The idea was invented
by the inventor of re-Captcha and Duo-lingo)

* The game consists of two players and one server, where each of the players can
only communicate with the server (but the other player).
At the beginning, when the two players log on to the server, the server sends
both of them a same image. he game then starts;
the players submit words or phrases to the server, one at a time, without being
told what the other player has inputed. The goal for both the players is to
enter a word or a phrase that has been submitted by the other as soon as possible.
Once the goal is achieved, the game ends; the server sends a web page indicating
that the game is completed and prompting the players to play again.
If both the players agree to play again, the process repeats with a new image.

* For detailed description please check out [project specification](resources/project_description.pdf)

## Usage (Linux)

To compile:

`make all`

To execute after compile:

`cd bin`

`./image_tagger <server_ip> <port_number>`

To clean up executable and object files and directories:

`make clean`

## Contribution
Xiuge Chen

xiugec@student.unimelb.edu.au

Subject: COMP30023 Computer Systems

University of Melbourne

Some code bring ideas from lab5/6 of comp30023 and [Beej's Guide to Network Programming]
