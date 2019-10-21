#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

#include "Block.cpp"

class Game;

////////// GameState //////////

class GameState
{
public:
    enum States
    {
        GetReady,
        Playing,
        LoadLevel,
        EndGame
    };
    GameState(Game* game);
    virtual ~GameState();
    virtual void update(sf::Clock& clock);
    virtual void draw(sf::RenderWindow& window);
    virtual void handleInput(sf::Event& e);
    Game& getParent();

    sf::RenderWindow& window;

private:
    Game* parent;
};

////////// GetReadyState //////////

class GetReadyState : public GameState
{
public:
    GetReadyState(Game* game);
    ~GetReadyState();
    void draw(sf::RenderWindow& window) override;

private:
    sf::Text gameTitle;
    sf::Text welcome;
};

////////// PlayingState //////////

class PlayingState : public GameState
{
public:
    PlayingState(Game* game);
    ~PlayingState();
    void update(sf::Clock& clock) override;
    void draw(sf::RenderWindow& window) override;
    void handleInput(sf::Event& e) override;

    friend struct Bonus;

private:
    void renderLevel();
    void randDrop();
    bool didCollide(sf::Sprite& sOne, sf::Sprite& sTwo);
    void releaseBall();
    void resetBall();
    void blockGetDamage(std::vector<Block>& blocks, unsigned& itr);
    void executeBonus();
    enum
    {
        tBall,
        tPaddle,
        tLongPaddle,
        tGray,
        tRed,
        tGreen,
        tBlue,
        tLightBlue,
        tDrop
    };
    bool isBallRelesed;
    int score;
    int code;
    int lives;
    int paddleStep; // default 5, bonus 8
    int time;
    sf::Text stats;
    sf::Text bonusInfo;
    sf::Image lvlImage;
    sf::Texture textures[9];
    std::vector<Block> blocks;
    std::vector<Ball> balls;
    sf::Sprite paddle;
    Bonus bonus;
};

////////// LoadLevelState ////////

class LoadLevelState : public GameState
{
public:
    LoadLevelState(Game* game);
    ~LoadLevelState();
    void update(sf::Clock& clock) override;
    void draw(sf::RenderWindow& window) override;

private:
    int time;
    sf::Text text;
    sf::Text keys;
};
////////// EndGameState //////////

class EndGameState : public GameState
{
public:
    EndGameState(Game* game);
    ~EndGameState();
    void update(sf::Clock& clock) override;
    void draw(sf::RenderWindow& window) override;

private:
    sf::Text text;
};

#endif // GAMESTATE_HPP
