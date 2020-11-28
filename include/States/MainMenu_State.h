#pragma once

#include "State.h"
#include "Button.h"

// Algorithms headers
#include "BFS.h"

class MainMenu_State: public State
{
private:
  sf::Font font_;
  sf::RectangleShape background_;

  std::map<std::string, Button*> buttons_;

  // Functions
  void initFonts();
  void initButtons();

public:
  MainMenu_State(sf::RenderWindow* window, std::stack<State*>* states);

  virtual ~MainMenu_State();

  // Functions
  void endState();
  void updateKeybinds();
  void update();
  void render();

  void updateButtons();
  void renderButtons();
};
