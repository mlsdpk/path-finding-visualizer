#pragma once

#include <math.h>

#include <SFML/Graphics.hpp>
#include <vector>

class Dijkstra_Node {
 private:
  // Variables
  bool isObstacle_;
  bool isVisited_;
  bool isFrontier_;
  bool isPath_;
  sf::Vector2i pos_;
  std::vector<Dijkstra_Node*> vecNeighbours_;
  Dijkstra_Node* parent_;
  double dist_;

 public:
  // Constructor
  Dijkstra_Node();

  // Destructor
  ~Dijkstra_Node();

  // Functions
  const bool isObstacle() const;
  const bool isVisited() const;
  const bool isFrontier() const;
  const bool isPath() const;

  // Accessors
  sf::Vector2i getPos();
  Dijkstra_Node* getParentNode();
  std::vector<Dijkstra_Node*>* getNeighbours();
  double getDistance() const;

  // Mutators
  void setObstacle(bool b);
  void setVisited(bool b);
  void setFrontier(bool b);
  void setPath(bool b);
  void setPosition(sf::Vector2i pos);
  void setNeighbours(Dijkstra_Node* Dijkstra_Node);
  void setParentNode(Dijkstra_Node* Dijkstra_Node);
  void setDistance(double dist);
};
