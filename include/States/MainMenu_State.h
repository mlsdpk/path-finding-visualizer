#pragma once

#include "State.h"
#include "Gui.h"

// Algorithms headers
#include "BFS.h"

class MainMenu_State: public State
{
private:
  sf::Font font_;
  sf::RectangleShape background_;
  sf::Texture backgroundTexture_;

  std::vector<std::string> algo_vec_;

  std::map<std::string, gui::Button*> buttons_;
  gui::DropDownList* testDDL_;

  // Functions
  void initFonts();
  void initBackground();
  void initAlgorithms();
  void initButtons();

public:
  MainMenu_State(sf::RenderWindow* window, std::stack<State*>* states);

  virtual ~MainMenu_State();

  // Functions
  void endState();
  void updateKeybinds();
  void update(const float &dt);
  void render();

  void renderBackground();
  void updateButtons(const float &dt);
  void renderButtons();
};
