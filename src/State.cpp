#include "State.h"

State::State(sf::RenderWindow* window, std::stack<State*>* states)
    : window_{window}, states_{states}, quit_{false} {}

State::~State() {}

const bool& State::getQuit() const { return quit_; }

void State::checkForQuit() {
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
    quit_ = true;
  }
}

void State::updateMousePosition() {
  mousePositionWindow_ = sf::Mouse::getPosition(*window_);
}
