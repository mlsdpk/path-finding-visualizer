#include "Node.h"

namespace path_finding_visualizer {
namespace graph_based {

// Constructor
Node::Node()
    : isObstacle_{false},
      isVisited_{false},
      isFrontier_{false},
      isPath_{false},
      isStart_{false},
      isGoal_{false},
      parent_{nullptr},
      gDist_{INFINITY},
      fDist_{INFINITY} {}

// Destructor
Node::~Node() {}

// Functions
const bool Node::isObstacle() const { return isObstacle_; }

const bool Node::isVisited() const { return isVisited_; }

const bool Node::isFrontier() const { return isFrontier_; }

const bool Node::isPath() const { return isPath_; }

const bool Node::isStart() const { return isStart_; }

const bool Node::isGoal() const { return isGoal_; }

// Accessors
sf::Vector2i Node::getPos() const { return pos_; }

std::shared_ptr<Node> Node::getParentNode() { return parent_; }

const std::vector<std::shared_ptr<Node>>* Node::getNeighbours() const {
  return &vecNeighbours_;
}

void Node::clearNeighbours() { vecNeighbours_.clear(); }

const double Node::getGDistance() const { return gDist_; }

const double Node::getFDistance() const { return fDist_; }

// Mutators
void Node::setObstacle(bool b) { isObstacle_ = b; }

void Node::setVisited(bool b) { isVisited_ = b; }

void Node::setFrontier(bool b) { isFrontier_ = b; }

void Node::setPath(bool b) { isPath_ = b; }

void Node::setStart(bool b) { isStart_ = b; }

void Node::setGoal(bool b) { isGoal_ = b; }

void Node::setPosition(sf::Vector2i pos) { pos_ = pos; }

void Node::setNeighbours(std::shared_ptr<Node> node) {
  vecNeighbours_.push_back(node);
}

void Node::setParentNode(std::shared_ptr<Node> node) { parent_ = node; }

void Node::setGDistance(double dist) { gDist_ = dist; }

void Node::setFDistance(double dist) { fDist_ = dist; }

}  // namespace graph_based
}  // namespace path_finding_visualizer
