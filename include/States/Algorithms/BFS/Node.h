#pragma once

#include <vector>
#include <SFML/Graphics.hpp>

class Node
{
private:
  // Variables
  bool isObstacle_;
  bool isVisited_;
  bool isFrontier_;
  sf::Vector2i pos_;
  std::vector<Node*> vecNeighbours_;
  Node* parent_;

public:
  // Constructor
  Node();

  // Destructor
  ~Node();

  // Functions
  const bool isObstacle() const;
  const bool isVisited() const;
  const bool isFrontier() const;

  // Accessors
  sf::Vector2i getPos();
  Node* getParentNode();
  std::vector<Node*>* getNeighbours();

  // Mutators
  void setObstacle(bool b);
  void setVisited(bool b);
  void setFrontier(bool b);

  void setPosition(sf::Vector2i pos);
  void setNeighbours(Node* node);
  void setParentNode(Node* node);
};
