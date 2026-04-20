#pragma once

class GameObject {
public:
    virtual ~GameObject();

    GameObject(const GameObject&) = delete;
    GameObject& operator=(const GameObject&) = delete;
    GameObject(GameObject&&) = delete;
    GameObject& operator=(GameObject&&) = delete;

    [[nodiscard]] float getPositionX() const;
    [[nodiscard]] float getPositionY() const;
    void setPositionX(float positionX);
    void setPositionY(float positionY);

    [[nodiscard]] float getWidth() const;
    [[nodiscard]] float getHeight() const;
    void setSize(float width, float height);

    virtual void updateModel(float deltaSeconds) = 0;

protected:
    GameObject(float positionX, float positionY, float width, float height);

private:
    float m_positionX;
    float m_positionY;
    float m_width;
    float m_height;
};
