#pragma once

#include <vector>
#include <SFML/Graphics.hpp>

class DFS_Node
{
private:
  // Variables
  bool isObstacle_;
  bool isVisited_;
  bool isFrontier_;
  bool isPath_;
  sf::Vector2i pos_;
  std::vector<DFS_Node*> vecNeighbours_;
  DFS_Node* parent_;

public:
  // Constructor
  DFS_Node();

  // Destructor
  ~DFS_Node();

  // Functions
  const bool isObstacle() const;
  const bool isVisited() const;
  const bool isFrontier() const;
  const bool isPath() const;

  // Accessors
  sf::Vector2i getPos();
  DFS_Node* getParentNode();
  std::vector<DFS_Node*>* getNeighbours();

  // Mutators
  void setObstacle(bool b);
  void setVisited(bool b);
  void setFrontier(bool b);
  void setPath(bool b);
  void setPosition(sf::Vector2i pos);
  void setNeighbours(DFS_Node* node);
  void setParentNode(DFS_Node* node);
};
