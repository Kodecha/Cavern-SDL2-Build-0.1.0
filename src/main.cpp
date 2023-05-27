#include <iostream>
#include <fstream>
#include <math.h>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

//------------------------------------------------------------
// Global Setup
//------------------------------------------------------------

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 640;
const int MAX_DISPLAY_WIDTH = 800;
const int MAP_DISPLAY_HEIGHT = 608;
const int MAP_WIDTH = 100;
const int MAP_HEIGHT = 100;
const int FOV = 8;
const int TILE_SIZE = 32;
const SDL_Rect HUD_BORDER = {MAX_DISPLAY_WIDTH - 188, MAP_DISPLAY_HEIGHT - 124, 192, 128};
const SDL_Rect HUD_RECT = {MAX_DISPLAY_WIDTH - 190, MAP_DISPLAY_HEIGHT - 126, 192, 128};
const SDL_Rect statsRectBorder = {MAX_DISPLAY_WIDTH - 196, MAX_DISPLAY_WIDTH - 130, 196, 130};
const SDL_Rect statsRect = {MAX_DISPLAY_WIDTH - 192, MAX_DISPLAY_WIDTH - 128, 192, 128};

class Player {
public:
    Player();
    ~Player();
    void drawPlayer();
    void movePlayer();
    void playerStats();
    void setX(int x);
    void setY(int y);
    void setD(int d);
    void setSpeed(int s);
    void setMomentum(int m);
    int getX();
    int getY();
    int getD();
    int getSpeed();
    int getMomentum();

private:
    int X;
    int Y;
    int D;
    int speed;
    int momentum;
    int weight;
};

Player::Player() {
    X = 10;
    Y = 10;
    D = 0;
    speed = 2;
    momentum = 0;
    weight = 1;
}

Player::~Player() {}

void Player::drawPlayer() {}

void Player::movePlayer() {}

void Player::playerStats() {}

void Player::setX(int x) {
    X = x;
}

int Player::getX() {
    return X;
}

void Player::setY(int y) {
    Y = y;
}

int Player::getY() {
    return Y;
}

void Player::setD(int d) {
    D = d;
}

int Player::getD() {
    return D;
}

void Player::setSpeed(int s) {
    speed = s;
}

int Player::getSpeed() {
    return speed;
}

class Game {
public:
    Game();
    ~Game();
    void run();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* tilesetTexture;
    SDL_Texture* playerTexture;
    SDL_Texture* fontTexture;
    TTF_Font* font;

    Player player;
    char map[MAP_WIDTH][MAP_HEIGHT];
    bool gameRunning;

    void drawGame();
    void inputHandle(SDL_Event& event);
    void gameReset();
};

Game::Game() {
    // Initialize SDL
    SDL_Init(SDL_INIT_EVERYTHING);
    window = SDL_CreateWindow("Roguelike", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Init();
    font = TTF_OpenFont("assets/font.ttf", 12);
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    // Load the tileset image to a surface
    SDL_Surface* tilesetSurface = IMG_Load("assets/tileset.png");

    // Create texture from the surface
    tilesetTexture = SDL_CreateTextureFromSurface(renderer, tilesetSurface);

    // Load the player image to a surface
    SDL_Surface* playerSurface = IMG_Load("assets/player.png");

    // Create player texture from the surface
    playerTexture = SDL_CreateTextureFromSurface(renderer, playerSurface);

    // Load the map from a text file
    std::ifstream mapFile("map.txt");
    if (mapFile.is_open()) {
        for (int y = 0; y < MAP_HEIGHT; y++) {
            for (int x = 0; x < MAP_WIDTH; x++) {
                mapFile >> map[x][y];
            }
        }
    }
    // Initialize game variables
    gameRunning = true;
}

Game::~Game() {
    SDL_DestroyTexture(tilesetTexture);
    SDL_DestroyTexture(playerTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
}

void Game::run() {
    SDL_Event event;
    while (gameRunning) {
// check which keys are down each frame
        const Uint8* keys = SDL_GetKeyboardState(NULL);
        if (keys[SDL_SCANCODE_W]) {
            player.setY(player.getY() - player.getSpeed());
        }
        if (keys[SDL_SCANCODE_S]) {
            player.setY(player.getY() + player.getSpeed());
        }
        if (keys[SDL_SCANCODE_A]) {
            player.setX(player.getX() - player.getSpeed());
        }
        if (keys[SDL_SCANCODE_D]) {
            player.setX(player.getX() + player.getSpeed());
        }
        if (keys[SDL_SCANCODE_Q]) {
            player.setD(player.getD() - 1);
        }
        if (keys[SDL_SCANCODE_E]) {
            player.setD(player.getD() + 1);
        }
        if (keys[SDL_SCANCODE_LSHIFT]) {
            player.setSpeed(4);
        } else {
            player.setSpeed(2);
        }
        if (keys[SDL_SCANCODE_ESCAPE]) {
            gameRunning = false;
        }

        // Handle events
        while (SDL_PollEvent(&event)) {
            inputHandle(event);
        }

        // Draw the game
        drawGame();
        SDL_RenderPresent(renderer);
    }
}

void Game::drawGame() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            int screenX = x * TILE_SIZE - player.getX() * TILE_SIZE + MAX_DISPLAY_WIDTH / 2;
            int screenY = y * TILE_SIZE - player.getY() * TILE_SIZE + MAP_DISPLAY_HEIGHT / 2;
            bool visible = true;
            for (int i = 0; i < FOV; i++) {
                int rayX = player.getX() + (x - player.getX()) * i / FOV;
                int rayY = player.getY() + (y - player.getY()) * i / FOV;
                if (map[rayX][rayY] == '#') {
                    visible = false;
                }
            }

            if (visible && sqrt(pow(x - player.getX(), 2) + pow(y - player.getY(), 2)) < FOV) {
                SDL_Rect tilesetRect = {0, 0, TILE_SIZE, TILE_SIZE};
                if (map[x][y] == '#') {
                    tilesetRect.x = 0;
                    tilesetRect.y = 0;
                } else if (map[x][y] == '.') {
                    tilesetRect.x = TILE_SIZE;
                    tilesetRect.y = 0;
                } else if (map[x][y] == 'X') {
                    tilesetRect.x = TILE_SIZE * 2;
                    tilesetRect.y = 0;
                } else if (map[x][y] == ',') {
                    tilesetRect.x = TILE_SIZE * 3;
                    tilesetRect.y = 0;
                }

                SDL_Rect tileRect = {screenX, screenY, TILE_SIZE, TILE_SIZE};
                SDL_RenderCopy(renderer, tilesetTexture, &tilesetRect, &tileRect);
            }
        }
    }

    SDL_Rect playerRect = {MAX_DISPLAY_WIDTH / 2, MAP_DISPLAY_HEIGHT / 2, TILE_SIZE, TILE_SIZE};
    SDL_RenderCopy(renderer, playerTexture, nullptr, &playerRect);
}

void Game::inputHandle(SDL_Event& event) {
    switch (event.key.keysym.sym) {
        case SDLK_w:
            // Update direction angle for upward movement   
            player.setD(0);
            break;
        case SDLK_a:
            // Update direction angle for left movement
            player.setD(1);
        case SDLK_s:
            // Update direction angle for downward movement
            player.setD(2);
            break;
        case SDLK_d:
            // Update direction angle for right movement
            player.setD(3);
            break;
        default:
            break;
        
        case SDLK_SPACE:
            // Update player x and y based on direction
            if (player.getD() == 0) {
                if (map[player.getX()][player.getY() - 1] != '#') {
                    player.setY(player.getY() - 1);
                }
            } else if (player.getD() == 1) {
                if (map[player.getX() - 1][player.getY()] != '#') {
                    player.setX(player.getX() - 1);
                }
            } else if (player.getD() == 2) {
                if (map[player.getX()][player.getY() + 1] != '#') {
                    player.setY(player.getY() + 1);
                }
            } else if (player.getD() == 3) {
                if (map[player.getX() + 1][player.getY()] != '#') {
                    player.setX(player.getX() + 1);
                }
            }
    }
}

void Game::gameReset() {
    player.setX(2);
    player.setY(2);
}

int main(int argc, char *argv[]) {
    Game game;
    game.run();
    return 0;
}