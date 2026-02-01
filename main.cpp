#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

struct PhysicsObject {
    sf::RectangleShape shape;
    sf::Vector2f velocity = {0.f, 0.f};
    sf::Vector2f lastPos;
    bool isGrabbed = false;
};

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Interactive-Physics-Sandbox");
    window.setFramerateLimit(60);



    const float gravity = 981.0f;
    const float bounceDamping = 0.98f;
    const float airResistance = 0.988f;
    const float dragCoefficient = 0.7f;

    sf::Texture helloKittyTexture;
    if (!helloKittyTexture.loadFromFile("hello-kitty.png")) {
        std::cerr << "Failed to load texture!" << std::endl;
        return -1;
    }

    std::vector<PhysicsObject> objects;
    PhysicsObject startObj;
    startObj.shape.setSize(sf::Vector2f(100.f, 100.f));
    startObj.shape.setTexture(&helloKittyTexture);
    startObj.shape.setPosition(350.f, 50.f);
    startObj.velocity = sf::Vector2f(0.f, 0.f);
    objects.push_back(startObj);

    sf::Clock clock;

    while (window.isOpen()) {
        sf::Time elapsed = clock.restart();
        float dt = elapsed.asSeconds();
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
                PhysicsObject newObj;
                newObj.shape.setSize(sf::Vector2f(100.f, 100.f));
                newObj.shape.setTexture(&helloKittyTexture);
                newObj.shape.setPosition(
                    static_cast<float>(mousePos.x) - 50.f, 
                    static_cast<float>(mousePos.y) - 50.f
                );
                objects.push_back(newObj);
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
                objects.clear();
                PhysicsObject startObj;
                startObj.shape.setSize(sf::Vector2f(100.f, 100.f));
                startObj.shape.setTexture(&helloKittyTexture);
                startObj.shape.setPosition(350.f, 50.f);
                startObj.velocity = sf::Vector2f(0.f, 0.f);
                objects.push_back(startObj);
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
        for (const auto& obj : objects) window.draw(obj.shape);
        window.display();
    }
    return 0;
}