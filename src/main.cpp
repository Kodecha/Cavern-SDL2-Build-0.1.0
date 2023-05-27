
#include <iostream>
#include <fstream>
#include <math.h>
#include <string>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include "include/Player.h"
#include "include/Game.h"

//------------------------------------------------------------
// Global Setup
//------------------------------------------------------------

// global constants
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 640;
const int MAP_DISPLAY_WIDTH = 800;
const int MAP_DISPLAY_HEIGHT = 608;
const int MAX_HUNGER = 4000;
const int MAP_WIDTH = 100;
const int MAP_HEIGHT = 100; 
const int FOV = 9;
const int TEXTURE_TILE_SIZE = 32;
const int MAX_MONSTERS = 100;
const int MAX_ITEMS = 100;

//TileSet variables
SDL_Texture* tilesetTexture;
SDL_Texture* vignette;
SDL_Texture* playerTexture;
SDL_Texture* mobTexture;
SDL_Texture* fontTexture;
TTF_Font* font;

//Game variables
SDL_Window* window;
SDL_Renderer* renderer;

//Game objects
Player* player;
Game* game;
