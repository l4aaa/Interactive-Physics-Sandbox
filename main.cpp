#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>
#include "PhysicsEngine.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Interactive-Physics-Sandbox");
    window.setFramerateLimit(60);

    PhysicsEngine engine;
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

    std::vector<PhysicsBody> bodies;
    auto spawnObject = [&](float x, float y) {
        PhysicsBody obj;
        obj.shape.setSize(sf::Vector2f(100.f, 100.f));
        obj.shape.setOrigin(50.f, 50.f);
        obj.shape.setTexture(&helloKittyTexture);
        obj.shape.setPosition(x + 50.f, y + 50.f);
        obj.mass = 1.0f;
        obj.inertia = (1.f/12.f) * obj.mass * (100.f*100.f + 100.f*100.f);
        bodies.push_back(obj);
    };

    spawnObject(350.f, 50.f);
    sf::Clock clock;

    while (window.isOpen()) {
        sf::Time elapsed = clock.restart();
        float dt = elapsed.asSeconds();
        if (dt > 0.1f) dt = 0.1f; // Cap dt to prevent tunneling

        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::MouseWheelScrolled) {
                engine.params.bounceDamping += event.mouseWheelScroll.delta * 0.05f;
                if (engine.params.bounceDamping < 0.f) engine.params.bounceDamping = 0.f;
                if (engine.params.bounceDamping > 1.5f) engine.params.bounceDamping = 1.5f;
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::R) {
                    bodies.clear();
                    spawnObject(350.f, 50.f);
                }
                if (event.key.code == sf::Keyboard::L) showLegend = !showLegend;
                if (event.key.code == sf::Keyboard::V) showVelocity = !showVelocity;
                if (event.key.code == sf::Keyboard::D) engine.debugEnabled = !engine.debugEnabled;
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
                spawnObject(static_cast<float>(mousePos.x) - 50.f, static_cast<float>(mousePos.y) - 50.f);
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                for (auto& obj : bodies) {
                    if (isInsideOBB(static_cast<sf::Vector2f>(mousePos), obj.getVertices())) {
                        obj.isGrabbed = true;
                        obj.grabOffset = obj.shape.getPosition() - static_cast<sf::Vector2f>(mousePos);
                        obj.wakeUp();
                        break;
                    }
                }
            }

            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                for (auto& obj : bodies) obj.isGrabbed = false;
            }
        }

        // Mouse Drag Interaction
        for (auto& obj : bodies) {
            if (obj.isGrabbed) {
                obj.lastPos = obj.shape.getPosition();
                obj.shape.setPosition(static_cast<float>(mousePos.x) + obj.grabOffset.x,
                                      static_cast<float>(mousePos.y) + obj.grabOffset.y);
                if (dt > 0) {
                    obj.velocity = (obj.shape.getPosition() - obj.lastPos) / dt;
                }
            }
        }

        engine.update(bodies, dt, window.getSize());

        // UI Text
        std::stringstream ss;
        ss << "CONTROLS:\n"
           << "[Left Click]  Grab & Throw\n"
           << "[Right Click] Spawn Object\n"
           << "[Scroll]      Bounciness: " << std::fixed << std::setprecision(2) << engine.params.bounceDamping << "\n"
           << "[D]           Debug View: " << (engine.debugEnabled ? "ON" : "OFF") << "\n"
           << "[L]           Toggle Legend\n"
           << "[V]           Toggle Velocity\n"
           << "[R]           Reset Simulation";
        legend.setString(ss.str());

        window.clear(sf::Color::Black);

        // Debug Grid
        if (engine.debugEnabled) {
            for (int x = 0; x < 800; x += 100) {
                sf::Vertex line[] = { sf::Vertex(sf::Vector2f(x, 0), sf::Color(50, 50, 50)), sf::Vertex(sf::Vector2f(x, 600), sf::Color(50, 50, 50)) };
                window.draw(line, 2, sf::Lines);
            }
            for (int y = 0; y < 600; y += 100) {
                sf::Vertex line[] = { sf::Vertex(sf::Vector2f(0, y), sf::Color(50, 50, 50)), sf::Vertex(sf::Vector2f(800, y), sf::Color(50, 50, 50)) };
                window.draw(line, 2, sf::Lines);
            }
        }
        
        for (auto& obj : bodies) {
            if (obj.isSleeping) obj.shape.setFillColor(sf::Color(100, 100, 100, 200));
            else obj.shape.setFillColor(sf::Color::White);
            
            window.draw(obj.shape);
            
            float currentSpeed = std::sqrt(lengthSq(obj.velocity));
            obj.smoothedVelocity = obj.smoothedVelocity + (currentSpeed - obj.smoothedVelocity) * 0.1f;
            
            if (showVelocity) {
                velocityText.setString("Vel: " + std::to_string(static_cast<int>(obj.smoothedVelocity)));
                velocityText.setPosition(obj.shape.getPosition().x - 40.f, obj.shape.getPosition().y - 70.f);
                window.draw(velocityText);
            }

            if (engine.debugEnabled) {
                sf::FloatRect bounds = obj.shape.getGlobalBounds();
                sf::RectangleShape aabb;
                aabb.setPosition(bounds.left, bounds.top);
                aabb.setSize(sf::Vector2f(bounds.width, bounds.height));
                aabb.setFillColor(sf::Color::Transparent);
                aabb.setOutlineColor(sf::Color::Green);
                aabb.setOutlineThickness(1.f);
                window.draw(aabb);
            }
        }

        // Debug Contacts
        if (engine.debugEnabled) {
            for (const auto& contact : engine.contacts) {
                sf::CircleShape dot(3.f);
                dot.setOrigin(3.f, 3.f);
                dot.setPosition(contact.position);
                dot.setFillColor(sf::Color::Red);
                window.draw(dot);

                sf::Vertex line[] = {
                    sf::Vertex(contact.position, sf::Color::Blue),
                    sf::Vertex(contact.position + contact.normal * 20.f, sf::Color::Blue)
                };
                window.draw(line, 2, sf::Lines);
            }
        }

        if (showLegend) window.draw(legend);
        window.display();
    }
    return 0;
}
