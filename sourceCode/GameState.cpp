#include "Game.hpp"
#include "GameState.hpp"
#include <iostream>

#include <cstdlib>
#include <ctime>

template <typename T>
void centerOrigin(T& drawable)
{
    sf::FloatRect bound = drawable.getLocalBounds();
    drawable.setOrigin(bound.width / 2, bound.height / 2);
}
///////////////////////////////
////////// GameState //////////
///////////////////////////////

GameState::GameState(Game* game) : window{game->getWindow()},
                                   parent{game}
{
}
GameState::~GameState() {}
void GameState::update(sf::Clock& clock) {}
void GameState::draw(sf::RenderWindow& window) {}
void GameState::handleInput(sf::Event& e) {}
Game& GameState::getParent()
{
    return *parent;
}
///////////////////////////////////
////////// GetReadyState //////////
///////////////////////////////////

GetReadyState::GetReadyState(Game *game) : GameState{game}
{
    gameTitle.setFont(getParent().getFont());
    gameTitle.setCharacterSize(48);
    gameTitle.setString("================\n    Arkanoid\n================\n");
    gameTitle.setFillColor(sf::Color(255, 255, 85));
    centerOrigin(gameTitle);
    gameTitle.setPosition(window.getSize().x / 2, window.getSize().y / 4);

    welcome.setFont(getParent().getFont());
    welcome.setCharacterSize(24);
    welcome.setString("    !!!! Welcome to Arkanoid !!!!\n\n Hit [SPACE] if You are ready to play,\n     or [ESC] if You want to quit.");
    centerOrigin(welcome);
    welcome.setPosition(585 / 2, 600 / 2);
}
GetReadyState::~GetReadyState()
{
}
void GetReadyState::draw(sf::RenderWindow& window)
{
    window.draw(gameTitle);
    window.draw(welcome);
}
//////////////////////////////////
////////// PlayingState //////////
//////////////////////////////////

PlayingState::PlayingState(Game* game) : GameState(game),
                                         isBallRelesed{false},
                                         score{0},
                                         code{3},
                                         lives{3},
                                         paddleStep{5},
                                         time{0}
{
    lvlImage.loadFromFile("assets/lvl/0" + std::to_string(getParent().getLevelNum()) + ".png");

    textures[tBall].loadFromFile("assets/ball.png");
    textures[tPaddle].loadFromFile("assets/paddle.png");
    textures[tLongPaddle].loadFromFile("assets/paddleLong.png");
    textures[tGray].loadFromFile("assets/grey.png");
    textures[tRed].loadFromFile("assets/red.png");
    textures[tGreen].loadFromFile("assets/green.png");
    textures[tBlue].loadFromFile("assets/blue.png");
    textures[tLightBlue].loadFromFile("assets/lightBlue.png");
    textures[tDrop].loadFromFile("assets/drop.png");

    renderLevel();
    randDrop();

    paddle.setTexture(textures[tPaddle]);
    paddle.setPosition(window.getSize().x / 2, window.getSize().y - paddle.getGlobalBounds().height - 24);

    balls.push_back(Ball(textures[tBall]));
    resetBall();

    stats.setFont(getParent().getFont());
    stats.setCharacterSize(24);
    stats.setFillColor(sf::Color::Cyan);
    stats.setPosition(2, window.getSize().y - 26);

    bonusInfo.setFont(getParent().getFont());
    bonusInfo.setCharacterSize(24);
    bonusInfo.setFillColor(sf::Color::Red);
    bonusInfo.setPosition(window.getSize().x / 2, window.getSize().y - 26);
}
PlayingState::~PlayingState()
{
}
void PlayingState::update(sf::Clock& clock)
{
    stats.setString("Lives: " + std::to_string(lives) + " Score: " + std::to_string(score));

    // paddle moving
    if (code == 1)
        if (paddle.getPosition().x > 0)
        {
            paddle.move(-paddleStep, 0);
            if (!isBallRelesed)
            {
                for (unsigned b = 0; b < balls.size(); b++)
                    balls[b].move(-paddleStep, 0);
            }
        }
    if (code == 2)
        if (paddle.getPosition().x + paddle.getGlobalBounds().width < window.getSize().x)
        {
            paddle.move(paddleStep, 0);
            if (!isBallRelesed)
            {
                for (unsigned b = 0; b < balls.size(); b++)
                    balls[b].move(paddleStep, 0);
            }
        }

    // ball move & collide with blocks Y
    for (unsigned b = 0; b < balls.size(); b++)
        balls[b].move(0, balls[b].offset.y);

    for (unsigned i = 0; i < blocks.size(); i++)
        for (unsigned b = 0; b < balls.size(); b++)
            if (didCollide(blocks[i].body, balls[b]))
            {
                if (blocks[i].hp > 0)
                    balls[b].offset.y = -balls[b].offset.y;
                balls[b].move(0, balls[b].offset.y);
                blockGetDamage(blocks, i);
            }
    // ball move & collide with blocks X
    for (unsigned b = 0; b < balls.size(); b++)
        balls[b].move(balls[b].offset.x, 0);

    for (unsigned i = 0; i < blocks.size(); i++)
        for (unsigned b = 0; b < balls.size(); b++)
            if (didCollide(blocks[i].body, balls[b]))
            {
                if (blocks[i].hp > 0)
                    balls[b].offset.x = -balls[b].offset.x;
                balls[b].move(balls[b].offset.x, 0);
                blockGetDamage(blocks, i);
            }

    // release DROP
    for (unsigned i = 0; i < blocks.size(); i++)
    {
        if (blocks[i].hp <= 0 && blocks[i].willDrop)
        {
            blocks[i].body.setTexture(textures[tDrop]);
            blocks[i].body.move(0, 3);
            if (blocks[i].body.getPosition().y > window.getSize().y) //when DROP falls over the screen
                blocks.erase(blocks.begin() + i);
            else if (didCollide(blocks[i].body, paddle)) //when DROP collide width paddle
            {
                blocks.erase(blocks.begin() + i);
                bonus.draw();
                bonus.execute(paddle, paddleStep, lives, balls, textures);

                if (bonus.drawnBonus == "1up")
                    bonusInfo.setString(bonus.drawnBonus);
                else if (bonus.drawnBonus == "balls")
                    bonusInfo.setString("You got more " + bonus.drawnBonus);
                else if (bonus.drawnBonus == "faster")
                    bonusInfo.setString("Paddle is " + bonus.drawnBonus + " now");
                else if (bonus.drawnBonus == "long")
                    bonusInfo.setString("Paddle is wider now");
                time = 3;
            }
        }
    }
    //draw time for bonus text
    if (clock.getElapsedTime().asSeconds() > 1 && time > 0)
    {
        time--;
        clock.restart();
        if (time <= 0)
            bonusInfo.setString("");
    }

    //get ball position
    for (unsigned b = 0; b < balls.size(); b++)
    {
        balls[b].position = balls[b].getPosition();
    }

    //when ball collide with walls
    for (unsigned b = 0; b < balls.size(); b++)
    {
        if (balls[b].position.x < 0 || balls[b].position.x > window.getSize().x)
            balls[b].offset.x = -balls[b].offset.x;
        if (balls[b].position.y < 0)
            balls[b].offset.y = -balls[b].offset.y;
    }
    //when ball collide with paddle
    for (unsigned b = 0; b < balls.size(); b++)
    {
        if (didCollide(paddle, balls[b]))
        {

            balls[b].offset.y = -balls[b].offset.y;
            balls[b].move(0, -balls[b].getGlobalBounds().height);
            if (balls[b].offset.x > 0)
                balls[b].offset.x = rand() % 4 + 3;
            if (balls[b].offset.x < 0)
                balls[b].offset.x = (rand() % 4 + 3) * -1;
        }
    }

    //when ball is out
    for (unsigned b = 0; b < balls.size(); b++)
    {
        if (balls[b].position.y > window.getSize().y)
        {
            if (balls.size() > 1)
                balls.erase(balls.begin() + b);
            else if (lives == 0)
            {
                getParent().totalScore += score;
                getParent().changeState(GameState::EndGame);
            }
            else if (lives-- > 0)
            {
                bonus.dismiss(paddle, paddleStep, textures); // dismiss bonus
                resetBall();
            }
        }
    }

    //when won
    if (blocks.empty())
    {
        getParent().totalScore += score;
        getParent().levelNumber++;
        if (getParent().levelNumber == 4)
        {
            getParent().gameWon = true;
            getParent().changeState(GameState::EndGame);
        }
        else
            getParent().changeState(GameState::LoadLevel);
    }
}
void PlayingState::draw(sf::RenderWindow& window)
{
    for (auto a : blocks)
    {
        window.draw(a.body);
    }
    window.draw(paddle);
    window.draw(stats);
    window.draw(bonusInfo);
    for (auto a : balls)
    {
        window.draw(a);
    }
}
void PlayingState::handleInput(sf::Event& e)
{
    if (e.type == sf::Event::KeyPressed)
    {
        if (!isBallRelesed)
        {
            if (e.key.code == sf::Keyboard::Up)
                releaseBall();
        }
        if (e.key.code == sf::Keyboard::Left)
            code = 1;
        if (e.key.code == sf::Keyboard::Right)
            code = 2;
        if (e.key.code == sf::Keyboard::Escape)
        {
            getParent().endedByPlayer = true;
            getParent().totalScore += score;
            getParent().changeState(GameState::EndGame);
        }
    }
    if (e.type == sf::Event::KeyReleased)
    {
        if (e.key.code == sf::Keyboard::Left)
            code = 0;
        if (e.key.code == sf::Keyboard::Right)
            code = 0;
    }
}
void PlayingState::renderLevel()
{
    for (unsigned y = 0; y < lvlImage.getSize().y; y++)
        for (unsigned x = 0; x < lvlImage.getSize().x; x++)
        {
            sf::Color cellColor = lvlImage.getPixel(x, y);
            if (cellColor != sf::Color::White)
            {
                Block b;
                if (cellColor == sf::Color::Black) //Black (gray)
                    b = Block(textures[tGray], 2, 15);
                if (cellColor == sf::Color::Red) //Red
                    b = Block(textures[tRed], 2, 15);
                if (cellColor == sf::Color(0, 255, 0)) //Green
                    b = Block(textures[tGreen]);
                if (cellColor == sf::Color(0, 0, 255)) //Blue
                    b = Block(textures[tBlue]);
                if (cellColor == sf::Color(0, 255, 255)) //lightBlue
                    b = Block(textures[tLightBlue]);
                b.body.setPosition(x * 45, y * 15);
                blocks.push_back(b);
            }
        }
}
void PlayingState::randDrop()
{
    for (int i = 0; i < bonus.amount; i++)
    {
        int randIndex = rand() % blocks.size();
        if (blocks[randIndex].willDrop)
            randIndex = rand() % blocks.size();
        blocks[randIndex].willDrop = true;
    }
}
bool PlayingState::didCollide(sf::Sprite& sOne, sf::Sprite& sTwo)
{
    return sOne.getGlobalBounds().intersects(sTwo.getGlobalBounds());
}
void PlayingState::releaseBall()
{
    for (unsigned b = 0; b < balls.size(); b++)
    {
        balls[b].offset.x = -3;
        balls[b].offset.y = -5;
    }
    isBallRelesed = true;
}
void PlayingState::resetBall()
{
    balls[0].offset.x = 0;
    balls[0].offset.y = 0;
    balls[0].setPosition(paddle.getPosition().x + 5, window.getSize().y - paddle.getGlobalBounds().height - balls[0].getGlobalBounds().height - 24);
    isBallRelesed = false;
}
void PlayingState::blockGetDamage(std::vector<Block>& blocks, unsigned &itr)
{
    blocks[itr].hp--;
    if (blocks[itr].hp == 0)
    {
        score += blocks[itr].score;
        if (!blocks[itr].willDrop)
            blocks.erase(blocks.begin() + itr);
    }
}
//////////////////////////////////
////////// LoadLevelState ////////
//////////////////////////////////

LoadLevelState::LoadLevelState(Game* game) : GameState{game},
                                             time{3}
{
    text.setFont(getParent().getFont());
    text.setCharacterSize(24);
    text.setPosition(window.getSize().x / 2, window.getSize().y / 2);

    keys.setFont(getParent().getFont());
    keys.setCharacterSize(24);
    keys.setPosition(window.getSize().x / 2, window.getSize().y * 0.75);
}
LoadLevelState::~LoadLevelState()
{
}
void LoadLevelState::update(sf::Clock& clock)
{
    if (clock.getElapsedTime().asSeconds() > 1)
    {
        text.setString("Level " + std::to_string(getParent().getLevelNum()) + "\nGet ready...\n" + std::to_string(time) + "s");
        time--;
        clock.restart();
        if (time < 0)
        {
            time = 3;
            if (getParent().getLevelNum() > 1)
                getParent().deleteLevel();
            getParent().loadLevel();
            getParent().changeState(GameState::Playing);
        }
    }
    keys.setString("[UP] - Release the ball\n[P] - Pause\n[LEFT] & [RIGHT] - Move paddle\n[ESC] - Quit");
    keys.setFillColor(sf::Color::Cyan);
    centerOrigin(keys);
    centerOrigin(text);
}
void LoadLevelState::draw(sf::RenderWindow& window)
{
    window.draw(text);
    window.draw(keys);
}
//////////////////////////////////
////////// EndGameState //////////
//////////////////////////////////

EndGameState::EndGameState(Game* game) : GameState{game}
{
    text.setFont(getParent().getFont());
    text.setCharacterSize(24);
}
EndGameState::~EndGameState()
{
}
void EndGameState::update(sf::Clock& clock)
{
    if (getParent().endedByPlayer)
    {
        text.setString("You finished the game.\nYour total score: " + std::to_string(getParent().getTotalScore()) + "\n[ESC] - Quit\n[ENTER] - Play again");
    }
    else if (!getParent().gameWon)
    {
        text.setString("You have lost.\nYour total score: " + std::to_string(getParent().getTotalScore()) + "\n[ESC] - Quit\n[ENTER] - Play again");
    }
    else if (getParent().gameWon)
    {
        text.setString("Good job mate!\nYou have won the game.\nYour score: " + std::to_string(getParent().getTotalScore()) + "\n[ESC] - Quit\n[ENTER] - Play again");
    }
    centerOrigin(text);
    text.setPosition(window.getSize().x / 2, window.getSize().y / 2);
}
void EndGameState::draw(sf::RenderWindow& window)
{
    window.draw(text);
}
