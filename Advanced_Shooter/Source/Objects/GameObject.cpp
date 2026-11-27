/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : GameObject.cpp
Description : Base renderable game object
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/
#include "Objects/GameObject.h"

GameObject::GameObject()
    : m_vPosition(0.f, 0.f)
    , m_bIsActive(true)
{
    // default one tile size
    m_shape.setSize(sf::Vector2f(32.f, 32.f));
}

void GameObject::Render(sf::RenderWindow& window)
{
    if (m_bIsActive)
    {
        if (m_sprite.has_value())
        {
            window.draw(*m_sprite);
        }
    }
}

sf::FloatRect GameObject::GetBounds() const
{
    return m_shape.getGlobalBounds();
}

void GameObject::SetPosition(const sf::Vector2f& vPosition)
{
    m_vPosition = vPosition;
    m_shape.setPosition(vPosition);

    if (m_sprite.has_value())
    {
        m_sprite->setPosition(vPosition);
    }
}

sf::Vector2f GameObject::GetPosition() const
{
    return m_vPosition;
}

void GameObject::SetSize(const sf::Vector2f& vSize)
{
    m_shape.setSize(vSize);
}

sf::Vector2f GameObject::GetSize() const
{
    return m_shape.getSize();
}

void GameObject::MoveBy(const sf::Vector2f& vDelta)
{
    SetPosition(m_vPosition + vDelta);
}

void GameObject::SetActive(bool bIsActive)
{
    m_bIsActive = bIsActive;
}

bool GameObject::IsActive() const
{
    return m_bIsActive;
}

void GameObject::SetTexture(const sf::Texture* pTexture)
{
    if (pTexture == nullptr)
    {
        m_sprite.reset();
        return;
    }

    m_sprite.emplace(*pTexture);
    m_sprite->setPosition(m_vPosition);
}

bool GameObject::HasTexture() const
{
    return m_sprite.has_value();
}
