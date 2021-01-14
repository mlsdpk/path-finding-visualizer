#include "Dijkstra_Node.h"

// Constructor
Dijkstra_Node::Dijkstra_Node()
    : isObstacle_{false},
      isVisited_{false},
      isFrontier_{false},
      isPath_{false},
      parent_{nullptr},
      dist_{INT_MAX} {}

// Destructor
Dijkstra_Node::~Dijkstra_Node() {}

// Functions
const bool Dijkstra_Node::isObstacle() const { return isObstacle_; }

const bool Dijkstra_Node::isVisited() const { return isVisited_; }

const bool Dijkstra_Node::isFrontier() const { return isFrontier_; }

const bool Dijkstra_Node::isPath() const { return isPath_; }

// Accessors
sf::Vector2i Dijkstra_Node::getPos() { return pos_; }

Dijkstra_Node* Dijkstra_Node::getParentNode() { return parent_; }

std::vector<Dijkstra_Node*>* Dijkstra_Node::getNeighbours() {
  return &vecNeighbours_;
}

float Dijkstra_Node::getDistance() const { return dist_; }

// Mutators
void Dijkstra_Node::setObstacle(bool b) { isObstacle_ = b; }

void Dijkstra_Node::setVisited(bool b) { isVisited_ = b; }

void Dijkstra_Node::setFrontier(bool b) { isFrontier_ = b; }

void Dijkstra_Node::setPath(bool b) { isPath_ = b; }

void Dijkstra_Node::setPosition(sf::Vector2i pos) { pos_ = pos; }

void Dijkstra_Node::setNeighbours(Dijkstra_Node* Dijkstra_Node) {
  vecNeighbours_.push_back(Dijkstra_Node);
}

void Dijkstra_Node::setParentNode(Dijkstra_Node* Dijkstra_Node) {
  parent_ = Dijkstra_Node;
}

void Dijkstra_Node::setDistance(float dist) { dist_ = dist; }
