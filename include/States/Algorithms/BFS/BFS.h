#pragma once

#include <queue>

#include "State.h"
#include "Button.h"
#include "Node.h"

class BFS: public State
{
private:
  // window related
  sf::Vector2u windowSize_;
  sf::VideoMode videoMode_;

  // fonts
  sf::Font font_;

  // buttons
  std::map<std::string, Button*> buttons_;

  // Map Variables
  unsigned int gridSize_;
  unsigned int offset_;
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
  void initButtons();
  void initVariables();
  void initNodes();
  void initBFS();

public:
  BFS(sf::RenderWindow* window, std::stack<State*>* states);

  virtual ~BFS();

  // Override Functions
  void endState();
  void updateKeybinds();
  void update();
  void render();

  void updateButtons();
  void updateNodes();

  void renderButtons();
  void renderNodes();

  void solve_BFS();
};
