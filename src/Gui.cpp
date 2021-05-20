#include "Gui.h"

// Constructor
gui::Button::Button(float x, float y, float width, float height, sf::Font* font,
                    std::string text, int fontSize, sf::Color idleColor,
                    sf::Color hoverColor, sf::Color activeColor)
    : buttonState_{BTN_IDLE},
      font_{font},
      idleColor_{idleColor},
      hoverColor_{hoverColor},
      activeColor_{activeColor},
      selected_{false} {
  shape_.setOrigin(width / 2.f, height / 2.f);
  shape_.setPosition(sf::Vector2f(x, y));
  shape_.setSize(sf::Vector2f(width, height));
  shape_.setFillColor(idleColor_);
  shape_.setOutlineThickness(2.f);
  shape_.setOutlineColor(sf::Color(246, 229, 245, 255));

  text_.setFont(*font_);
  text_.setString(text);
  text_.setFillColor(sf::Color(78, 95, 131, 255));
  text_.setCharacterSize(fontSize);
  text_.setPosition(
      shape_.getPosition().x - text_.getGlobalBounds().width / 2.f,
      shape_.getPosition().y - text_.getGlobalBounds().height);
}

// Destructor
gui::Button::~Button() {}

/**
 * Getter function for button IDLE state.
 *
 * @param none.
 * @return true if button is idle, else false.
 */
const bool gui::Button::isIdled() const {
  if (buttonState_ == BTN_IDLE) return true;
  return false;
}

/**
 * Getter function for button HOVER state.
 *
 * @param none.
 * @return true if button is idle, else false.
 */
const bool gui::Button::isHovered() const {
  if (buttonState_ == BTN_HOVER) return true;

  return false;
}

/**
 * Getter function for button PRESS state.
 *
 * @param none.
 * @return true if button is idle, else false.
 */
const bool gui::Button::isPressed() const {
  if (buttonState_ == BTN_ACTIVE) return true;

  return false;
}

/**
 * Getter function for button text.
 *
 * @param none.
 * @return text of button.
 */
const sf::String& gui::Button::getText() { return text_.getString(); }

/**
 * Setter function for button.
 *
 * Setting the button as selected or not.
 *
 * @param i boolean state.
 * @return void.
 */
void gui::Button::setSelected(bool i) { selected_ = i; }

/**
 * Update function of button.
 *
 * @param mousePos current mouse position
 *    information.
 * @return void.
 */
void gui::Button::update(const sf::Vector2f mousePos) {
  buttonState_ = BTN_IDLE;

  if (shape_.getGlobalBounds().contains(mousePos)) {
    buttonState_ = BTN_HOVER;

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
      buttonState_ = BTN_ACTIVE;
    }
  }

  if (selected_) buttonState_ = BTN_HOVER;

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

/**
 * Render function of button.
 *
 * @param window Window object for rendering.
 * @return void.
 */
void gui::Button::render(sf::RenderWindow* window) {
  window->draw(shape_);
  window->draw(text_);
}

// ===== END BUTTON ======

// Constructor
gui::DropDownList::DropDownList(float x, float y, float width, float height,
                                sf::Font* font, std::string text,
                                const std::vector<std::string>& algo_vec)
    : font_{font},
      showList_{false},
      keyTimeMax_{1.f},
      keyTime_{0.f},
      activeButton_{nullptr},
      activeMode_{false} {
  mainButton_ = std::make_unique<gui::Button>(
      x, y, width, height, font_, text, 20, sf::Color(251, 244, 249, 255),
      sf::Color(245, 238, 243, 255), sf::Color(214, 214, 215, 200));

  for (size_t i = 0; i < algo_vec.size(); i++) {
    list_.emplace_back(std::make_unique<gui::Button>(
        x, y + ((i + 1) * height), width, height, font_, algo_vec[i], 20,
        sf::Color(251, 244, 249, 255), sf::Color(245, 238, 243, 255),
        sf::Color(240, 240, 240, 200)));
  }
}

// Destructor
gui::DropDownList::~DropDownList() {}

/**
 * Getter function for DropDownList ACTIVE state.
 *
 * @param none.
 * @return true if button is active, else false.
 */
const bool gui::DropDownList::hasActiveButton() const {
  if (activeButton_ != nullptr) {
    return true;
  }
  return false;
}

/**
 * Getter function for DropDownList ACTIVE state.
 *
 * @param none.
 * @return current active button ptr.
 */
gui::Button* gui::DropDownList::getActiveButton() { return activeButton_; }

void gui::DropDownList::makeButtonInActive() { activeButton_ = nullptr; }

/**
 * Getter function for DropDownList key timer.
 *
 * @param none.
 * @return true if keytime > keytime_max else false.
 */
const bool gui::DropDownList::getKeyTime() {
  if (keyTime_ >= keyTimeMax_) {
    keyTime_ = 0.f;
    return true;
  }
  return false;
}

/**
 * Update the current time of key timer.
 *
 * @param dt delta time.
 * @return void.
 */
void gui::DropDownList::updateKeyTime(const float& dt) {
  if (keyTime_ < keyTimeMax_) {
    keyTime_ += 5.f * dt;
  }
}

/**
 * Update function of DropDownList.
 *
 * @param mousePos current mouse position
 *    information.
 * @param dt delta time.
 * @return void.
 */
void gui::DropDownList::update(const sf::Vector2f mousePos, const float& dt) {
  updateKeyTime(dt);
  mainButton_->update(mousePos);

  if (mainButton_->isPressed() && getKeyTime()) {
    activeMode_ = true;
  }

  if (activeMode_) {
    showList_ = true;
    mainButton_->setSelected(true);
  } else {
    showList_ = false;
    mainButton_->setSelected(false);
  }

  if (showList_) {
    for (auto& i : list_) {
      i->update(mousePos);
      if (i->isPressed()) {
        activeButton_ = i.get();
      }
    }

    if ((sf::Mouse::isButtonPressed(sf::Mouse::Left)) && getKeyTime()) {
      activeMode_ = false;
    }
  }
}

/**
 * Render function of DropDownList.
 *
 * @param window Window object for rendering.
 * @return void.
 */
void gui::DropDownList::render(sf::RenderWindow* window) {
  mainButton_->render(window);

  if (showList_) {
    for (auto& i : list_) {
      i->render(window);
    }
  }
}

// ===== END DropDownList ======
