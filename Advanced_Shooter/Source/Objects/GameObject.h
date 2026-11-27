/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : GameObject.h
Description : Base renderable game object
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/

#pragma once

#include <SFML/Graphics.hpp>
#include <optional>

// base position hitbox and sprite
class GameObject
{
public:
    GameObject();
    virtual ~GameObject() = default;

    virtual void Render(sf::RenderWindow& window);

    sf::FloatRect GetBounds() const;

    void SetPosition(const sf::Vector2f& vPosition);
    sf::Vector2f GetPosition() const;

    void SetSize(const sf::Vector2f& vSize);
    sf::Vector2f GetSize() const;

    void MoveBy(const sf::Vector2f& vDelta);

    void SetActive(bool bIsActive);
    bool IsActive() const;

    void SetTexture(const sf::Texture* pTexture);
    bool HasTexture() const;

protected:
    sf::RectangleShape m_shape;
    std::optional<sf::Sprite> m_sprite;
    sf::Vector2f m_vPosition;
    bool m_bIsActive;
};
