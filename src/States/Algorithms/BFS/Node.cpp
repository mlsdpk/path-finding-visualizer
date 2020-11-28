#include "Node.h"

// Constructor
Node::Node():
  isObstacle_{false},
  isVisited_{false},
  isFrontier_{false},
  parent_{nullptr}
{

}

// Destructor
Node::~Node()
{

}

// Functions
const bool Node::isObstacle() const
{
  return isObstacle_;
}

const bool Node::isVisited() const
{
  return isVisited_;
}

const bool Node::isFrontier() const
{
  return isFrontier_;
}

// Accessors
sf::Vector2i Node::getPos()
{
  return pos_;
}

Node* Node::getParentNode()
{
  return parent_;
}

// Mutators
void Node::setObstacle(bool b)
{
  isObstacle_ = b;
}

void Node::setVisited(bool b)
{
  isVisited_ = b;
}

void Node::setFrontier(bool b)
{
  isFrontier_ = b;
}

void Node::setPosition(sf::Vector2i pos)
{
  pos_ = pos;
}

void Node::setNeighbours(Node* node)
{
  vecNeighbours_.push_back(node);
}

void Node::setParentNode(Node* node)
{
  parent_ = node;
}
