#pragma once

#include "MainMenu_State.h"

class Game {
 private:
  // Variables
  sf::RenderWindow* window_;
  sf::Event ev_;

  sf::Clock dtClock_;
  float dt_;

  // States
  std::stack<State*> states_;

  // Private Functions
  void initWindow(sf::RenderWindow* window);
  void initStates();

 public:
  // Constructors
  Game(sf::RenderWindow* window);

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
