#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <vector>
#include <SFML/Graphics.hpp>
#include <windows.h>

void log(const char* str) {
    std::cout << str << std::endl;
}

void log(const std::string& str) {
    std::cout << str << std::endl;
}

void errmsg(const char* str) {
    std::cerr << str << std::endl;
    MessageBoxA(NULL, str, "Erreur", MB_OK | MB_ICONERROR);
}

struct Brick {
    sf::RectangleShape shape;
    bool alive = true;
    int hp = 1;
    int maxHp = 1;
    int points = 10;
};

enum Difficulty {
    EASY,
    EXPERT
};

sf::Color lerpColor(sf::Color a, sf::Color b, float t) {
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;

    return sf::Color(
        static_cast<sf::Uint8>(a.r + (b.r - a.r) * t),
        static_cast<sf::Uint8>(a.g + (b.g - a.g) * t),
        static_cast<sf::Uint8>(a.b + (b.b - a.b) * t),
        static_cast<sf::Uint8>(a.a + (b.a - a.a) * t)
    );
}

void updateBrickColor(Brick& brick) {
    if (brick.maxHp == 1) brick.shape.setFillColor(sf::Color(80, 220, 120));
    else if (brick.maxHp == 2) brick.shape.setFillColor(sf::Color(80, 180, 255));
    else if (brick.maxHp == 3) brick.shape.setFillColor(sf::Color(255, 180, 60));
    else brick.shape.setFillColor(sf::Color(220, 60, 255));

    if (brick.hp < brick.maxHp) {
        sf::Color c = brick.shape.getFillColor();
        brick.shape.setFillColor(sf::Color(c.r / 2, c.g / 2, c.b / 2));
    }
}

void generateBricks(std::vector<Brick>& bricks, int level) {
    bricks.clear();

    int rows = 4 + level;
    if (rows > 9) rows = 9;

    int cols = 12;

    float startX = 200.0f;
    float startY = 100.0f - level * 8.0f;

    if (startY < 35.0f) startY = 35.0f;

    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            Brick brick;

            int difficulty = 1 + (row / 2) + (level / 2);
            if (difficulty > 4) difficulty = 4;

            brick.hp = difficulty;
            brick.maxHp = difficulty;
            brick.points = difficulty * 10;

            brick.shape.setSize(sf::Vector2f(120.0f, 35.0f));
            brick.shape.setPosition(startX + col * 130.0f, startY + row * 45.0f);

            updateBrickColor(brick);
            bricks.push_back(brick);
        }
    }
}

bool allBricksDestroyed(const std::vector<Brick>& bricks) {
    for (const auto& brick : bricks) {
        if (brick.alive) return false;
    }
    return true;
}

int aliveBricksCount(const std::vector<Brick>& bricks) {
    int count = 0;

    for (const auto& brick : bricks) {
        if (brick.alive) count++;
    }

    return count;
}

bool removeOneLastBrick(std::vector<Brick>& bricks, int& score) {
    for (auto& brick : bricks) {
        if (brick.alive) {
            brick.alive = false;
            score += brick.points;
            return true;
        }
    }

    return false;
}

int main() {
    int lifes = 3;
    int score = 0;
    int level = 1;
    int uselessShots = 0;

    bool difficultyChosen = false;
    Difficulty difficulty = EASY;

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Breakout In C++");

    sf::CircleShape ball(20.0f);
    sf::RectangleShape box(sf::Vector2f(1920, 1080));
    sf::RectangleShape paddle(sf::Vector2f(100, 15));

    sf::Texture boxTexture;
    sf::Font font;

    if (!font.loadFromFile("arial.ttf")) {
        errmsg("Impossible de charger la police.");
        return 1;
    }

    sf::Text scoreText;
    sf::Text lifesText;
    sf::Text levelText;
    sf::Text menuText;
    sf::Text helpText;

    scoreText.setFont(font);
    scoreText.setCharacterSize(32);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(20, 20);

    lifesText.setFont(font);
    lifesText.setCharacterSize(32);
    lifesText.setFillColor(sf::Color::White);
    lifesText.setPosition(20, 60);

    levelText.setFont(font);
    levelText.setCharacterSize(32);
    levelText.setFillColor(sf::Color::White);
    levelText.setPosition(20, 100);

    menuText.setFont(font);
    menuText.setCharacterSize(60);
    menuText.setFillColor(sf::Color::White);
    menuText.setString("Choisis la difficulte\n\n1 - Facile\n2 - Expert");
    menuText.setPosition(650, 350);

    helpText.setFont(font);
    helpText.setCharacterSize(26);
    helpText.setFillColor(sf::Color::White);
    helpText.setPosition(20, 140);

    float ballX = 936;
    float ballY = 930;
    float ballSpeedX = 3.0f;
    float ballSpeedY = -3.0f;

    float paddleX = 936;
    float paddleY = 965;

    paddle.setFillColor(sf::Color::White);
    ball.setFillColor(sf::Color::White);

    if (!boxTexture.loadFromFile("background.jpg")) {
        errmsg("Impossible de charger l'arrière plan.");
        return 1;
    }

    box.setTexture(&boxTexture, true);
    window.setFramerateLimit(120);

    std::vector<Brick> bricks;
    generateBricks(bricks, level);

    sf::Clock clock;

    bool gameOver = false;
    bool gameOverLogged = false;

    float fadeTimer = 0.0f;
    float fadeDuration = 2.0f;

    sf::Color fadeStartColor(90, 0, 0);
    sf::Color fadeEndColor(255, 90, 90);

    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();

        sf::Event event{};

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }

            if (!difficultyChosen && event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Num1 || event.key.code == sf::Keyboard::Numpad1) {
                    difficulty = EASY;
                    difficultyChosen = true;
                }

                if (event.key.code == sf::Keyboard::Num2 || event.key.code == sf::Keyboard::Numpad2) {
                    difficulty = EXPERT;
                    difficultyChosen = true;
                }
            }
        }

        if (!difficultyChosen) {
            window.clear();
            window.draw(box);
            window.draw(menuText);
            window.display();
            continue;
        }

        if (!gameOver) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) {
                uselessShots = 19;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                paddleX -= 10;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::F10)) {
                lifes = 5000;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                paddleX += 10;
            }

            if (paddleX < 0.0f) {
                paddleX = 0.0f;
            }

            if (paddleX + paddle.getSize().x > box.getSize().x) {
                paddleX = box.getSize().x - paddle.getSize().x;
            }

            ballX += ballSpeedX;
            ballY += ballSpeedY;

            if (ballY <= 0.0f) {
                ballY = 0.0f;
                ballSpeedY = -ballSpeedY;
            }

            if (ballX <= 0.0f) {
                ballX = 0.0f;
                ballSpeedX = -ballSpeedX;
            }

            if (ballX + ball.getRadius() * 2 >= box.getSize().x) {
                ballX = box.getSize().x - ball.getRadius() * 2;
                ballSpeedX = -ballSpeedX;
            }

            if (ballY + ball.getRadius() * 2 >= box.getSize().y) {
                ballY = box.getSize().y - ball.getRadius() * 2;
                ballSpeedY = -ballSpeedY;
                lifes--;
                uselessShots = 0;

                if (lifes <= 0) {
                    gameOver = true;
                }
            }

            ball.setPosition(ballX, ballY);
            paddle.setPosition(paddleX, paddleY);

            if (ball.getGlobalBounds().intersects(paddle.getGlobalBounds())) {
                if (ball.getGlobalBounds().intersects(paddle.getGlobalBounds())) {
                    float ballCenter = ballX + ball.getRadius();
                    float paddleCenter = paddleX + paddle.getSize().x / 2.0f;

                    float hitPosition = (ballCenter - paddleCenter) / (paddle.getSize().x / 2.0f);

                    if (hitPosition < -1.0f) hitPosition = -1.0f;
                    if (hitPosition > 1.0f) hitPosition = 1.0f;

                    float speed = std::sqrt(ballSpeedX * ballSpeedX + ballSpeedY * ballSpeedY);

                    ballSpeedX = hitPosition * speed;
                    ballSpeedY = -std::abs(speed * 0.85f);

                    if (std::abs(ballSpeedX) < 1.5f) {
                        ballSpeedX = ballSpeedX < 0 ? -1.5f : 1.5f;
                    }

                    ballY = paddleY - ball.getRadius() * 2;
                    uselessShots++;
                }
            }

            bool brickHit = false;

            for (auto& brick : bricks) {
                if (brick.alive && ball.getGlobalBounds().intersects(brick.shape.getGlobalBounds())) {
                    brick.hp--;
                    brickHit = true;
                    uselessShots = 0;

                    if (brick.hp <= 0) {
                        brick.alive = false;
                        score += brick.points;
                    } else {
                        updateBrickColor(brick);
                    }

                    ballSpeedY = -ballSpeedY;
                    break;
                }
            }

            if (difficulty == EASY && !brickHit && uselessShots >= 20 && aliveBricksCount(bricks) <= 5) {
                if (removeOneLastBrick(bricks, score)) {
                    uselessShots = 0;
                }
            }

            if (allBricksDestroyed(bricks)) {
                level++;
                uselessShots = 0;

                float speedBoost = 0.4f;

                if (ballSpeedX > 0) ballSpeedX += speedBoost;
                else ballSpeedX -= speedBoost;

                if (ballSpeedY > 0) ballSpeedY += speedBoost;
                else ballSpeedY -= speedBoost;

                float newPaddleWidth = 100.0f - level * 4.0f;
                if (newPaddleWidth < 55.0f) {
                    newPaddleWidth = 55.0f;
                }

                paddle.setSize(sf::Vector2f(newPaddleWidth, 15.0f));

                ballX = 936;
                ballY = 930;
                paddleX = 936;

                ball.setPosition(ballX, ballY);
                paddle.setPosition(paddleX, paddleY);

                generateBricks(bricks, level);
            }
        }

        if (gameOver) {
            if (!gameOverLogged) {
                log("lifes = " + std::to_string(lifes));
                gameOverLogged = true;
            }

            fadeTimer += deltaTime;

            float t = fadeTimer / fadeDuration;
            sf::Color fadeColor = lerpColor(fadeStartColor, fadeEndColor, t);

            box.setTexture(nullptr);
            box.setFillColor(fadeColor);

            if (t >= 1.0f) {
                lifes = 3;
                score = 0;
                level = 1;
                uselessShots = 0;

                ballX = 936;
                ballY = 930;

                ballSpeedX = 3.0f;
                ballSpeedY = -3.0f;

                paddleX = 936;
                paddleY = 965;

                paddle.setSize(sf::Vector2f(100.0f, 15.0f));

                generateBricks(bricks, level);

                box.setTexture(&boxTexture, true);

                fadeTimer = 0.0f;
                gameOver = false;
                gameOverLogged = false;
                difficultyChosen = false;
            }
        }

        scoreText.setString("Score: " + std::to_string(score));
        lifesText.setString("Vies: " + std::to_string(lifes));
        levelText.setString("Niveau: " + std::to_string(level));

        if (difficulty == EASY) {
            helpText.setString("Mode: Facile | Shots inutiles: " + std::to_string(uselessShots) + "/20");
        } else {
            helpText.setString("Mode: Expert");
        }

        window.clear();

        window.draw(box);

        for (const auto& brick : bricks) {
            if (brick.alive) {
                window.draw(brick.shape);
            }
        }

        window.draw(paddle);
        window.draw(ball);

        window.draw(scoreText);
        window.draw(lifesText);
        window.draw(levelText);
        window.draw(helpText);

        window.display();
    }

    return 0;
}