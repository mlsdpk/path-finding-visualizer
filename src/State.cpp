#include "State.h"

namespace path_finding_visualizer {

State::State(sf::RenderWindow *window,
             std::stack<std::unique_ptr<State>> &states)
    : window_{window}, states_{states}, quit_{false} {}

State::~State() {}

const bool State::getQuit() const { return quit_; }

// TODO: Check escape not working properly
void State::checkForQuit() {
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
    quit_ = true;
  }
}

void State::updateMousePosition() {
  mousePositionWindow_ = sf::Mouse::getPosition(*window_);
}

}  // namespace path_finding_visualizer