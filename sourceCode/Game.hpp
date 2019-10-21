#ifndef GAME_HPP
#define GAME_HPP

#include"GameState.hpp"

#include<SFML/Graphics.hpp>
#include<array>

class Game
{
public:
    Game();
    ~Game();
    void run();
    void changeState(GameState::States state);

    sf::Font& getFont();
    sf::RenderWindow& getWindow();
    int getTotalScore() const;
    int getLevelNum() const;

    void resetGame();
    void deleteLevel();
    void loadLevel();

    friend class PlayingState;
    friend class EndGameState;

private:
    sf::RenderWindow window;
    std::array<GameState*, 4> states;
    GameState* currentState;
    sf::Font font;
    bool endedByPlayer;
    bool gamePaused;
    bool gameWon;
    int totalScore;
    int levelNumber;
};

#endif // GAME_HPP
