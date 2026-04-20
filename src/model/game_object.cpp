#include "model/game_object.h"

GameObject::~GameObject() = default;

GameObject::GameObject(float positionX, float positionY, float width, float height)
    : m_positionX(positionX), m_positionY(positionY), m_width(width), m_height(height) {}

float GameObject::getPositionX() const {
    return m_positionX;
}

float GameObject::getPositionY() const {
    return m_positionY;
}

void GameObject::setPositionX(float positionX) {
    m_positionX = positionX;
}

void GameObject::setPositionY(float positionY) {
    m_positionY = positionY;
}

float GameObject::getWidth() const {
    return m_width;
}

float GameObject::getHeight() const {
    return m_height;
}

void GameObject::setSize(float width, float height) {
    m_width = width;
    m_height = height;
}
