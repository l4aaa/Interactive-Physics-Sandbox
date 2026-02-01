#include <SFML/Graphics.hpp>

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Interactive-Physics-Sandbox");
    window.setFramerateLimit(60);
    sf::RectangleShape shape(sf::Vector2f(50.f, 50.f));
    shape.setFillColor(sf::Color::Green);
    shape.setPosition(350.f, 250.f);
    sf::Vector2f velocity(0.f, 0.f);
    const float gravity = 0.5f;


    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::Black);
        velocity.y += gravity;
        shape.move(velocity);
        window.clear(sf::Color::Black);
        window.draw(shape);
        window.display();
    }

    return 0;
}