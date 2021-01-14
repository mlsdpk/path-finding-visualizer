#include "Dijkstra.h"

// Constructor
Dijkstra::Dijkstra(sf::RenderWindow* window, std::stack<State*>* states)
    : State(window, states), keyTimeMax_{1.f}, keyTime_{0.f} {
  initVariables();
  initFonts();
  initColors();
  initBackground();
  initButtons();
  initNodes();
  initDijkstra();
}

// Destructor
Dijkstra::~Dijkstra() { delete[] nodes_; }

void Dijkstra::initVariables() {
  /*
    @return void

    - initialize all variables
  */

  // these variables depend on the visualizer
  // for now, just use these and can improve it later
  gridSize_ = 20;
  mapWidth_ = 900;
  mapHeight_ = 640;

  nodes_ =
      new Dijkstra_Node[(mapWidth_ / gridSize_) * (mapHeight_ / gridSize_)];

  dijkstra_running_ = false;
  dijkstra_initialized_ = false;
  dijkstra_reset_ = false;
  dijkstra_solved_ = false;
}

void Dijkstra::initFonts() {
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

void Dijkstra::initColors() {
  BGN_COL = sf::Color(246, 229, 245, 255);
  FONT_COL = sf::Color(78, 95, 131, 255);
  IDLE_COL = sf::Color(251, 244, 249, 255);
  HOVER_COL = sf::Color(245, 238, 243, 255);
  ACTIVE_COL = sf::Color(232, 232, 232);
  START_COL = sf::Color(190, 242, 227, 255);
  END_BORDER_COL = sf::Color(67, 246, 130);
  VISITED_COL = sf::Color(198, 202, 229, 255);
  FRONTIER_COL = sf::Color(242, 204, 209, 255);
  OBST_COL = sf::Color(186, 186, 186, 255);
  PATH_COL = sf::Color(190, 242, 227, 255);
}

void Dijkstra::initBackground() {
  // Main title
  titleText_.setFont(font1_);
  titleText_.setString("DIJKSTRA");
  titleText_.setFillColor(FONT_COL);
  titleText_.setCharacterSize(20);
  sf::FloatRect textRect = titleText_.getLocalBounds();
  titleText_.setOrigin(textRect.left + textRect.width / 2.0f,
                       textRect.top + textRect.height / 2.0f);
  titleText_.setPosition(sf::Vector2f(mapWidth_ / 6.0f, 50.0f));

  // Cell names description
  cellNamesBGN_.setPosition(sf::Vector2f(mapWidth_ / 3.0f, 0.f));
  cellNamesBGN_.setSize(sf::Vector2f(mapWidth_, 60));
  cellNamesBGN_.setFillColor(IDLE_COL);
}

void Dijkstra::initButtons() {
  /*
    @return void

    - initialize two buttons
  */

  buttons_["RUN"] = new gui::Button(150, 150, 150, 40, &font2_, "RUN", 18,
                                    IDLE_COL, HOVER_COL, ACTIVE_COL);

  buttons_["RESET"] = new gui::Button(150, 210, 150, 40, &font2_, "RESET", 18,
                                      IDLE_COL, HOVER_COL, ACTIVE_COL);
}

void Dijkstra::initNodes() {
  /*
    @return void

    - set all nodes to obstacle free and not visited
    - set all neighbours of each node
    - set start and end nodes
  */

  // set all nodes to free obsts and respective positions
  for (int x = 0; x < mapHeight_ / gridSize_; x++) {
    for (int y = 0; y < mapWidth_ / gridSize_; y++) {
      int nodeIndex = (mapWidth_ / gridSize_) * x + y;
      nodes_[nodeIndex].setPosition(sf::Vector2i(x, y));
      nodes_[nodeIndex].setObstacle(false);
      nodes_[nodeIndex].setVisited(false);
      nodes_[nodeIndex].setFrontier(false);
      nodes_[nodeIndex].setPath(false);
      nodes_[nodeIndex].setParentNode(nullptr);
      nodes_[nodeIndex].setDistance(INFINITY);
    }
  }

  // add all neighbours to respective nodes
  for (int x = 0; x < mapHeight_ / gridSize_; x++) {
    for (int y = 0; y < mapWidth_ / gridSize_; y++) {
      int nodeIndex = (mapWidth_ / gridSize_) * x + y;
      if (y > 0)
        nodes_[nodeIndex].setNeighbours(
            &nodes_[x * (mapWidth_ / gridSize_) + (y - 1)]);
      if (y < ((mapWidth_ / gridSize_) - 1))
        nodes_[nodeIndex].setNeighbours(
            &nodes_[x * (mapWidth_ / gridSize_) + (y + 1)]);
      if (x > 0)
        nodes_[nodeIndex].setNeighbours(
            &nodes_[(x - 1) * (mapWidth_ / gridSize_) + y]);
      if (x < ((mapHeight_ / gridSize_) - 1))
        nodes_[nodeIndex].setNeighbours(
            &nodes_[(x + 1) * (mapWidth_ / gridSize_) + y]);
    }
  }

  // initialize Start and End nodes ptrs (upper left and lower right corners)
  nodeStart_ = &nodes_[(mapWidth_ / gridSize_) * 0 + 0];
  nodeStart_->setParentNode(nullptr);
  nodeEnd_ = &nodes_[(mapWidth_ / gridSize_) * (mapHeight_ / gridSize_ - 1) +
                     (mapWidth_ / gridSize_ - 1)];
}

void Dijkstra::initDijkstra() {
  /*
    @return void

    - initialize Dijkstra by clearing frontier and add start node
  */

  // clear frontier
  while (!frontier_.empty()) {
    frontier_.pop();
  }
  nodeStart_->setDistance(0.0);
  frontier_.push(nodeStart_);
}

void Dijkstra::endState() {
  /*
    @return void

    - ending state function
  */

  std::cout << "Ending Dijkstra State" << '\n';
}

void Dijkstra::updateKeybinds() {
  /*
    @return void

    - checking ESCAPE key to exit
  */

  checkForQuit();
}

void Dijkstra::updateButtons() {
  /*
    @return void

    - assigns boolean flags
  */

  for (auto& it : buttons_) {
    it.second->update(sf::Vector2f(mousePositionWindow_));
  }

  // START the algorithm
  if (buttons_["RUN"]->isPressed() && getKeyTime() && !dijkstra_solved_) {
    dijkstra_running_ = true;
  }

  // RESET the nodes
  if (buttons_["RESET"]->isPressed() && getKeyTime()) {
    dijkstra_reset_ = true;
  }
}

void Dijkstra::update(const float& dt) {
  /*
    @return void

    - update mouse, keys and buttons
    - logic to start and reset the program
  */
  updateKeyTime(dt);
  updateMousePosition();  // from base class
  updateKeybinds();
  updateButtons();

  if (dijkstra_reset_) {
    initNodes();
    dijkstra_running_ = false;
    dijkstra_initialized_ = false;
    dijkstra_reset_ = false;
    dijkstra_solved_ = false;
  }

  if (dijkstra_running_) {
    dijkstra_reset_ = false;

    // initialize Dijkstra from starting node
    if (!dijkstra_initialized_) {
      initDijkstra();
      dijkstra_initialized_ = true;
    }

    // run the main algorithm
    solve_Dijkstra();
  } else {
    // only allow mouse and key inputs
    // if the algorithm is not running
    updateNodes();
  }
}

void Dijkstra::render() {
  /*
    @return void

    - render buttons and nodes
  */

  renderBackground();
  renderButtons();
  renderNodes();
}

void Dijkstra::renderBackground() {
  window_->clear(BGN_COL);

  window_->draw(titleText_);
  window_->draw(cellNamesBGN_);
}

/**
 * Getter function for Dijkstra key timer.
 *
 * @param none.
 * @return true if keytime > keytime_max else false.
 */
const bool Dijkstra::getKeyTime() {
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
void Dijkstra::updateKeyTime(const float& dt) {
  if (keyTime_ < keyTimeMax_) {
    keyTime_ += 5.f * dt;
  }
}

void Dijkstra::updateNodes() {
  /*
    @return void

    - update nodes accordingly using mouse and keys pressed
  */

  if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && getKeyTime()) {
    int localY = ((mousePositionWindow_.x - 300) / gridSize_);
    int localX = ((mousePositionWindow_.y - 60) / gridSize_);

    if (localX >= 0 && localX < mapHeight_ / gridSize_) {
      if (localY >= 0 && localY < mapWidth_ / gridSize_) {
        if (!dijkstra_solved_) {
          if ((sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))) {
            nodeStart_ = &nodes_[(mapWidth_ / gridSize_) * localX + localY];
          } else if ((sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))) {
            nodeEnd_ = &nodes_[(mapWidth_ / gridSize_) * localX + localY];
          } else {
            nodes_[(mapWidth_ / gridSize_) * localX + localY].setObstacle(
                !nodes_[(mapWidth_ / gridSize_) * localX + localY]
                     .isObstacle());
          }
        } else {
          if ((sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))) {
            nodeEnd_ = &nodes_[(mapWidth_ / gridSize_) * localX + localY];
          }
        }
      }
    }
  }
}

void Dijkstra::renderButtons() {
  /*
    @return

    - render all the buttons
  */

  for (auto& it : buttons_) {
    it.second->render(window_);
  }
}

void Dijkstra::renderNodes() {
  /*
    @return void

    - render all the nodes to their respective colors
    - render final path with backtracking
  */

  for (int x = 0; x < mapHeight_ / gridSize_; x++) {
    for (int y = 0; y < mapWidth_ / gridSize_; y++) {
      float size = static_cast<float>(gridSize_);
      sf::RectangleShape rectangle(sf::Vector2f(size, size));
      rectangle.setOutlineThickness(2.f);
      rectangle.setOutlineColor(BGN_COL);
      rectangle.setPosition(300 + y * size, 60 + x * size);

      if (nodes_[(mapWidth_ / gridSize_) * x + y].isObstacle()) {
        rectangle.setFillColor(OBST_COL);
      } else {
        rectangle.setFillColor(IDLE_COL);
      }

      if (nodes_[(mapWidth_ / gridSize_) * x + y].isVisited()) {
        rectangle.setFillColor(VISITED_COL);
      }

      if (nodes_[(mapWidth_ / gridSize_) * x + y].isFrontier()) {
        rectangle.setFillColor(FRONTIER_COL);
      }

      if (nodes_[(mapWidth_ / gridSize_) * x + y].isPath()) {
        rectangle.setFillColor(START_COL);
        nodes_[(mapWidth_ / gridSize_) * x + y].setPath(false);
      }

      if (&nodes_[(mapWidth_ / gridSize_) * x + y] == nodeStart_) {
        rectangle.setFillColor(START_COL);
      }

      if (&nodes_[(mapWidth_ / gridSize_) * x + y] == nodeEnd_) {
        rectangle.setFillColor(END_BORDER_COL);
      }

      window_->draw(rectangle);
    }
  }

  // visualizing path
  if (nodeEnd_ != nullptr) {
    Dijkstra_Node* current = nodeEnd_;

    while (current->getParentNode() != nullptr && current != nodeStart_) {
      current->setPath(true);
      current = current->getParentNode();
    }
  }
}

double Dijkstra::L1_Distance(Dijkstra_Node* n1, Dijkstra_Node* n2) {
  return fabs(n1->getPos().x - n2->getPos().x) +
         fabs(n1->getPos().y - n2->getPos().y);
}

void Dijkstra::solve_Dijkstra() {
  /*
    @return void

    - Dijkstra algorithm
  */

  if (!frontier_.empty()) {
    Dijkstra_Node* nodeCurrent = frontier_.top();
    nodeCurrent->setFrontier(false);
    nodeCurrent->setVisited(true);
    frontier_.pop();

    for (auto nodeNeighbour : *nodeCurrent->getNeighbours()) {
      if (nodeNeighbour->isVisited() || nodeNeighbour->isObstacle()) {
        continue;
      }

      double dist =
          nodeCurrent->getDistance() + L1_Distance(nodeCurrent, nodeNeighbour);

      if (dist < nodeNeighbour->getDistance()) {
        nodeNeighbour->setParentNode(nodeCurrent);
        nodeNeighbour->setDistance(dist);

        nodeNeighbour->setFrontier(true);
        frontier_.push(nodeNeighbour);
      }
    }

    if (nodeCurrent == nodeEnd_) {
      dijkstra_running_ = false;
      dijkstra_solved_ = true;
    }
  }
}
