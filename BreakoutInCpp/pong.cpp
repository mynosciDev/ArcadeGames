#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>
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

    sf::RenderWindow window(sf::VideoMode(800, 600), "Pong");
    window.setFramerateLimit(120);

    // Balle
    sf::CircleShape ball(10.0f);
    ball.setFillColor(sf::Color::White);

    float ballX = 400.0f - ball.getRadius();
    float ballY = 300.0f - ball.getRadius();
    float speedX = 0.30f;
    float speedY = 0.24f;

    // Scores
    int scoreLeft = 0;
    int scoreRight = 0;

    // Fond
    sf::RectangleShape box(sf::Vector2f(800.0f, 600.0f));
    sf::Texture boxTexture;
    if (!boxTexture.loadFromFile("res/background.png")) {
        errmsg("Impossible de charger res/background.png");
        return 1;
    }
    box.setTexture(&boxTexture, true);

    // Raquette gauche
    sf::RectangleShape paddleLeft(sf::Vector2f(10.0f, 100.0f));
    paddleLeft.setFillColor(sf::Color::Black);
    float paddleLeftX = 20.0f;
    float paddleLeftY = 250.0f;

    // Raquette droite
    sf::RectangleShape paddleRight(sf::Vector2f(10.0f, 100.0f));
    paddleRight.setFillColor(sf::Color::Black);
    float paddleRightX = 770.0f;
    float paddleRightY = 250.0f;

    // Police
    sf::Font font;
    if (!font.loadFromFile("res/arial.ttf")) {
        errmsg("Impossible de charger res/arial.ttf");
        return 1;
    }

    sf::Text textLeft("Score Gauche : 0", font, 20);
    textLeft.setFillColor(sf::Color::Black);
    textLeft.setStyle(sf::Text::Bold);
    textLeft.setPosition(60.0f, 20.0f);

    sf::Text textRight("Score Droit : 0", font, 20);
    textRight.setFillColor(sf::Color::Black);
    textRight.setStyle(sf::Text::Bold);
    textRight.setPosition(520.0f, 20.0f);

    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        // Déplacement raquette gauche
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            paddleLeftY -= 0.45f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            paddleLeftY += 0.45f;
        }

        // Déplacement raquette droite
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            paddleRightY -= 0.45f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            paddleRightY += 0.45f;
        }

        // Empêcher les raquettes de sortir de l'écran
        if (paddleLeftY < 0.0f) {
            paddleLeftY = 0.0f;
        }
        if (paddleLeftY + paddleLeft.getSize().y > box.getSize().y) {
            paddleLeftY = box.getSize().y - paddleLeft.getSize().y;
        }

        if (paddleRightY < 0.0f) {
            paddleRightY = 0.0f;
        }
        if (paddleRightY + paddleRight.getSize().y > box.getSize().y) {
            paddleRightY = box.getSize().y - paddleRight.getSize().y;
        }

        // Déplacement de la balle
        ballX += speedX;
        ballY += speedY;

        float ballDiameter = ball.getRadius() * 2.0f;

        // Rebond haut / bas
        if (ballY <= 0.0f) {
            ballY = 0.0f;
            speedY = -speedY;
        }
        if (ballY + ballDiameter >= box.getSize().y) {
            ballY = box.getSize().y - ballDiameter;
            speedY = -speedY;
        }

        // Collision avec la raquette gauche
        if (ballX + ballDiameter >= paddleLeftX &&
            ballX <= paddleLeftX + paddleLeft.getSize().x &&
            ballY + ballDiameter >= paddleLeftY &&
            ballY <= paddleLeftY + paddleLeft.getSize().y) {

            ballX = paddleLeftX + paddleLeft.getSize().x;
            speedX = std::abs(speedX);

            // Effet selon où la balle touche la raquette
            float paddleCenter = paddleLeftY + paddleLeft.getSize().y / 2.0f;
            float ballCenter = ballY + ball.getRadius();
            float offset = (ballCenter - paddleCenter) / (paddleLeft.getSize().y / 2.0f);
            speedY = offset * 0.35f;
        }

        // Collision avec la raquette droite
        if (ballX <= paddleRightX + paddleRight.getSize().x &&
            ballX + ballDiameter >= paddleRightX &&
            ballY + ballDiameter >= paddleRightY &&
            ballY <= paddleRightY + paddleRight.getSize().y) {

            ballX = paddleRightX - ballDiameter;
            speedX = -std::abs(speedX);

            // Effet selon où la balle touche la raquette
            float paddleCenter = paddleRightY + paddleRight.getSize().y / 2.0f;
            float ballCenter = ballY + ball.getRadius();
            float offset = (ballCenter - paddleCenter) / (paddleRight.getSize().y / 2.0f);
            speedY = offset * 0.35f;
        }

        // But à gauche -> point pour la droite
        if (ballX <= 0.0f) {
            scoreRight++;
            log("But pour droit");
            resetBall(ballX, ballY, speedX, speedY, ball.getRadius(), false);
        }

        // But à droite -> point pour la gauche
        if (ballX + ballDiameter >= box.getSize().x) {
            scoreLeft++;
            log("But pour gauche");
            resetBall(ballX, ballY, speedX, speedY, ball.getRadius(), true);
        }

        // Debug position curseur
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::F7)) {
            log("poscurseur : " +
                std::to_string(sf::Mouse::getPosition(window).x) + " " +
                std::to_string(sf::Mouse::getPosition(window).y));
        }

        // Mise à jour des textes
        textLeft.setString("Score Gauche : " + std::to_string(scoreLeft));
        textRight.setString("Score Droit : " + std::to_string(scoreRight));

        // Mise à jour des positions SFML
        ball.setPosition(ballX, ballY);
        paddleLeft.setPosition(paddleLeftX, paddleLeftY);
        paddleRight.setPosition(paddleRightX, paddleRightY);

        // Rendu
        window.clear();
        window.draw(box);
        window.draw(paddleLeft);
        window.draw(paddleRight);
        window.draw(ball);
        window.draw(textLeft);
        window.draw(textRight);
        window.display();
    }

    msg(
        "Merci d'avoir joue a ce jeu de Pong en C++ !\n\n"
        "Developpe par mynosci\n\n"
        "Score final : " + std::to_string(scoreLeft) + " - " + std::to_string(scoreRight)
    );

    return 0;
}