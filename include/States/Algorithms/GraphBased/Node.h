#pragma once

#include <math.h>

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

namespace path_finding_visualizer {
namespace graph_based {

class Node {
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
  const bool isStart() const;
  const bool isGoal() const;

  // Accessors
  sf::Vector2i getPos() const;
  std::shared_ptr<Node> getParentNode();
  const std::vector<std::shared_ptr<Node>>* getNeighbours() const;
  const double getGDistance() const;
  const double getFDistance() const;

  // Mutators
  void setObstacle(bool b);
  void setVisited(bool b);
  void setFrontier(bool b);
  void setPath(bool b);
  void setStart(bool b);
  void setGoal(bool b);
  void setPosition(sf::Vector2i pos);
  void setNeighbours(std::shared_ptr<Node> node);
  void clearNeighbours();
  void setParentNode(std::shared_ptr<Node> node);
  void setGDistance(double dist);
  void setFDistance(double dist);

 protected:
  // Variables
  bool isObstacle_;
  bool isVisited_;
  bool isFrontier_;
  bool isPath_;
  bool isStart_;
  bool isGoal_;
  sf::Vector2i pos_;
  std::vector<std::shared_ptr<Node>> vecNeighbours_;
  std::shared_ptr<Node> parent_;
  double gDist_;
  double fDist_;
};

}  // namespace graph_based
}  // namespace path_finding_visualizer