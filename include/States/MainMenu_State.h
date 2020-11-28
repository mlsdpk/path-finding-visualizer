#pragma once

#include "State.h"

class MainMenu_State: public State
{
private:
  void initFonts();

public:
  MainMenu_State(sf::RenderWindow* window);

  virtual ~MainMenu_State();

  // Functions
  void endState();
  void updateKeybinds();
  void update();
  void render();
};
