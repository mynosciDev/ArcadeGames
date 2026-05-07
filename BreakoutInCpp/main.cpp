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

void msg(const std::string& str) {
    MessageBoxA(NULL, str.c_str(), "Pong de wish", MB_OK | MB_ICONINFORMATION);
}

struct Brick {
    sf::RectangleShape shape;
    bool alive = true;
};

std::string getExeDirectory() {
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string path(buffer);
    size_t pos = path.find_last_of("\\/");
    return path.substr(0, pos);
}

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

int main() {
    int lifes = 3;
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Breakout In C++");

    sf::CircleShape ball(20.0f);
    sf::RectangleShape box(sf::Vector2f(1920, 1080));
    sf::RectangleShape paddle(sf::Vector2f(100, 15));

    sf::Texture boxTexture;

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

    std::vector<sf::Color> brickColors = {
        sf::Color(255, 80, 80),
        sf::Color(255, 160, 60),
        sf::Color(255, 230, 80),
        sf::Color(80, 220, 120),
        sf::Color(80, 180, 255),
        sf::Color(180, 100, 255)
    };

    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 12; col++) {
            Brick brick;
            brick.shape.setSize(sf::Vector2f(120, 35));
            brick.shape.setFillColor(brickColors[row % brickColors.size()]);
            brick.shape.setPosition(200 + col * 130, 100 + row * 45);
            bricks.push_back(brick);
        }
    }

    sf::Clock clock;

    bool gameOver = false;
    bool gameOverLogged = false;

    float fadeTimer = 0.0f;
    float fadeDuration = 2.0f;

    sf::Color fadeStartColor(10, 30, 30);
    sf::Color fadeEndColor(255, 255, 255);

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
        }

        if (!gameOver) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::F1)) {
                log("game over triggered");
                lifes = 0;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::F7)) {
                log("poscurseur : " +
                    std::to_string(sf::Mouse::getPosition(window).x) + " " +
                    std::to_string(sf::Mouse::getPosition(window).y));
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                paddleX -= 10;
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

                log("lifes-1 = " + std::to_string(lifes));

                if (lifes <= 0) {
                    gameOver = true;
                }
            }

            ball.setPosition(ballX, ballY);
            paddle.setPosition(paddleX, paddleY);

            if (ball.getGlobalBounds().intersects(paddle.getGlobalBounds())) {
                ballSpeedY = -std::abs(ballSpeedY);
                ballY = paddleY - ball.getRadius() * 2;
            }

            for (auto& brick : bricks) {
                if (brick.alive && ball.getGlobalBounds().intersects(brick.shape.getGlobalBounds())) {
                    brick.alive = false;
                    ballSpeedY = -ballSpeedY;
                    break;
                }
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
            sf::Texture boxlost;
            if (!boxlost.loadFromFile("bglost.jpg")) {
                errmsg("Une erreur est survenue. Les fichiers sont probablement corormpus.");
                return 1;
            }
            box.setTexture(&boxlost, true);
            box.setFillColor(fadeColor);
            if (t >= 1.0f) {
                lifes = 3;
                ballX = 936;
                ballY = 930;

                ballSpeedX = 3.0f;
                ballSpeedY = -3.0f;

                paddleX = 936;
                paddleY = 965;

                // reset partie, donc reset briques (et oui)
                for (auto& brick : bricks) {
                    brick.alive = true;
                }

                box.setTexture(&boxTexture, true);

                fadeTimer = 0.0f;
                gameOver = false;
                gameOverLogged = false;

                log("Partie reset.");
            }
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

        window.display();
    }

    return 0;
}