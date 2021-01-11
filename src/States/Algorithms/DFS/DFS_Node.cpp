#include "DFS_Node.h"

// Constructor
DFS_Node::DFS_Node():
  isObstacle_{false},
  isVisited_{false},
  isFrontier_{false},
  isPath_{false},
  parent_{nullptr} {}

// Destructor
DFS_Node::~DFS_Node() {}

// Functions
const bool DFS_Node::isObstacle() const
{
  return isObstacle_;
}

const bool DFS_Node::isVisited() const
{
  return isVisited_;
}

const bool DFS_Node::isFrontier() const
{
  return isFrontier_;
}

const bool DFS_Node::isPath() const
{
  return isPath_;
}

// Accessors
sf::Vector2i DFS_Node::getPos()
{
  return pos_;
}

DFS_Node* DFS_Node::getParentNode()
{
  return parent_;
}

std::vector<DFS_Node*>* DFS_Node::getNeighbours()
{
  return &vecNeighbours_;
}

// Mutators
void DFS_Node::setObstacle(bool b)
{
  isObstacle_ = b;
}

void DFS_Node::setVisited(bool b)
{
  isVisited_ = b;
}

void DFS_Node::setFrontier(bool b)
{
  isFrontier_ = b;
}

void DFS_Node::setPath(bool b)
{
  isPath_ = b;
}

void DFS_Node::setPosition(sf::Vector2i pos)
{
  pos_ = pos;
}

void DFS_Node::setNeighbours(DFS_Node* node)
{
  vecNeighbours_.push_back(node);
}

void DFS_Node::setParentNode(DFS_Node* node)
{
  parent_ = node;
}
