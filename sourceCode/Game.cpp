#include "Game.hpp"
#include "GameState.hpp"

Game::Game():
    window{sf::VideoMode(585, 600), "ArkanoidGame!"},
    endedByPlayer{false},
    gamePaused{false},
    gameWon{false},
    totalScore{0},
    levelNumber{1}
{
    window.setFramerateLimit(60);
    font.loadFromFile("assets/vgaSquarepx.ttf");

    states[GameState::GetReady] = new GetReadyState(this);
    states[GameState::Playing] = nullptr;
    states[GameState::LoadLevel] = new LoadLevelState(this);
    states[GameState::EndGame] = new EndGameState(this);


    changeState(GameState::GetReady);
}
Game::~Game()
{
    for(GameState* state : states)
    {
        delete state;
    }
}
void Game::run()
{
    sf::Clock c;
    sf::Event e;
    while(window.isOpen())
    {
        while(window.pollEvent(e))
        {
            if(e.type == sf::Event::Closed)
                window.close();

            if(e.type == sf::Event::KeyPressed)
            {
                if(e.key.code == sf::Keyboard::Space)               //Space
                {
                    if(currentState == states[GameState::GetReady])
                        changeState(GameState::LoadLevel);
                }
                if(e.key.code == sf::Keyboard::Enter)               //Enter
                {
                    if(currentState == states[GameState::EndGame])
                    {
                        resetGame();
                        changeState(GameState::GetReady);
                    }
                }
                if(e.key.code == sf::Keyboard::Escape)              //Escape
                {
                    if(currentState == states[GameState::EndGame] || currentState == states[GameState::GetReady])
                        window.close();
                }
                if(e.key.code == sf::Keyboard::P)                   //P
                {
                    if(currentState == states[GameState::Playing])
                        gamePaused = !gamePaused;
                }
            }
            currentState->handleInput(e);
        }
        if(!gamePaused)
            currentState->update(c);
        window.clear();
        currentState->draw(window);
        window.display();
    }
}
void Game::changeState(GameState::States state)
{
    currentState = states[state];
}
sf::Font& Game::getFont()
{
    return font;
}
sf::RenderWindow& Game::getWindow()
{
    return window;
}
int Game::getTotalScore() const
{
    return totalScore;
}
int Game::getLevelNum() const
{
    return levelNumber;
}
void Game::resetGame()
{
    levelNumber = 1;
    totalScore = 0;
    endedByPlayer = false;
}
void Game::deleteLevel()
{
   delete states[GameState::Playing];
   states[GameState::Playing] = nullptr;
}
void Game::loadLevel()
{
    states[GameState::Playing] = new PlayingState(this);
}




















