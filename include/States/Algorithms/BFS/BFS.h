#pragma once

#include <queue>

#include "State.h"
#include "Gui.h"
#include "Node.h"

class BFS: public State
{
private:
  // fonts
  sf::Font font1_, font2_;

  // background related
  sf::Text titleText_;
  sf::RectangleShape cellNamesBGN_;

  // colors
  sf::Color BGN_COL, FONT_COL,
            IDLE_COL, HOVER_COL, ACTIVE_COL,
            START_COL, END_BORDER_COL,
            VISITED_COL, FRONTIER_COL, OBST_COL, PATH_COL;

  // buttons
  std::map<std::string, gui::Button*> buttons_;

  // key timers
  float keyTime_;
  float keyTimeMax_;

  // Map Variables
  unsigned int gridSize_;
  unsigned int mapWidth_;
  unsigned int mapHeight_;

  // BFS related
  Node* nodes_;
  Node* nodeStart_;
  Node* nodeEnd_;
  std::queue<Node*> frontier_;

  // logic flags
  bool BFS_running_;
  bool BFS_initialized_;
  bool BFS_reset_;
  bool BFS_solved_;

  // initialization Functions
  void initFonts();
  void initColors();
  void initBackground();
  void initButtons();
  void initVariables();
  void initNodes();
  void initBFS();

  void updateKeyTime(const float &dt);
  const bool getKeyTime();

public:
  // Constructor
  BFS(sf::RenderWindow* window, std::stack<State*>* states);

  // Destructor
  virtual ~BFS();

  // Override Functions
  void endState();
  void updateKeybinds();
  void update(const float &dt);
  void render();

  // New update functions
  void updateButtons();
  void updateNodes();

  // New render functions
  void renderBackground();
  void renderButtons();
  void renderNodes();

  // BFS algorithm function
  void solve_BFS();
};
