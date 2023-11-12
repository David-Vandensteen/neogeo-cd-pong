![Platform](https://img.shields.io/badge/platform-%20%7C%20windows-lightgrey) ![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)  

# Pong Game for Neo Geo CD using Neocore Kit  
  
![](https://media.giphy.com/media/mDknOSUEaP0zcI7tzV/giphy.gif)  

----------- **_Visuals by Grass_** -----------
  

This repository contains a simple implementation of the classic game Pong for the Neo Geo CD platform, utilizing the Neocore Kit.  
It can be used as a starting point for a Pong-style game or for learning the Neocore library.  
https://github.com/David-Vandensteen/neocore  


## Introduction  

Pong is a two-player arcade game where players control paddles on opposite sides of the screen and use them to hit a ball back and forth.  
The objective is to prevent the ball from passing your own paddle while trying to make the ball pass your opponent's paddle.  


## How to Play  

1) **Game Start:** When the game is launched, the title screen will be displayed. Press the "START" button on the controller to initiate the game.

2) **Player Controls:**

Player 1 (Left Paddle): Use the "UP" and "DOWN" buttons on the controller to move the paddle up and down, respectively.  
Player 2 (Right Paddle): The AI-controlled paddle automatically moves to intercept the ball.  

Gameplay: The ball will start moving across the screen, bouncing off the paddles and walls. The objective is to prevent the ball from passing your paddle while trying to make the ball pass your opponent's paddle.

3) **Win condition:** If the ball passes the opponent's paddle and reaches the edge of the screen, the human player wins  

## Code Structure  

The codebase is organized into several functions, each responsible for a specific aspect of the game.

- wait_game_start(): Waits for the player to press the "START" button to begin the game.
- get_next_ball_position(): Calculates the next position of the ball based on its current position, slope, direction, and speed.
- init_ball_state(): Initializes the state of the ball with an initial slope, direction, and position.
- init(): Initializes the game environment and elements, including paddles, ball, and walls.
- display(): Displays the game elements on the screen, including the playfield, paddles, and ball.
- update_logic(): Checks if the ball has hit the left or right edge of the screen and handles game over conditions.
- slope_ball(): Determines the ball's slope based on its position relative to a paddle.
- update_ball(): Updates the ball's position, manages collisions, and adjusts slope and direction.
- update_player(): Allows the player to control their paddle using the controller's "UP" and "DOWN" buttons.
- update_ia(): Controls the AI-controlled paddle's movement to intercept the ball.
- update(): Main game update loop that orchestrates game logic, ball movement, player paddle, and AI paddle positions.
- main(): Entry point of the game, initializes the game, displays the initial state, and enters the main game loop.

Overall, this code provides the foundation or a bootstrap for a simple Pong game on a Neo Geo CD system using the "Neocore" library.  

To make a fully functional and polished Pong game, you can consider adding more features such as scoring, multiple levels of difficulty, better collision handling, and graphical enhancements.

## License  

This Pong game is provided under the MIT License, allowing you to use, modify, and distribute the code freely.  

Feel free to explore, enhance, and enjoy this implementation of the classic Pong game on the Neo Geo CD platform!

## Clone this repo
```cmd
git clone git@github.com:David-Vandensteen/neogeo-cd-pong.git
```

## Building
```cmd
cd neogeo-cd-pong\src
```

```cmd
mak
```

## Testing

**With Raine**
```cmd
mak run:raine
```

**With Mame**
```cmd
mak run:mame
```

## Release
Build a iso & chd to distribute the game :  
```cmd
mak dist:iso
```
```cmd
mak dist:mame
```
