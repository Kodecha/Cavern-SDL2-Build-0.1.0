
#include "Game.h"

Game::Game()
{
    init();
}

void Game::init()
{
    // Initialize SDL
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    // Create window
    window = SDL_CreateWindow("Cavern", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Load textures
    playerTexture = IMG_LoadTexture(renderer, "assets/player.png");
    textureTileSet = IMG_LoadTexture(renderer, "assets/tileset.png");
    font = TTF_OpenFont("assets/arial.ttf", 24);

    // Load Map File and initialize memory map / map memory
    std::ifstream mapFile;
    mapFile.open("assets/map.txt");
    if (mapFile.is_open())
    {
        for (int i = 0; i < MAP_WIDTH; i++)
        {
            for (int j = 0; j < MAP_HEIGHT; j++)
            {
                mapFile >> map[i][j];
                memoryMap[i][j] = 0;
            }
        }
    }

    // Initialize player
    player = new Player(3, 3, 100);

    running = true;
}

void Game::render(){
    // Clear screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Render map
    for (int i = 0; i < MAP_WIDTH; i++)
    {
        for (int j = 0; j < MAP_HEIGHT; j++)
        {
            // Render map tiles
            SDL_Rect srcRect = {map[i][j] * 32, 0, 32, 32};
            SDL_Rect destRect = {i * 32, j * 32, 32, 32};
            SDL_RenderCopy(renderer, textureTileSet, &srcRect, &destRect);

            // Render memory map
            if (memoryMap[i][j] == 1)
            {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderFillRect(renderer, &destRect);
            }
        }
    }

    // Render player
    SDL_Rect srcRect = {0, 0, 32, 32};
    SDL_Rect destRect = {player->getX() * 32, player->getY() * 32, 32, 32};
    SDL_RenderCopy(renderer, playerTexture, &srcRect, &destRect);

    // Render text
    SDL_Color color = {255, 255, 255};
    std::string hpString = "HP: " + std::to_string(player->getHP());
    SDL_Surface *surface = TTF_RenderText_Solid(font, hpString.c_str(), color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect textRect = {0, 0, 100, 100};
    SDL_RenderCopy(renderer, texture, NULL, &textRect);

    // Render to screen
    SDL_RenderPresent(renderer);
}

char Game::checkInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            running = false;
            return 'q';
        }
        else if (event.type == SDL_KEYDOWN)
        {
            switch (event.key.keysym.sym)
            {
            case SDLK_UP:
                return 'w';
                break;
            case SDLK_DOWN:
                return 's';
                break;
            case SDLK_LEFT:
                return 'a';
                break;
            case SDLK_RIGHT:
                return 'd';
                break;
            case SDLK_ESCAPE:
                running = false;
                return 'q';
                break;
            }
        }
    }
    return ' ';
}

void update(char input)
{
    switch (input)
    {
    case 'w':
        if (map[player->getX()][player->getY() - 1] != 1)
        {
            player->setY(player->getY() - 1);
        }
        break;
    case 's':
        if (map[player->getX()][player->getY() + 1] != 1)
        {
            player->setY(player->getY() + 1);
        }
        break;
    case 'a':
        if (map[player->getX() - 1][player->getY()] != 1)
        {
            player->setX(player->getX() - 1);
        }
        break;
    case 'd':
        if (map[player->getX() + 1][player->getY()] != 1)
        {
            player->setX(player->getX() + 1);
        }
        break;
    }
}

Game::~Game()
{
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(playerTexture);
    SDL_DestroyTexture(textureTileSet);
    TTF_CloseFont(font);
    SDL_Quit();
}