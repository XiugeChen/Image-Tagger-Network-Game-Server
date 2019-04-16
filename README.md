## DESCRIPTION

Project 1 for COMP30023 Computer System at University of Melbourne

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

For detailed description please look at resources/project_description.pdf

## COMMAND (Linux)

To compile:
`cd code`
`make all`

To execute after compile:
`cd code/bin`
`./image_tagger <server_ip> <port_number>`

To clean up executable and object files and directories:
`cd code`
`make clean`

## ATTRIBUTION

xiugec@student.unimelb.edu.au
Subject: COMP30023 Computer Systems
University of Melbourne

Some code bring ideas from lab5/6 of comp30023
