#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Interactive-Physics-Sandbox");
    window.setFramerateLimit(60);
    sf::RectangleShape shape(sf::Vector2f(100.f, 100.f));
    //shape.setFillColor(sf::Color::Green);
    shape.setPosition(350.f, 50.f);
    sf::Vector2f velocity(0.f, 0.f);
    sf::Vector2f lastPos;
    const float gravity = 981.0f;
    const float bounceDamping = 0.7f;
    const float airResistance = 0.988f;
    const float dragCoefficient = 0.7f;

    bool isGrabbed = false;
    sf::Clock clock;
    sf::Texture helloKittyTexture;
    if (!helloKittyTexture.loadFromFile("hello-kitty.png")) {
        std::cerr << "Failed to load texture!" << std::endl;
        return -1;
    }
    shape.setTexture(&helloKittyTexture);
    shape.setOrigin(0.f, 0.f);

    auto resetSimulation = [&]() {
        shape.setPosition(350.f, 50.f);
        velocity = sf::Vector2f(0.f, 0.f);
        isGrabbed = false;
    };

    while (window.isOpen()) {
        sf::Event event;
        sf::Time elapsed = clock.restart();
        float dt = elapsed.asSeconds();

        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::R) {
                    resetSimulation();
                }
            }
            
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    if (shape.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
                        isGrabbed = true;
                }
            }
            if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    isGrabbed = false;
                }
            }
        }
        if (isGrabbed) {
            lastPos = shape.getPosition();
            shape.setPosition(static_cast<float>(mousePos.x) - shape.getSize().x / 2,
                              static_cast<float>(mousePos.y) - shape.getSize().y / 2);
            if (dt > 0) {
                velocity = (shape.getPosition() - lastPos) / dt;
            }
        } 
        else {
            velocity.y += gravity * dt;
            velocity *= airResistance;
            shape.move(velocity * dt);

            sf::FloatRect bounds = shape.getGlobalBounds();
            sf::Vector2 pos = shape.getPosition();

            if (bounds.top + bounds.height > window.getSize().y) {
                shape.setPosition(bounds.left, window.getSize().y - bounds.height);
                velocity.y = -velocity.y * bounceDamping;
            }
            if (bounds.top < 0) {
                shape.setPosition(bounds.left, 0);
                velocity.y = -velocity.y * bounceDamping;
            }
            if (bounds.left < 0) {
                shape.setPosition(0, bounds.top);
                velocity.x = -velocity.x * bounceDamping;
            }
            if (bounds.left + bounds.width > window.getSize().x) {
                shape.setPosition(window.getSize().x - bounds.width, bounds.top);
                velocity.x = -velocity.x * bounceDamping;
            }

            if (pos.y == bounds.top + bounds.height) {
                velocity.x *= dragCoefficient;
            }

            
        }
        window.clear(sf::Color::Black);
        window.draw(shape);
        window.display();
    }

    return 0;
}