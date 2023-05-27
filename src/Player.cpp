#include "include/Player.h"

Player::Player(char name, int x, int y, int hp, int hunger, int atk, int type)
{
    this->x = x;
    this->y = y;
    this->hp = hp;
}

int Player::getX()
{
    return x;
}

int Player::getY()
{
    return y;
}

int Player::getHP()
{
    return hp;
}

void Player::setX(int x)
{
    this->x = x;
}

void Player::setY(int y)
{
    this->y = y;
}

void Player::setHP(int hp)
{
    this->hp = hp;
}


