#include <iostream>
#include <fstream>
#include <cmath>
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
    void movePlayer(float deltaTime);
    void playerStats();
    void setX(float x);
    void setY(float y);
    void setRotation(float rotation);
    void setSpeed(float s);
    float getX();
    float getY();
    float getRotation();
    float getSpeed();
    bool isMoving();

private:
    float X;
    float Y;
    float rotation;
    float speed;
    bool isMovingForward;
};

Player::Player() {
    X = 11.0f;
    Y = 11.0f;
    rotation = 0.0f;
    speed = 50.0f;  // Adjust the speed as needed
    isMovingForward = false;
}

Player::~Player() {}

void Player::drawPlayer() {}

void Player::movePlayer(float deltaTime) {
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_A]) {
        rotation -= speed * deltaTime;
    }
    if (keys[SDL_SCANCODE_D]) {
        rotation += speed * deltaTime;
    }
    if (keys[SDL_SCANCODE_SPACE]) {
        isMovingForward = true;
    } else {
        isMovingForward = false;
    }

    if (isMovingForward) {
        X += cos(rotation) * speed * deltaTime;
        Y += sin(rotation) * speed * deltaTime;
    }
}

void Player::playerStats() {}

void Player::setX(float x) {
    X = x;
}

float Player::getX() {
    return X;
}

void Player::setY(float y) {
    Y = y;
}

float Player::getY() {
    return Y;
}

void Player::setRotation(float r) {
    rotation = r;
}

float Player::getRotation() {
    return rotation;
}

void Player::setSpeed(float s) {
    speed = s;
}

float Player::getSpeed() {
    return speed;
}

bool Player::isMoving() {
    return isMovingForward;
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
    SDL_FreeSurface(tilesetSurface);

    // Load the player image to a surface
    SDL_Surface* playerSurface = IMG_Load("assets/player.png");

    // Create player texture from the surface
    playerTexture = SDL_CreateTextureFromSurface(renderer, playerSurface);
    SDL_FreeSurface(playerSurface);

    // Load the map from a text file
    std::ifstream mapFile("map.txt");
    if (mapFile.is_open()) {
        for (int y = 0; y < MAP_HEIGHT; y++) {
            for (int x = 0; x < MAP_WIDTH; x++) {
                mapFile >> map[x][y];
            }
        }
        mapFile.close();
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
    Uint32 lastTime = SDL_GetTicks();
    while (gameRunning) {
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        // Handle events
        while (SDL_PollEvent(&event)) {
            inputHandle(event);
            if (event.type == SDL_QUIT) {
                gameRunning = false;
            }
        }

        // Move the player
        player.movePlayer(deltaTime);

        // Handle collisions with walls
        int playerTileX = static_cast<int>(player.getX() / TILE_SIZE);
        int playerTileY = static_cast<int>(player.getY() / TILE_SIZE);
        if (map[playerTileX][playerTileY] == '#') {
            // Calculate the reflection angle
            float wallAngle = atan2(playerTileY * TILE_SIZE - player.getY(), playerTileX * TILE_SIZE - player.getX());
            float reflectionAngle = 2 * wallAngle - player.getRotation();

            // Reflect the player's rotation
            player.setRotation(reflectionAngle);

            // Move the player away from the wall
            float moveDist = 1.0f;
            player.setX(player.getX() + cos(reflectionAngle) * moveDist);
            player.setY(player.getY() + sin(reflectionAngle) * moveDist);
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
            int screenX = x * TILE_SIZE - static_cast<int>(player.getX()) + MAX_DISPLAY_WIDTH / 2;
            int screenY = y * TILE_SIZE - static_cast<int>(player.getY()) + MAP_DISPLAY_HEIGHT / 2;
            bool visible = true;
            for (int i = 0; i < FOV; i++) {
                float rayX = player.getX() + (x - player.getX() / TILE_SIZE) * i / FOV;
                float rayY = player.getY() + (y - player.getY() / TILE_SIZE) * i / FOV;
                if (map[static_cast<int>(rayX / TILE_SIZE)][static_cast<int>(rayY / TILE_SIZE)] == '#') {
                    visible = false;
                    break;
                }
            }

            if (visible) {
                SDL_Rect tilesetRect = {0, 0, TILE_SIZE, TILE_SIZE};
                if (map[x][y] == '#') {
                    tilesetRect.x = 0;
                    tilesetRect.y = 0;
                } else {
                    tilesetRect.x = 32;
                    tilesetRect.y = 0;
                }

                SDL_Rect dstRect = {screenX, screenY, TILE_SIZE, TILE_SIZE};
                SDL_RenderCopy(renderer, tilesetTexture, &tilesetRect, &dstRect);
            }
        }
    }

    // Draw the player
    SDL_Rect playerRect = {0, 0, TILE_SIZE, TILE_SIZE};
    playerRect.x = MAX_DISPLAY_WIDTH / 2 - TILE_SIZE / 2;
    playerRect.y = MAP_DISPLAY_HEIGHT / 2 - TILE_SIZE / 2;
    SDL_RenderCopyEx(renderer, playerTexture, NULL, &playerRect, player.getRotation() * 180 / M_PI, NULL, SDL_FLIP_NONE);

    // Draw player stats X Y Direction and isMoving
    std::string playerStats = "X: " + std::to_string(player.getX()) + " Y: " + std::to_string(player.getY()) + " Direction: " + std::to_string(player.getRotation()) + " isMoving: " + std::to_string(player.isMoving());
    SDL_Color white = {255, 255, 255};
    SDL_Color black = {0, 0, 0};
    SDL_Surface* playerStatsSurface = TTF_RenderText_Blended(font, playerStats.c_str(), white);
    SDL_Texture* playerStatsTexture = SDL_CreateTextureFromSurface(renderer, playerStatsSurface);
    SDL_Rect playerStatsRect = {0, 0, playerStatsSurface->w, playerStatsSurface->h};
    SDL_Rect playerStatsOutlineRect = {0, 0, playerStatsSurface->w + 2, playerStatsSurface->h + 2};
    playerStatsRect.x = 10;
    playerStatsRect.y = 10;
    playerStatsOutlineRect.x = 10;
    playerStatsOutlineRect.y = 10;
    SDL_SetRenderDrawColor(renderer, black.r, black.g, black.b, 255);
    SDL_RenderFillRect(renderer, &playerStatsOutlineRect);
    SDL_SetRenderDrawColor(renderer, white.r, white.g, white.b, 255);
    SDL_RenderFillRect(renderer, &playerStatsRect);
    SDL_RenderCopy(renderer, playerStatsTexture, NULL, &playerStatsRect);
    SDL_FreeSurface(playerStatsSurface);
    SDL_DestroyTexture(playerStatsTexture);
}

void Game::inputHandle(SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_ESCAPE) {
            gameRunning = false;
        }
    } else if (event.type == SDL_KEYUP) {
        if (event.key.keysym.sym == SDLK_SPACE) {
            // Stop the player
            player.setSpeed(0.0f);
        }
    }
}

void Game::gameReset() {
    // Reset the game state to the initial state
    // ...
}

int main(int argc, char** argv) {
    Game game;
    game.run();

    return 0;
}