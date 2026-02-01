#include <SFML/Graphics.hpp>

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Interactive-Physics-Sandbox");
    window.setFramerateLimit(60);
    sf::RectangleShape shape(sf::Vector2f(50.f, 50.f));
    shape.setFillColor(sf::Color::Green);
    shape.setPosition(350.f, 50.f);
    sf::Vector2f velocity(0.f, 0.f);
    const float gravity = 981.0f;
    const float floorY = 550.f;
    sf::Clock clock;


    while (window.isOpen()) {
        sf::Event event;
        sf::Time elapsed = clock.restart();
        float dt = elapsed.asSeconds();
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::Black);
        velocity.y += gravity * dt;
        shape.move(velocity * dt);
        if (shape.getPosition().y > floorY) {
            velocity.y = 0.f;
            shape.setPosition(shape.getPosition().x, floorY);
        }
        window.draw(shape);
        window.display();
    }

    return 0;
}