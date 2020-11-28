#pragma once

#include "BFS.h"
#include "MainMenu_State.h"
#include "Node.h"

class Game
{
private:
  // Variables
  sf::RenderWindow* window_;
  sf::Event ev_;

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
  void update();
  void render();
};
