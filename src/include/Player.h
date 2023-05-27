#ifndef PLAYER_H
#define PLAYER_H

class Player
{
    private:   
        int x;
        int y;
        int hp;
    public:
        Player(int x, int y, int hp)
        {
            this->x = x;
            this->y = y;
            this->hp = hp;
        }
        int getX()
        {
            return x;
        }
        int getY()
        {
            return y;
        }
        int getHP()
        {
            return hp;
        }
        void setX(int x)
        {
            this->x = x;
        }
        void setY(int y)
        {
            this->y = y;
        }
        void setHP(int hp)
        {
            this->hp = hp;
        }
};
#endif