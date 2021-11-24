#pragma once

#include <math.h>

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

namespace path_finding_visualizer {
namespace graph_based {

class Node {
 private:
  // Variables
  bool isObstacle_;
  bool isVisited_;
  bool isFrontier_;
  bool isPath_;
  sf::Vector2i pos_;
  std::vector<std::shared_ptr<Node>> vecNeighbours_;
  std::shared_ptr<Node> parent_;
  double gDist_;
  double fDist_;

 public:
  // Constructor
  Node();

  // Destructor
  ~Node();

  // Functions
  const bool isObstacle() const;
  const bool isVisited() const;
  const bool isFrontier() const;
  const bool isPath() const;

  // Accessors
  sf::Vector2i getPos();
  std::shared_ptr<Node> getParentNode();
  const std::vector<std::shared_ptr<Node>>* getNeighbours() const;
  const double getGDistance() const;
  const double getFDistance() const;

  // Mutators
  void setObstacle(bool b);
  void setVisited(bool b);
  void setFrontier(bool b);
  void setPath(bool b);
  void setPosition(sf::Vector2i pos);
  void setNeighbours(std::shared_ptr<Node> node);
  void setParentNode(std::shared_ptr<Node> node);
  void setGDistance(double dist);
  void setFDistance(double dist);
};

}  // namespace graph_based
}  // namespace path_finding_visualizer