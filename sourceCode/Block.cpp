#include<SFML/Graphics.hpp>
#include<iostream>
#include<string>
#include<vector>

class PlayingState;

struct Block
{
    Block(){}
    Block(sf::Texture& texture, int health = 1, int points = 5, bool drop = false)
    {
        body.setTexture(texture);
        hp = health;
        score = points;
        willDrop = drop;
    }
    sf::Sprite body;
    int hp;
    int score;
    bool willDrop;
};

struct Ball : public sf::Sprite
{
    Ball(const sf::Texture& texture):
        sf::Sprite(texture)
    {}
    sf::Vector2f position;
    sf::Vector2f offset;
};

struct Bonus
{
    Bonus()
    {
        bonuses.push_back("1up");
        bonuses.push_back("long");
        bonuses.push_back("balls");
        bonuses.push_back("faster");
        bonuses.push_back("balls");
        bonuses.push_back("faster");
        amount = bonuses.size();
    }
    std::string draw()
    {
        int randIndex = rand()%bonuses.size();
        std::string bonus = bonuses[randIndex];
        drawnBonus = bonuses[randIndex];
        bonuses.erase(bonuses.begin()+randIndex);
        return bonus;
    }
    void execute(sf::Sprite& paddle, int& speed, int& lives, std::vector<Ball>& balls, sf::Texture textures[9])
    {
        if(drawnBonus == "1up")
            lives++;
        if(drawnBonus == "long")
        {
            isExtended = true;
            paddle.setTextureRect(sf::IntRect(paddle.getTextureRect().left, paddle.getTextureRect().top, textures[2].getSize().x,textures[2].getSize().y));
            paddle.setTexture(textures[2]);
        }
        if(drawnBonus == "faster")
        {
            isFaster = true;
            speed = 8;

        }
        if(drawnBonus == "balls")
        {
            for(unsigned n = 1; n < 3; n++ )
            {
                Ball b(textures[0]);
                b.setPosition(balls[n-1].getPosition());
                b.offset = balls[n-1].offset;
                balls.push_back(b);
            }
        }
    }
    void dismiss(sf::Sprite& paddle, int& speed, sf::Texture textures[9] )
    {
        if(isFaster)
            speed = 5;

        if(isExtended)
        {
            paddle.setTextureRect(sf::IntRect(paddle.getTextureRect().left, paddle.getTextureRect().top, textures[1].getSize().x, textures[1].getSize().y));
            paddle.setTexture(textures[1]);
        }

    }

    int amount;
    std::string drawnBonus;
private:
    std::vector<std::string> bonuses;
    bool isExtended;
    bool isFaster;
};


