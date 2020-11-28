#include "BFS.h"

BFS::BFS(sf::RenderWindow* window, std::stack<State*>* states)
    : State(window, states) {
  initVariables();
  initFonts();
  initButtons();
  initializeNodes();
  initBFS();
}

BFS::~BFS()
{
  delete[] nodes_;
}

void BFS::initFonts()
{
  if (!font_.loadFromFile("../fonts/ostrich-regular.ttf")) {
    throw("ERROR::MainMenuSTATE::COULD NOT LOAD FONT.");
  }
}

void BFS::initButtons()
{
  buttons_["START"] = new Button(5, 5, 100, 30,
      &font_, "START", 15,
      sf::Color(70, 70, 70, 200),
      sf::Color(150, 150, 150, 255),
      sf::Color(20, 20, 20, 200)
  );

  buttons_["RESET"] = new Button(110, 5, 100, 30,
      &font_, "RESET", 15,
      sf::Color(70, 70, 70, 200),
      sf::Color(150, 150, 150, 255),
      sf::Color(20, 20, 20, 200)
  );
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

  nodes_ = new Node[(mapWidth_/gridSize_)*(mapHeight_/gridSize_)];

  BFS_running_ = false;
  BFS_initialized_ = false;
  BFS_reset_ = false;
}

void BFS::initializeNodes()
{
  /*
    @return void

    - set all nodes to obstacle free and not visited
    - set all neighbours of each node
    - set start and end nodes
  */

  // set all nodes to free obsts and respective positions
  for (int x = 0; x < mapWidth_/gridSize_; x++) {
    for (int y = 0; y < mapHeight_/gridSize_; y++) {
      int nodeIndex = (mapWidth_/gridSize_) * x + y;
      nodes_[nodeIndex].setPosition(sf::Vector2i(x, y));
      nodes_[nodeIndex].setObstacle(false);
      nodes_[nodeIndex].setVisited(false);
      nodes_[nodeIndex].setFrontier(false);
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
  nodeStart_->setParentNode(nullptr);
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

void BFS::updateNodes()
{
  if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
    int localX = (mousePositionWindow_.x / gridSize_) - ((offset_/2)/gridSize_);
    int localY = (mousePositionWindow_.y / gridSize_) - ((offset_/2)/gridSize_);

    if (localX >= 0 && localX < mapWidth_/gridSize_) {
      if (localY >= 0 && localY < mapHeight_/gridSize_) {

        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))) {
          nodeStart_ = &nodes_[(mapWidth_/gridSize_) * localX + localY];
        }
        else if ((sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))) {
          nodeEnd_ = &nodes_[(mapWidth_/gridSize_) * localX + localY];
        }
        else {
          nodes_[(mapWidth_/gridSize_) * localX + localY].setObstacle(!nodes_[(mapWidth_/gridSize_) * localX + localY].isObstacle());
        }
      }
    }
  }
}

void BFS::update()
{
  updateMousePosition();
  updateKeybinds();
  updateButtons();

  if (BFS_reset_) {
    std::cout << "resetted" << '\n';
    initializeNodes();
    BFS_running_ = false;
    BFS_initialized_ = false;
    BFS_reset_ = false;
  }

  if (BFS_running_) {
    BFS_reset_ = false;

    // initialize BFS from starting node
    if (!BFS_initialized_) {
      initBFS();
      BFS_initialized_ = true;
    }

    solve_BFS();
  }
  else {
    updateNodes();
  }
}

void BFS::render()
{
  renderButtons();
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

      if (nodes_[(mapWidth_/gridSize_) * x + y].isVisited()) {
        rectangle.setFillColor(sf::Color::Cyan);
      }

      if (nodes_[(mapWidth_/gridSize_) * x + y].isFrontier()) {
        rectangle.setFillColor(sf::Color::Magenta);
      }

      if (&nodes_[(mapWidth_/gridSize_) * x + y] == nodeStart_) {
        rectangle.setFillColor(sf::Color::Green);
      }

      if (&nodes_[(mapWidth_/gridSize_) * x + y] == nodeEnd_) {
        rectangle.setFillColor(sf::Color::Red);
      }

      window_->draw(rectangle);
    }
  }

  if (nodeEnd_ != nullptr) {
    Node* current = nodeEnd_;

    while (current->getParentNode() != nullptr && current != nodeStart_) {

      float x1 = static_cast<float>((current->getPos().x * gridSize_) + (gridSize_/2) + offset_/2);
      float y1 = static_cast<float>((current->getPos().y * gridSize_) + (gridSize_/2) + offset_/2);
      float x2 = static_cast<float>((current->getParentNode()->getPos().x * gridSize_) + (gridSize_/2) + offset_/2);
      float y2 = static_cast<float>((current->getParentNode()->getPos().y * gridSize_) + (gridSize_/2) + offset_/2);

      std::cout << "x1: " << x1 << "y1: " << y1 << '\n';

      sf::VertexArray line(sf::Lines, 2);

      line[0].position = sf::Vector2f(x1, y1);
      line[1].position = sf::Vector2f(x2, y2);

      line[0].color = sf::Color::Red;
      line[1].color = sf::Color::Red;

      window_->draw(line);

      current = current->getParentNode();
    }
  }
}

void BFS::updateButtons()
{
  for (auto &it: buttons_) {
    it.second->update(sf::Vector2f(mousePositionWindow_));
  }

  // START/RESET the algorithm
  if (buttons_["START"]->isPressed()) {
    // run BFS here
    BFS_running_ = true;
  }

  if (buttons_["RESET"]->isPressed()) {
    BFS_reset_ = true;
  }
}

void BFS::renderButtons()
{
  for (auto &it: buttons_) {
    it.second->render(window_);
  }
}

void BFS::solve_BFS()
{
  if (!frontier_.empty()) {
    Node* nodeCurrent = frontier_.front();
    nodeCurrent->setFrontier(false);
    frontier_.pop();

    if (nodeCurrent == nodeEnd_) {
      BFS_running_ = false;
    }

    for (auto nodeNeighbour : *nodeCurrent->getNeighbours()) {
      if (!nodeNeighbour->isVisited() && nodeNeighbour->isObstacle() == 0) {
        nodeNeighbour->setParentNode(nodeCurrent);
        nodeNeighbour->setVisited(true);
        nodeNeighbour->setFrontier(true);
        frontier_.push(nodeNeighbour);
      }
    }
  }
}

void BFS::initBFS()
{
  // clear frontier
  while (!frontier_.empty()) {
    frontier_.pop();
  }

  frontier_.push(nodeStart_);
}
