#ifndef PHYSICS_ENGINE_HPP
#define PHYSICS_ENGINE_HPP

#include "PhysicsBody.hpp"
#include <map>
#include <set>

struct ContactPoint {
    sf::Vector2f position;
    sf::Vector2f normal;
};

struct CollisionInfo {
    bool collided = false;
    sf::Vector2f normal;
    float depth = 0.f;
    sf::Vector2f contact;
};

class PhysicsEngine {
public:
    struct Params {
        float gravity = 981.0f;
        float bounceDamping = 0.8f;
        float airResistance = 0.988f;
        float dragCoefficient = 0.7f;
    };

    Params params;
    std::vector<ContactPoint> contacts;
    bool debugEnabled = false;

    void update(std::vector<PhysicsBody>& bodies, float dt, sf::Vector2u windowSize) {
        contacts.clear();

        // 1. Integration & Boundaries
        for (auto& obj : bodies) {
            if (obj.isGrabbed) {
                obj.lastPos = obj.shape.getPosition();
                // Handled in main loop for mouse pos, but let's ensure it stays awake
                obj.wakeUp();
            } else if (!obj.isSleeping) {
                obj.velocity.y += params.gravity * dt;
                obj.velocity *= params.airResistance;
                obj.angularVelocity *= params.airResistance;
                obj.shape.move(obj.velocity * dt);
                obj.shape.rotate(obj.angularVelocity * 180.f / PI * dt);

                // Sleeping check
                float speedSq = lengthSq(obj.velocity);
                float angSpeedSq = obj.angularVelocity * obj.angularVelocity;
                if (speedSq < 100.f && angSpeedSq < 0.1f) {
                    obj.sleepTimer += dt;
                    if (obj.sleepTimer > 0.5f) obj.isSleeping = true;
                } else {
                    obj.sleepTimer = 0.f;
                }

                // Boundaries
                sf::FloatRect bounds = obj.shape.getGlobalBounds();
                if (bounds.top + bounds.height > windowSize.y) {
                    obj.shape.move(0, windowSize.y - (bounds.top + bounds.height));
                    obj.velocity.y = -obj.velocity.y * params.bounceDamping;
                    obj.velocity.x *= params.dragCoefficient;
                    obj.angularVelocity *= params.dragCoefficient;
                    if (std::abs(obj.velocity.y) > 10.f) obj.wakeUp();
                }
                if (bounds.top < 0) {
                    obj.shape.move(0, -bounds.top);
                    obj.velocity.y = -obj.velocity.y * params.bounceDamping;
                    obj.wakeUp();
                }
                if (bounds.left < 0) {
                    obj.shape.move(-bounds.left, 0);
                    obj.velocity.x = -obj.velocity.x * params.bounceDamping;
                    obj.wakeUp();
                }
                if (bounds.left + bounds.width > windowSize.x) {
                    obj.shape.move(windowSize.x - (bounds.left + bounds.width), 0);
                    obj.velocity.x = -obj.velocity.x * params.bounceDamping;
                    obj.wakeUp();
                }
            }
        }

        // 2. Broad-phase (Grid)
        const float cellSize = 100.f;
        std::map<int, std::vector<size_t>> grid;

        for (size_t i = 0; i < bodies.size(); ++i) {
            sf::FloatRect bounds = bodies[i].shape.getGlobalBounds();
            int minX = static_cast<int>(std::floor(bounds.left / cellSize));
            int maxX = static_cast<int>(std::floor((bounds.left + bounds.width) / cellSize));
            int minY = static_cast<int>(std::floor(bounds.top / cellSize));
            int maxY = static_cast<int>(std::floor((bounds.top + bounds.height) / cellSize));

            for (int x = minX; x <= maxX; ++x) {
                for (int y = minY; y <= maxY; ++y) {
                    grid[x + y * 1000].push_back(i);
                }
            }
        }

        // 3. Narrow-phase (SAT)
        std::set<std::pair<size_t, size_t>> tested;
        for (auto const& [key, cell] : grid) {
            for (size_t i = 0; i < cell.size(); ++i) {
                for (size_t j = i + 1; j < cell.size(); ++j) {
                    size_t a = cell[i];
                    size_t b = cell[j];
                    if (a > b) std::swap(a, b);
                    if (tested.count({a, b})) continue;
                    tested.insert({a, b});

                    if (bodies[a].isSleeping && bodies[b].isSleeping) continue;

                    CollisionInfo info = checkOBB(bodies[a], bodies[b]);
                    if (info.collided) {
                        resolveCollision(bodies[a], bodies[b], info);
                        if (debugEnabled) {
                            contacts.push_back({info.contact, info.normal});
                        }
                    }
                }
            }
        }
    }

private:
    CollisionInfo checkOBB(const PhysicsBody& objA, const PhysicsBody& objB) {
        CollisionInfo info;
        std::vector<sf::Vector2f> vA = objA.getVertices();
        std::vector<sf::Vector2f> vB = objB.getVertices();

        std::vector<sf::Vector2f> axes;
        axes.push_back(normalize(vA[1] - vA[0]));
        axes.push_back(normalize(vA[3] - vA[0]));
        axes.push_back(normalize(vB[1] - vB[0]));
        axes.push_back(normalize(vB[3] - vB[0]));

        float minOverlap = 1e9f;

        for (const auto& axis : axes) {
            float minA = 1e9f, maxA = -1e9f;
            for (const auto& p : vA) {
                float proj = dot(p, axis);
                minA = std::min(minA, proj);
                maxA = std::max(maxA, proj);
            }
            float minB = 1e9f, maxB = -1e9f;
            for (const auto& p : vB) {
                float proj = dot(p, axis);
                minB = std::min(minB, proj);
                maxB = std::max(maxB, proj);
            }

            if (maxA < minB || maxB < minA) {
                info.collided = false;
                return info;
            }

            float overlap = std::min(maxA, maxB) - std::max(minA, minB);
            if (overlap < minOverlap) {
                minOverlap = overlap;
                info.normal = axis;
            }
        }

        info.collided = true;
        info.depth = minOverlap;
        if (dot(objB.shape.getPosition() - objA.shape.getPosition(), info.normal) < 0) {
            info.normal = -info.normal;
        }

        sf::Vector2f contact(0,0);
        int count = 0;
        for(const auto& v : vA) if (isInsideOBB(v, vB)) { contact += v; count++; }
        for(const auto& v : vB) if (isInsideOBB(v, vA)) { contact += v; count++; }
        
        if (count > 0) info.contact = contact / (float)count;
        else info.contact = (objA.shape.getPosition() + objB.shape.getPosition()) / 2.f;

        return info;
    }

    void resolveCollision(PhysicsBody& a, PhysicsBody& b, const CollisionInfo& info) {
        float push = info.depth / 2.0f;
        a.shape.move(-info.normal * push);
        b.shape.move(info.normal * push);

        sf::Vector2f rA = info.contact - a.shape.getPosition();
        sf::Vector2f rB = info.contact - b.shape.getPosition();

        sf::Vector2f vA = a.velocity + sf::Vector2f(-a.angularVelocity * rA.y, a.angularVelocity * rA.x);
        sf::Vector2f vB = b.velocity + sf::Vector2f(-b.angularVelocity * rB.y, b.angularVelocity * rB.x);

        sf::Vector2f vRel = vB - vA;
        float vNormal = dot(vRel, info.normal);
        if (vNormal > 0) return;

        float invMassSum = 1.0f / a.mass + 1.0f / b.mass;
        float rACrossN = cross(rA, info.normal);
        float rBCrossN = cross(rB, info.normal);
        float invInertiaSum = (rACrossN * rACrossN) / a.inertia + (rBCrossN * rBCrossN) / b.inertia;

        float j_impulse = -(1.0f + params.bounceDamping) * vNormal / (invMassSum + invInertiaSum);

        sf::Vector2f impulse = j_impulse * info.normal;
        a.velocity -= impulse / a.mass;
        a.angularVelocity -= cross(rA, impulse) / a.inertia;
        b.velocity += impulse / b.mass;
        b.angularVelocity += cross(rB, impulse) / b.inertia;

        a.wakeUp();
        b.wakeUp();
    }
};

#endif
