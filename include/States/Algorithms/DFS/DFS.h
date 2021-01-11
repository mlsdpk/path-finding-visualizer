#pragma once

#include <stack>

#include "State.h"
#include "Gui.h"
#include "States/Algorithms/DFS/DFS_Node.h"

class DFS: public State
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

  // DFS related
  DFS_Node* nodes_;
  DFS_Node* nodeStart_;
  DFS_Node* nodeEnd_;
  std::stack<DFS_Node*> frontier_;

  // logic flags
  bool DFS_running_;
  bool DFS_initialized_;
  bool DFS_reset_;
  bool DFS_solved_;

  // initialization Functions
  void initFonts();
  void initColors();
  void initBackground();
  void initButtons();
  void initVariables();
  void initNodes();
  void initDFS();

  void updateKeyTime(const float &dt);
  const bool getKeyTime();

public:
  // Constructor
  DFS(sf::RenderWindow* window, std::stack<State*>* states);

  // Destructor
  virtual ~DFS();

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
  void solve_DFS();
};
