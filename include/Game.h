#pragma once

#include "MainMenu_State.h"

class Game {
 private:
  sf::RenderWindow* window_;
  sf::Event ev_;
  sf::Clock dtClock_;
  float dt_;

  // States
  std::stack<std::unique_ptr<State>> states_;

 public:
  // Constructors
  Game(sf::RenderWindow* window, unsigned int frameLimit);

  // Destructors
  virtual ~Game();

  // Accessors
  const bool running() const;

  // Functions
  void pollEvents();
  void updateDt();
  void update();
  void render();
};
