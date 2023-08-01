# neogeo-cd-pong

This code is a simple implementation of the classic game Pong for the Neo Geo CD,
using a library called "Neocore" (https://github.com/David-Vandensteen/neocore) for operations, graphics and building.

The code defines several constants for screen dimensions, speed of the paddles and ball, and a timeout for the AI's direction change.
It also defines an enumeration to represent the possible directions of the ball (NONE, UP, DOWN, LEFT, RIGHT).

A structure named "BallState" is defined to store the current state of the ball, including its position, slope, and direction.

Two static boxes represent the upper and lower walls of the game.  
There are two paddles (racquets) for the players, represented by an array of "GFX_Picture_Physic" structures.

The ball is also represented by a "GFX_Picture_Physic" structure.  
Another variable keeps track of the current direction of the AI (computer-controlled paddle) and a timeout counter for its direction change.

The function "get_next_ball_position" calculates the next position of the ball based on its current slope and direction.  
The "init_racquet" function initializes a racquet with graphics.  
The "init_ball_state" function sets the initial state of the ball.  

The "init" function initializes the game by setting up the GPU, initializing racquets and the ball, and positioning the walls.  
The "display" function shows the game elements on the screen and initializes the ball's state.  
The "update_logic" function checks if the ball has hit the left or right edge of the screen (the game boundary) and displays the winner when the game ends.  
The "slope_ball" function determines the slope of the ball based on its position relative to a given racquet.  
The "update_ball" function updates the ball's position, handles collisions with walls and racquets, and performs reflection for more realistic movement.  

The "update_player" function updates the player-controlled racquet based on joystick input.  
The "update_ia" function updates the AI-controlled racquet, which tracks the ball's position and moves accordingly.  

The "update" function is responsible for updating the game state by calling "update_logic", "update_ball", "update_player", and "update_ia" functions.

Finally, the main function initializes the game, displays the elements, and enters a loop to continuously update the game state while waiting for vertical blanking.
The game loop updates the game state and refreshes the screen accordingly.

Overall, this code provides the foundation for a simple Pong game on a Neo Geo CD system using the "Neocore" library.  
To make a fully functional and polished Pong game, you can consider adding more features such as scoring, multiple levels of difficulty, better collision handling, and graphical enhancements.
