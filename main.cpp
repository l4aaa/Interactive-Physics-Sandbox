#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <iomanip>
#include <sstream>

struct PhysicsObject {
    sf::RectangleShape shape;
    sf::Vector2f velocity = {0.f, 0.f};
    sf::Vector2f lastPos;
    float smoothedVelocity = 0.f;
    bool isGrabbed = false;
};

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Interactive-Physics-Sandbox");
    window.setFramerateLimit(60);

    const float gravity = 981.0f;
    float bounceDamping = 0.8f;
    const float airResistance = 0.988f;
    const float dragCoefficient = 0.7f;

    bool showLegend = true;
    bool showVelocity = true;

    sf::Texture helloKittyTexture;
    if (!helloKittyTexture.loadFromFile("hello-kitty.png")) {
        std::cerr << "Failed to load texture!" << std::endl;
        return -1;
    }

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
    }

    sf::Text legend;
    legend.setFont(font);
    legend.setCharacterSize(16);
    legend.setFillColor(sf::Color::Cyan);
    legend.setPosition(10.f, 10.f);

    sf::Text velocityText;
    velocityText.setFont(font);
    velocityText.setCharacterSize(14);
    velocityText.setFillColor(sf::Color::White);

    std::vector<PhysicsObject> objects;
    auto spawnObject = [&](float x, float y) {
        PhysicsObject obj;
        obj.shape.setSize(sf::Vector2f(100.f, 100.f));
        obj.shape.setTexture(&helloKittyTexture);
        obj.shape.setPosition(x, y);
        obj.velocity = sf::Vector2f(0.f, 0.f);
        objects.push_back(obj);
    };

    spawnObject(350.f, 50.f);
    sf::Clock clock;

    while (window.isOpen()) {
        sf::Time elapsed = clock.restart();
        float dt = elapsed.asSeconds();

        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::MouseWheelScrolled) {
                bounceDamping += event.mouseWheelScroll.delta * 0.05f;
                if (bounceDamping < 0.f) bounceDamping = 0.f;
                if (bounceDamping > 1.5f) bounceDamping = 1.5f;
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::R) {
                    objects.clear();
                    spawnObject(350.f, 50.f);
                }
                if (event.key.code == sf::Keyboard::L) showLegend = !showLegend;
                if (event.key.code == sf::Keyboard::V) showVelocity = !showVelocity;
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
                spawnObject(static_cast<float>(mousePos.x) - 50.f, static_cast<float>(mousePos.y) - 50.f);
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                for (auto& obj : objects) {
                    if (obj.shape.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                        obj.isGrabbed = true;
                        break;
                    }
                }
            }

            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                for (auto& obj : objects) obj.isGrabbed = false;
            }
        }

        std::stringstream ss;
        ss << "CONTROLS:\n"
           << "[Left Click]  Grab & Throw\n"
           << "[Right Click] Spawn Object\n"
           << "[Scroll]      Bounciness: " << std::fixed << std::setprecision(2) << bounceDamping << "\n"
           << "[L]           Toggle Legend\n"
           << "[V]           Toggle Velocity\n"
           << "[R]           Reset Simulation";
        legend.setString(ss.str());

        for (auto& obj : objects) {
            if (obj.isGrabbed) {
                obj.lastPos = obj.shape.getPosition();
                obj.shape.setPosition(static_cast<float>(mousePos.x) - obj.shape.getSize().x / 2,
                                      static_cast<float>(mousePos.y) - obj.shape.getSize().y / 2);
                if (dt > 0) {
                    obj.velocity = (obj.shape.getPosition() - obj.lastPos) / dt;
                }
            } else {
                obj.velocity.y += gravity * dt;
                obj.velocity *= airResistance;
                obj.shape.move(obj.velocity * dt);

                sf::FloatRect bounds = obj.shape.getGlobalBounds();
                if (bounds.top + bounds.height > window.getSize().y) {
                    obj.shape.setPosition(bounds.left, window.getSize().y - bounds.height);
                    obj.velocity.y = -obj.velocity.y * bounceDamping;
                    obj.velocity.x *= dragCoefficient;
                }
                if (bounds.top < 0) {
                    obj.shape.setPosition(bounds.left, 0);
                    obj.velocity.y = -obj.velocity.y * bounceDamping;
                }
                if (bounds.left < 0) {
                    obj.shape.setPosition(0, bounds.top);
                    obj.velocity.x = -obj.velocity.x * bounceDamping;
                }
                if (bounds.left + bounds.width > window.getSize().x) {
                    obj.shape.setPosition(window.getSize().x - bounds.width, bounds.top);
                    obj.velocity.x = -obj.velocity.x * bounceDamping;
                }
            }
        }

        for (size_t i = 0; i < objects.size(); ++i) {
            for (size_t j = i + 1; j < objects.size(); ++j) {
                sf::FloatRect overlap;
                if (objects[i].shape.getGlobalBounds().intersects(objects[j].shape.getGlobalBounds(), overlap)) {
                    if (overlap.width < overlap.height) {
                        float push = overlap.width / 2.0f;
                        float dir = (objects[i].shape.getPosition().x < objects[j].shape.getPosition().x) ? -1.f : 1.f;
                        objects[i].shape.move(push * dir, 0);
                        objects[j].shape.move(push * -dir, 0);
                        std::swap(objects[i].velocity.x, objects[j].velocity.x);
                        objects[i].velocity.x *= bounceDamping;
                        objects[j].velocity.x *= bounceDamping;
                    } else {
                        float push = overlap.height / 2.0f;
                        float dir = (objects[i].shape.getPosition().y < objects[j].shape.getPosition().y) ? -1.f : 1.f;
                        objects[i].shape.move(0, push * dir);
                        objects[j].shape.move(0, push * -dir);
                        std::swap(objects[i].velocity.y, objects[j].velocity.y);
                        objects[i].velocity.y *= bounceDamping;
                        objects[j].velocity.y *= bounceDamping;
                    }
                }
            }
        }

        window.clear(sf::Color::Black);
        
        for (auto& obj : objects) {
            window.draw(obj.shape);
            float currentSpeed = std::sqrt(obj.velocity.x * obj.velocity.x + obj.velocity.y * obj.velocity.y);
            obj.smoothedVelocity = obj.smoothedVelocity + (currentSpeed - obj.smoothedVelocity) * 0.1f;
            
            if (showVelocity) {
                velocityText.setString("Vel: " + std::to_string(static_cast<int>(obj.smoothedVelocity)));
                velocityText.setPosition(obj.shape.getPosition().x, obj.shape.getPosition().y - 20.f);
                window.draw(velocityText);
            }
        }

        if (showLegend) window.draw(legend);
        window.display();
    }
    return 0;
}