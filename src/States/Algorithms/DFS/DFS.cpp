#include "DFS.h"

// Constructor
DFS::DFS(sf::RenderWindow* window, std::stack<State*>* states)
    : State(window, states),
      keyTimeMax_{1.f},
      keyTime_{0.f} {
  initVariables();
  initFonts();
  initColors();
  initBackground();
  initButtons();
  initNodes();
  initDFS();
}

// Destructor
DFS::~DFS()
{
  delete[] nodes_;
}

void DFS::initVariables() {
  /*
    @return void

    - initialize all variables
  */

  // these variables depend on the visualizer
  // for now, just use these and can improve it later
  gridSize_   = 20;
  mapWidth_   = 900;
  mapHeight_  = 640;

  nodes_ = new DFS_Node[(mapWidth_/gridSize_)*(mapHeight_/gridSize_)];

  DFS_running_ = false;
  DFS_initialized_ = false;
  DFS_reset_ = false;
  DFS_solved_ = false;
}

void DFS::initFonts() {
  /*
    @return void

    - load font from file
  */

  if (!font1_.loadFromFile("../fonts/bungee-inline-regular.ttf")) {
    throw("ERROR::MainMenuSTATE::COULD NOT LOAD FONT1.");
  }
  if (!font2_.loadFromFile("../fonts/american-typewriter-regular.ttf")) {
    throw("ERROR::MainMenuSTATE::COULD NOT LOAD FONT2.");
  }
}

void DFS::initColors() {
  BGN_COL = sf::Color(246, 229, 245, 255);
  FONT_COL = sf::Color(78, 95, 131, 255);
  IDLE_COL = sf::Color(251, 244, 249, 255);
  HOVER_COL = sf::Color(245, 238, 243, 255);
  ACTIVE_COL= sf::Color(232, 232, 232);
  START_COL = sf::Color(190, 242, 227, 255);
  END_BORDER_COL = sf::Color(67, 246, 130);
  VISITED_COL = sf::Color(198, 202, 229, 255);
  FRONTIER_COL = sf::Color(242, 204, 209, 255);
  OBST_COL = sf::Color(186, 186, 186, 255);
  PATH_COL = sf::Color(190, 242, 227, 255);
}

void DFS::initBackground() {
  // Main title
  titleText_.setFont(font1_);
  titleText_.setString("BREADTH FIRST SEARCH");
  titleText_.setFillColor(FONT_COL);
  titleText_.setCharacterSize(20);
  sf::FloatRect textRect = titleText_.getLocalBounds();
  titleText_.setOrigin(textRect.left + textRect.width/2.0f,
    textRect.top  + textRect.height/2.0f);
  titleText_.setPosition(
    sf::Vector2f(mapWidth_/6.0f, 50.0f)
  );

  // Cell names description
  cellNamesBGN_.setPosition(sf::Vector2f(mapWidth_/3.0f, 0.f));
  cellNamesBGN_.setSize(sf::Vector2f(mapWidth_, 60));
  cellNamesBGN_.setFillColor(IDLE_COL);
}

void DFS::initButtons() {
  /*
    @return void

    - initialize two buttons
  */

  buttons_["RUN"] = new gui::Button(150, 150, 150, 40,
      &font2_, "RUN", 18,
      IDLE_COL,
      HOVER_COL,
      ACTIVE_COL
  );

  buttons_["RESET"] = new gui::Button(150, 210, 150, 40,
      &font2_, "RESET", 18,
      IDLE_COL,
      HOVER_COL,
      ACTIVE_COL
  );
}

void DFS::initNodes() {
  /*
    @return void

    - set all nodes to obstacle free and not visited
    - set all neighbours of each node
    - set start and end nodes
  */

  // set all nodes to free obsts and respective positions
  for (int x = 0; x < mapHeight_/gridSize_; x++) {
    for (int y = 0; y < mapWidth_/gridSize_; y++) {
      int nodeIndex = (mapWidth_/gridSize_) * x + y;
      nodes_[nodeIndex].setPosition(sf::Vector2i(x, y));
      nodes_[nodeIndex].setObstacle(false);
      nodes_[nodeIndex].setVisited(false);
      nodes_[nodeIndex].setFrontier(false);
      nodes_[nodeIndex].setPath(false);
      nodes_[nodeIndex].setParentNode(nullptr);
    }
  }

  // add all neighbours to respective nodes
  for (int x = 0; x < mapHeight_/gridSize_; x++) {
    for (int y = 0; y < mapWidth_/gridSize_; y++) {
      int nodeIndex = (mapWidth_/gridSize_) * x + y;
      if(y>0)
        nodes_[nodeIndex].setNeighbours(&nodes_[x * (mapWidth_/gridSize_) + (y - 1)]);
      if(y<((mapWidth_/gridSize_)-1))
        nodes_[nodeIndex].setNeighbours(&nodes_[x * (mapWidth_/gridSize_) + (y + 1)]);
      if (x>0)
        nodes_[nodeIndex].setNeighbours(&nodes_[(x - 1) * (mapWidth_/gridSize_) + y]);
      if(x<((mapHeight_/gridSize_)-1))
        nodes_[nodeIndex].setNeighbours(&nodes_[(x + 1) * (mapWidth_/gridSize_) + y]);
    }
  }

  // initialize Start and End nodes ptrs (upper left and lower right corners)
  nodeStart_ = &nodes_[(mapWidth_/gridSize_) * 0 + 0];
  nodeStart_->setParentNode(nullptr);
  nodeEnd_   = &nodes_[(mapWidth_/gridSize_) * (mapHeight_/gridSize_ - 1) + (mapWidth_/gridSize_ - 1)];
}

void DFS::initDFS() {
  /*
    @return void

    - initialize BFS by clearing frontier and add start node
  */

  // clear frontier
  while (!frontier_.empty()) {
    frontier_.pop();
  }

  frontier_.push(nodeStart_);
}

void DFS::endState() {
  /*
    @return void

    - ending state function
  */

  std::cout << "Ending DFS State" << '\n';
}

void DFS::updateKeybinds() {
  /*
    @return void

    - checking ESCAPE key to exit
  */

  checkForQuit();
}

void DFS::updateButtons() {
  /*
    @return void

    - assigns boolean flags
  */

  for (auto &it: buttons_) {
    it.second->update(sf::Vector2f(mousePositionWindow_));
  }

  // START the algorithm
  if (buttons_["RUN"]->isPressed() && getKeyTime() && !DFS_solved_) {
    DFS_running_ = true;
  }

  // RESET the nodes
  if (buttons_["RESET"]->isPressed() && getKeyTime()) {
    DFS_reset_ = true;
  }
}

void DFS::update(const float &dt) {
  /*
    @return void

    - update mouse, keys and buttons
    - logic to start and reset the program
  */
  updateKeyTime(dt);
  updateMousePosition(); // from base class
  updateKeybinds();
  updateButtons();

  if (DFS_reset_) {
    initNodes();
    DFS_running_ = false;
    DFS_initialized_ = false;
    DFS_reset_ = false;
    DFS_solved_ = false;
  }

  if (DFS_running_) {
    DFS_reset_ = false;

    // initialize BFS from starting node
    if (!DFS_initialized_) {
      initDFS();
      DFS_initialized_ = true;
    }

    // run the main algorithm
    solve_DFS();
  }
  else {
    // only allow mouse and key inputs
    // if the algorithm is not running
    updateNodes();
  }
}

void DFS::render() {
  /*
    @return void

    - render buttons and nodes
  */

  renderBackground();
  renderButtons();
  renderNodes();
}

void DFS::renderBackground() {
  window_->clear(BGN_COL);

  window_->draw(titleText_);
  window_->draw(cellNamesBGN_);
}

/**
 * Getter function for BFS key timer.
 *
 * @param none.
 * @return true if keytime > keytime_max else false.
 */
const bool DFS::getKeyTime() {
  if (keyTime_ >= keyTimeMax_) {
    keyTime_ = 0.f;
    return true;
  }
  return false;
}

/**
 * Update the current time of key timer.
 *
 * @param dt delta time.
 * @return void.
 */
void DFS::updateKeyTime(const float &dt) {
  if (keyTime_ < keyTimeMax_) {
    keyTime_ += 5.f * dt;
  }
}

void DFS::updateNodes() {
  /*
    @return void

    - update nodes accordingly using mouse and keys pressed
  */

  if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && getKeyTime()) {
    int localY = ((mousePositionWindow_.x - 300) / gridSize_);
    int localX = ((mousePositionWindow_.y - 60) / gridSize_);

    if (localX >= 0 && localX < mapHeight_/gridSize_) {
      if (localY >= 0 && localY < mapWidth_/gridSize_) {

        if (!DFS_solved_) {
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
        else {
          if ((sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))) {
            nodeEnd_ = &nodes_[(mapWidth_/gridSize_) * localX + localY];
          }
        }
      }
    }
  }
}

void DFS::renderButtons() {
  /*
    @return

    - render all the buttons
  */

  for (auto &it: buttons_) {
    it.second->render(window_);
  }
}

void DFS::renderNodes() {
  /*
    @return void

    - render all the nodes to their respective colors
    - render final path with backtracking
  */

  for (int x = 0; x < mapHeight_/gridSize_; x++) {
    for (int y = 0; y < mapWidth_/gridSize_; y++) {

      float size = static_cast<float>(gridSize_);
      sf::RectangleShape rectangle(sf::Vector2f(size, size));
      rectangle.setOutlineThickness(2.f);
      rectangle.setOutlineColor(BGN_COL);
      rectangle.setPosition(300 + y*size, 60 + x*size);

      if (nodes_[(mapWidth_/gridSize_) * x + y].isObstacle()) {
        rectangle.setFillColor(OBST_COL);
      }
      else {
        rectangle.setFillColor(IDLE_COL);
      }

      if (nodes_[(mapWidth_/gridSize_) * x + y].isVisited()) {
        rectangle.setFillColor(VISITED_COL);
      }

      if (nodes_[(mapWidth_/gridSize_) * x + y].isFrontier()) {
        rectangle.setFillColor(FRONTIER_COL);
      }

      if (nodes_[(mapWidth_/gridSize_) * x + y].isPath()) {
        rectangle.setFillColor(START_COL);
        nodes_[(mapWidth_/gridSize_) * x + y].setPath(false);
      }

      if (&nodes_[(mapWidth_/gridSize_) * x + y] == nodeStart_) {
        rectangle.setFillColor(START_COL);
      }

      if (&nodes_[(mapWidth_/gridSize_) * x + y] == nodeEnd_) {
        rectangle.setFillColor(END_BORDER_COL);
      }

      window_->draw(rectangle);
    }
  }

  // visualizing path
  if (nodeEnd_ != nullptr) {
    DFS_Node* current = nodeEnd_;

    while (current->getParentNode() != nullptr && current != nodeStart_) {
      current->setPath(true);
      current = current->getParentNode();
    }
  }
}

void DFS::solve_DFS() {
  /*
    @return void

    - DFS algorithm
  */

  if (!frontier_.empty()) {
    DFS_Node* nodeCurrent = frontier_.top();
    nodeCurrent->setFrontier(false);
    frontier_.pop();

    if (nodeCurrent == nodeEnd_) {
      DFS_running_ = false;
      DFS_solved_ = true;
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
