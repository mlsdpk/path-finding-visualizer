#pragma once

#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

enum button_states{
  BTN_IDLE = 0,
  BTN_HOVER = 1,
  BTN_ACTIVE = 2
};

namespace gui
{
  class Button
  {
  private:
    unsigned int buttonState_;
    bool pressed_;
    bool hover_;
    bool selected_;

    sf::RectangleShape shape_;
    sf::Font* font_;
    sf::Text text_;

    sf::Color idleColor_;
    sf::Color hoverColor_;
    sf::Color activeColor_;

  public:
    // Constructor
    Button(float x, float y, float width, float height,
      sf::Font* font, std::string text, int fontSize,
      sf::Color idleColor, sf::Color hoverColor, sf::Color activeColor);

    // Destructor
    ~Button();

    // Accessors
    const bool isHovered() const;
    const bool isPressed() const;
    const bool isIdled() const;
    const sf::String& getText();

    // Mutators
    void setSelected(bool i);

    // Functions
    void update(const sf::Vector2f mousePos);
    void render(sf::RenderWindow* window);
  };

  class DropDownList
  {
  private:
    sf::Font* font_;
    gui::Button* mainButton_;
    gui::Button* activeButton_;
    std::vector<gui::Button*> list_;
    bool showList_;
    bool activeMode_;

    float keyTime_;
    float keyTimeMax_;

  public:
    // Constructor
    DropDownList(float x, float y, float width, float height,
      sf::Font* font, std::string text, const std::vector<std::string> &algo_vec,
      unsigned numOfElements, unsigned default_index = 0);

    // Destructor
    ~DropDownList();

    // Accessors
    const bool getKeyTime();
    gui::Button* getActiveButton();
    const bool hasActiveButton() const;

    // Functions
    void updateKeyTime(const float &dt);
    void update(const sf::Vector2f mousePos, const float &dt);
    void render(sf::RenderWindow* window);
  };
}
