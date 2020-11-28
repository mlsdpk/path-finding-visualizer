#pragma once

#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

enum button_states{
  BTN_IDLE = 0,
  BTN_HOVER,
  BTN_ACTIVE
};

class Button
{
private:
  unsigned int buttonState_;
  bool pressed_;
  bool hover_;

  sf::RectangleShape shape_;
  sf::Font* font_;
  sf::Text text_;

  sf::Color idleColor_;
  sf::Color hoverColor_;
  sf::Color activeColor_;

public:

  Button(float x, float y, float width, float height,
    sf::Font* font, std::string text, int fontSize,
    sf::Color idleColor, sf::Color hoverColor, sf::Color activeColor);

  ~Button();

  // Accessors
  const bool isPressed() const;

  // Functions
  void update(const sf::Vector2f mousePos);
  void render(sf::RenderWindow* window);
};
