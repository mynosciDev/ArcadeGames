#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <windows.h>

/**
 * Breakout in C++
 */


void log(const char* str) {
    std::cout << str << std::endl;
}

void log(const std::string& str) {
    std::cout << str << std::endl;
}

void errmsg(const char* str) {
    std::cerr << str << std::endl;
    MessageBoxA(
        NULL,
        str,
        "Erreur",
        MB_OK | MB_ICONERROR
    );
}

void msg(const std::string& str) {
    MessageBoxA(
        NULL,
        str.c_str(),
        "Pong de wish",
        MB_OK | MB_ICONINFORMATION
    );
}


// getExeDirectory() - taken on github
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
    sf::CircleShape ball(10.0f);
    sf::RectangleShape box(sf::Vector2f(1920, 1080));
    sf::RectangleShape paddle(sf::Vector2f(100, 15));
    sf::Texture boxTexture;
    float ballX = 936;
    float ballY = 930;
    float ballSpeedX = 0.3f;
    float ballSpeedY = 0.3f;
    paddle.setFillColor(sf::Color::White);
    ball.setFillColor(sf::Color::White);
    float paddleX = 936;
    float paddleY = 965;
    if (!boxTexture.loadFromFile("background.png")) {
        errmsg("Impossible de charger background.png");
        return 1;
    }
    box.setTexture(&boxTexture, true);
    window.setFramerateLimit(120);
    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                }
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::F7)) {
            log("poscurseur : " +
                std::to_string(sf::Mouse::getPosition(window).x) + " " +
                std::to_string(sf::Mouse::getPosition(window).y));
        }
        ballX += ballSpeedX;
        ballY += ballSpeedY;
        if (ballY <= 0.0f) {
            ballY = 0.0f;
            ballSpeedY = -ballSpeedY;
        }
        if (ballY + ball.getRadius() >= box.getSize().y) {
            ballY = box.getSize().y - ball.getRadius();
            ballSpeedY = -ballSpeedY;
        }
        if (ballX + ball.getRadius() >= box.getSize().x) {
            ballX = box.getSize().x - ball.getRadius();
            ballSpeedX = -ballSpeedX;
        }
        if (ballX <= 0.0f) {
            ballX = 0.0f;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            paddleX -= 10;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            paddleX += 10;
        }
        if (paddleY < 0.0f) {
            paddleY = 0.0f;
        }
        if (paddleY + paddle.getSize().y > box.getSize().y) {
            paddleY = box.getSize().y - paddle.getSize().y;
        }
        if (paddleX < 0.0f) {
            paddleX = 0.0f;
        }
        if (paddleX + paddle.getSize().x > box.getSize().x) {
            paddleX = box.getSize().x - paddle.getSize().x;
        }
        ball.setPosition(ballX, ballY);
        paddle.setPosition(paddleX, paddleY);
        window.clear();
        window.draw(box);
        window.draw(paddle);
        window.draw(ball);
        window.display();
    }
    return 0;


}