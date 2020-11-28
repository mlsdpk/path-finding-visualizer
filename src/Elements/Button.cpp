#include "Button.h"

Button::Button(float x, float y, float width, float height,
    sf::Font* font, std::string text, int fontSize,
    sf::Color idleColor, sf::Color hoverColor, sf::Color activeColor):
  buttonState_{BTN_IDLE},
  font_{font},
  idleColor_{idleColor},
  hoverColor_{hoverColor},
  activeColor_{activeColor}
{
  shape_.setPosition(sf::Vector2f(x, y));
  shape_.setSize(sf::Vector2f(width, height));
  shape_.setFillColor(idleColor_);

  text_.setFont(*font_);
  text_.setString(text);
  text_.setFillColor(sf::Color::White);
  text_.setCharacterSize(fontSize);
  text_.setPosition(
    shape_.getPosition().x + (shape_.getGlobalBounds().width / 2.f) - text_.getGlobalBounds().width / 2.f,
    shape_.getPosition().y + (shape_.getGlobalBounds().height / 2.f) - text_.getGlobalBounds().height / 2.f
  );
}

Button::~Button()
{

}

const bool Button::isPressed() const
{
  if (buttonState_ == BTN_ACTIVE)
    return true;

  return false;
}

void Button::update(const sf::Vector2f mousePos)
{
  buttonState_ = BTN_IDLE;

  if (shape_.getGlobalBounds().contains(mousePos)) {
    buttonState_ = BTN_HOVER;

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
      buttonState_ = BTN_ACTIVE;
    }
  }

  switch (buttonState_) {
    case BTN_IDLE:
      shape_.setFillColor(idleColor_);
      break;

    case BTN_HOVER:
      shape_.setFillColor(hoverColor_);
      break;

    case BTN_ACTIVE:
      shape_.setFillColor(activeColor_);
      break;

    default:
      break;
  }
}

void Button::render(sf::RenderWindow* window)
{
  window->draw(shape_);
  window->draw(text_);
}
