#ifndef PHYSICS_BODY_HPP
#define PHYSICS_BODY_HPP

#include "MathUtils.hpp"

struct PhysicsBody {
    sf::RectangleShape shape;
    sf::Vector2f velocity = {0.f, 0.f};
    float angularVelocity = 0.f; // Radians / sec
    sf::Vector2f lastPos;
    float smoothedVelocity = 0.f;
    bool isGrabbed = false;
    sf::Vector2f grabOffset;
    float mass = 1.0f;
    float inertia = 1.0f;

    bool isSleeping = false;
    float sleepTimer = 0.f;

    std::vector<sf::Vector2f> getVertices() const {
        std::vector<sf::Vector2f> vertices(4);
        sf::Vector2f size = shape.getSize();
        sf::Vector2f halfSize = size / 2.f;
        float angle = shape.getRotation();
        vertices[0] = rotate({-halfSize.x, -halfSize.y}, angle) + shape.getPosition();
        vertices[1] = rotate({halfSize.x, -halfSize.y}, angle) + shape.getPosition();
        vertices[2] = rotate({halfSize.x, halfSize.y}, angle) + shape.getPosition();
        vertices[3] = rotate({-halfSize.x, halfSize.y}, angle) + shape.getPosition();
        return vertices;
    }

    void wakeUp() {
        isSleeping = false;
        sleepTimer = 0.f;
    }
};

inline bool isInsideOBB(const sf::Vector2f& p, const std::vector<sf::Vector2f>& v) {
    for (int i : {0, 3}) {
        sf::Vector2f axis = normalize(v[(i == 0) ? 1 : 0] - v[i]);
        float proj = dot(p, axis);
        float minP = 1e9f, maxP = -1e9f;
        for(const auto& bp : v) { float pr = dot(bp, axis); minP = std::min(minP, pr); maxP = std::max(maxP, pr); }
        if (proj < minP - 0.1f || proj > maxP + 0.1f) return false;
    }
    return true;
}

#endif
