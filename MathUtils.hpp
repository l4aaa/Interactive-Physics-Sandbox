#ifndef MATH_UTILS_HPP
#define MATH_UTILS_HPP

#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>

const float PI = 3.14159265f;

inline float dot(const sf::Vector2f& a, const sf::Vector2f& b) { return a.x * b.x + a.y * b.y; }
inline float cross(const sf::Vector2f& a, const sf::Vector2f& b) { return a.x * b.y - a.y * b.x; }
inline float lengthSq(const sf::Vector2f& v) { return v.x * v.x + v.y * v.y; }
inline sf::Vector2f normalize(const sf::Vector2f& v) {
    float len = std::sqrt(lengthSq(v));
    return (len > 0) ? v / len : sf::Vector2f(0, 0);
}
inline sf::Vector2f rotate(const sf::Vector2f& v, float angleDeg) {
    float rad = angleDeg * PI / 180.f;
    return { v.x * std::cos(rad) - v.y * std::sin(rad), v.x * std::sin(rad) + v.y * std::cos(rad) };
}

#endif
