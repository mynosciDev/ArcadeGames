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

int main() {
    SetDllDirectoryA((getExeDirectory() + "\\lib").c_str());
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

    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 12; col++) {
            Brick brick;
            brick.shape.setSize(sf::Vector2f(120, 35));
            brick.shape.setFillColor(sf::Color::Red);
            brick.shape.setPosition(200 + col * 130, 100 + row * 45);
            bricks.push_back(brick);
        }
    }

    while (window.isOpen()) {
        sf::Event event{};

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
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
        window.clear();
        window.draw(box);
        for (const auto& brick : bricks) {
            if (brick.alive) {
                window.draw(brick.shape);
            }
        }
        for (auto& brick : bricks) {
            if (!brick.alive) {

            }
        }
        window.draw(paddle);
        window.draw(ball);
        window.display();
    }

    return 0;
}