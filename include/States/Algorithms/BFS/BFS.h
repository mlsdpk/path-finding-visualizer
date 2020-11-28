#pragma once

#include <queue>

#include "State.h"
#include "Button.h"
#include "Node.h"

class BFS: public State
{
private:
  sf::Vector2u windowSize_;
  sf::VideoMode videoMode_;

  sf::Font font_;

  // buttons
  std::map<std::string, Button*> buttons_;

  // Map Variables
  unsigned int gridSize_;
  unsigned int offset_;
  unsigned int mapWidth_;
  unsigned int mapHeight_;

  // Node Variables
  Node* nodes_;
  Node* nodeStart_;
  Node* nodeEnd_;

  std::queue<Node*> frontier_;

  bool BFS_running_;
  bool BFS_initialized_;
  bool BFS_reset_;

  // Functions
  void initFonts();
  void initButtons();

  void initBFS();

public:
  BFS(sf::RenderWindow* window, std::stack<State*>* states);

  virtual ~BFS();

  // Functions
  void endState();
  void updateKeybinds();
  void update();
  void render();

  void initVariables();
  void initializeNodes();

  void updateNodes();
  void renderNodes();

  void updateButtons();
  void renderButtons();

  void solve_BFS();
};
