#ifndef GAME_H
#define GAME_H


class Game {

private:
char map[100][100];
int mapMemory[100][100];
bool running;

public:

    Game();
    void init();
    void render();
    char checkInput();
    void update(char input);
};

#endif