#pragma once

#include "Gui.h"
#include "State.h"

// Algorithms headers
#include "ASTAR.h"
#include "BFS.h"
#include "DFS.h"
#include "DIJKSTRA.h"

class MainMenu_State : public State {
 private:
  sf::Color BGN_COL, FONT_COL, IDLE_COL, HOVER_COL;

  sf::Font font1_, font2_;
  sf::RectangleShape background_;
  sf::Texture backgroundTexture_;
  sf::Text backgroundText_, versionText_;

  std::vector<std::string> algo_vec_;

  std::map<std::string, std::unique_ptr<gui::Button>> buttons_;
  std::unique_ptr<gui::DropDownList> ddl_;

  // Functions
  void initFonts();
  void initColors();
  void initBackground();
  void initAlgorithms();
  void initButtons();

 public:
  MainMenu_State(sf::RenderWindow *window,
                 std::stack<std::unique_ptr<State>> &states);

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
