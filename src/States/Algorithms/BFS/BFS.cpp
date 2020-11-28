#include "BFS.h"

BFS::BFS(sf::RenderWindow* window): State(window)
{
  initVariables();
  initializeNodes();
}

BFS::~BFS()
{
  delete[] nodes_;
}

void BFS::initVariables()
{
  /*
    @return void

    - initialize all variables
  */

  windowSize_ = window_->getSize();
  gridSize_   = 20;
  offset_     = 80;
  mapWidth_   = 800  - offset_;
  mapHeight_  = 800  - offset_;
}

void BFS::initializeNodes()
{
  /*
    @return void

    - initialize nodes dynamic array
    - set all nodes to obstacle free and not visited
    - set all neighbours of each node
    - set start and end nodes
  */

  nodes_ = new Node[(mapWidth_/gridSize_)*(mapHeight_/gridSize_)];

  // set all nodes to free obsts and respective positions
  for (int x = 0; x < mapWidth_/gridSize_; x++) {
    for (int y = 0; y < mapHeight_/gridSize_; y++) {
      int nodeIndex = (mapWidth_/gridSize_) * x + y;
      nodes_[nodeIndex].setPosition(sf::Vector2i(x, y));
      nodes_[nodeIndex].setObstacle(false);
      nodes_[nodeIndex].setVisited(false);
      nodes_[nodeIndex].setParentNode(nullptr);
    }
  }

  // add all neighbours to respective nodes
  for (int x = 0; x < mapWidth_/gridSize_; x++) {
    for (int y = 0; y < mapHeight_/gridSize_; y++) {
      int nodeIndex = (mapWidth_/gridSize_) * x + y;
      if(y>0)
        nodes_[nodeIndex].setNeighbours(&nodes_[x * (mapWidth_/gridSize_) + (y - 1)]);
      if(y<((mapWidth_/gridSize_)-1))
        nodes_[nodeIndex].setNeighbours(&nodes_[x * (mapWidth_/gridSize_) + (y + 1)]);
      if (x>0)
        nodes_[nodeIndex].setNeighbours(&nodes_[(x - 1) * (mapWidth_/gridSize_) + y]);
      if(x<((mapWidth_/gridSize_)-1))
        nodes_[nodeIndex].setNeighbours(&nodes_[(x + 1) * (mapWidth_/gridSize_) + y]);
    }
  }

  // initialize Start and End nodes ptrs (upper left and lower right corners)
  nodeStart_ = &nodes_[(mapWidth_/gridSize_) * 0 + 0];
  nodeEnd_   = &nodes_[(mapWidth_/gridSize_) * (mapWidth_/gridSize_ - 1) + (mapHeight_/gridSize_ - 1)];
}

void BFS::endState()
{
  std::cout << "Ending BFS State" << '\n';
}

void BFS::updateKeybinds()
{
  checkForQuit();
}

// void BFS::updateMouseInputNodes(const sf::Vector2i &pos)
// {
//   if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
//     int localX = (pos.x / gridSize_) - ((offset_/2)/gridSize_);
//     int localY = (pos.y / gridSize_) - ((offset_/2)/gridSize_);
//
//     if (localX >= 0 && localX < mapWidth_/gridSize_) {
//       if (localY >= 0 && localY < mapHeight_/gridSize_) {
//
//         if ((sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))) {
//           nodeStart_ = &nodes_[(mapWidth_/gridSize_) * localX + localY];
//         }
//         else if ((sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))) {
//           nodeEnd_ = &nodes_[(mapWidth_/gridSize_) * localX + localY];
//         }
//         else {
//           nodes_[(mapWidth_/gridSize_) * localX + localY].setObstacle(!nodes_[(mapWidth_/gridSize_) * localX + localY].isObstacle());
//         }
//       }
//     }
//   }
// }

void BFS::update()
{
  updateMousePosition();
  updateKeybinds();
}

void BFS::render()
{
  renderNodes();
}

void BFS::renderNodes()
{
  /*
    @return void

    - render all the nodes to their respective colors
    - empty node    (WHITE)
    - obstacle node (GRAY)
    - start node    (GREEN)
    - end node      (RED)
    - visited node  (CYAN)
    - frontier node (MAGENTA)
  */

  for (int x = 0; x < mapWidth_/gridSize_; x++) {
    for (int y = 0; y < mapWidth_/gridSize_; y++) {

      float size = static_cast<float>(gridSize_);
      sf::RectangleShape rectangle(sf::Vector2f(size, size));
      rectangle.setOutlineThickness(2.f);
      rectangle.setOutlineColor(sf::Color::Black);
      rectangle.setPosition(offset_/2 + x*size, offset_/2 + y*size);

      if (nodes_[(mapWidth_/gridSize_) * x + y].isObstacle()) {
        rectangle.setFillColor(sf::Color(128,128,128));
      }
      else {
        rectangle.setFillColor(sf::Color::White);
      }

      if (&nodes_[(mapWidth_/gridSize_) * x + y] == nodeStart_) {
        rectangle.setFillColor(sf::Color::Green);
      }

      if (&nodes_[(mapWidth_/gridSize_) * x + y] == nodeEnd_) {
        rectangle.setFillColor(sf::Color::Red);
      }

      if (nodes_[(mapWidth_/gridSize_) * x + y].isVisited()) {
        rectangle.setFillColor(sf::Color::Cyan);
      }

      if (nodes_[(mapWidth_/gridSize_) * x + y].isFrontier()) {
        rectangle.setFillColor(sf::Color::Magenta);
      }

      window_->draw(rectangle);
    }
  }
}
