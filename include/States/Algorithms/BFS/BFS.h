#pragma once

#include "State.h"
#include "Node.h"

class BFS: public State
{
private:
  sf::Vector2u windowSize_;
  sf::VideoMode videoMode_;

  // Map Variables
  unsigned int gridSize_;
  unsigned int offset_;
  unsigned int mapWidth_;
  unsigned int mapHeight_;

  // Node Variables
  Node* nodes_;
  Node* nodeStart_;
  Node* nodeEnd_;

public:
  BFS(sf::RenderWindow* window);

  virtual ~BFS();

  // Functions
  void endState();
  void updateKeybinds();
  void update();
  void render();

  void initVariables();
  void initializeNodes();

  // void updateMouseInputNodes();
  void renderNodes();
};
