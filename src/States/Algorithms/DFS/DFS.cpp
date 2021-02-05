#include "DFS.h"

// Constructor
DFS::DFS(sf::RenderWindow *window, std::stack<std::unique_ptr<State>> &states)
    : Algorithm(window, states) {}

// Destructor
DFS::~DFS() {}

// override initAlgorithm() function
void DFS::initAlgorithm() {
  // initialize DFS by clearing frontier and add start node
  while (!frontier_.empty()) {
    frontier_.pop();
  }

  frontier_.push(nodeStart_);
}

// override updateNodes() function
void DFS::updateNodes() {
  if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && getKeyTime()) {
    int localY = ((mousePositionWindow_.x - 300) / gridSize_);
    int localX = ((mousePositionWindow_.y - 60) / gridSize_);

    if (localX >= 0 && localX < mapHeight_ / gridSize_) {
      if (localY >= 0 && localY < mapWidth_ / gridSize_) {
        if (!Algorithm_solved_) {
          if ((sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))) {
            nodeStart_ = nodes_[(mapWidth_ / gridSize_) * localX + localY];
          } else if ((sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))) {
            nodeEnd_ = nodes_[(mapWidth_ / gridSize_) * localX + localY];
          } else {
            nodes_[(mapWidth_ / gridSize_) * localX + localY]->setObstacle(
                !nodes_[(mapWidth_ / gridSize_) * localX + localY]
                     ->isObstacle());
          }
        } else {
          if ((sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))) {
            nodeEnd_ = nodes_[(mapWidth_ / gridSize_) * localX + localY];
          }
        }
      }
    }
  }
}

// override renderNodes() function
void DFS::renderNodes() {
  for (int x = 0; x < mapHeight_ / gridSize_; x++) {
    for (int y = 0; y < mapWidth_ / gridSize_; y++) {
      float size = static_cast<float>(gridSize_);
      sf::RectangleShape rectangle(sf::Vector2f(size, size));
      rectangle.setOutlineThickness(2.f);
      rectangle.setOutlineColor(BGN_COL);
      rectangle.setPosition(300 + y * size, 60 + x * size);

      if (nodes_[(mapWidth_ / gridSize_) * x + y]->isObstacle()) {
        rectangle.setFillColor(OBST_COL);
      } else {
        rectangle.setFillColor(IDLE_COL);
      }

      if (nodes_[(mapWidth_ / gridSize_) * x + y]->isVisited()) {
        rectangle.setFillColor(VISITED_COL);
      }

      if (nodes_[(mapWidth_ / gridSize_) * x + y]->isFrontier()) {
        rectangle.setFillColor(FRONTIER_COL);
      }

      if (nodes_[(mapWidth_ / gridSize_) * x + y]->isPath()) {
        rectangle.setFillColor(START_COL);
        nodes_[(mapWidth_ / gridSize_) * x + y]->setPath(false);
      }

      if (nodes_[(mapWidth_ / gridSize_) * x + y] == nodeStart_) {
        rectangle.setFillColor(START_COL);
      }

      if (nodes_[(mapWidth_ / gridSize_) * x + y] == nodeEnd_) {
        rectangle.setFillColor(END_BORDER_COL);
      }

      window_->draw(rectangle);
    }
  }

  // visualizing path
  if (nodeEnd_ != nullptr) {
    std::shared_ptr<Node> current = nodeEnd_;

    while (current->getParentNode() != nullptr && current != nodeStart_) {
      current->setPath(true);
      current = current->getParentNode();
    }
  }
}

void DFS::solveConcurrently(std::shared_ptr<Node> nodeStart,
                            std::shared_ptr<Node> nodeEnd,
                            std::shared_ptr<MessageQueue<bool>> message_queue) {
  // copy assignment
  // thread-safe due to shared_ptrs
  std::shared_ptr<Node> s_nodeStart = nodeStart;
  std::shared_ptr<Node> s_nodeEnd = nodeEnd;
  std::shared_ptr<MessageQueue<bool>> s_message_queue = message_queue;

  bool solved = false;

  double cycleDuration = 1;  // duration of a single simulation cycle in ms
  // init stop watch
  auto lastUpdate = std::chrono::system_clock::now();

  while (true) {
    // compute time difference to stop watch
    long timeSinceLastUpdate =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now() - lastUpdate)
            .count();

    if (timeSinceLastUpdate >= cycleDuration) {
      ////////////////////////////
      // run the main algorithm //
      ////////////////////////////
      if (!frontier_.empty()) {
        std::shared_ptr<Node> nodeCurrent = frontier_.top();
        nodeCurrent->setFrontier(false);
        frontier_.pop();

        if (nodeCurrent == s_nodeEnd) {
          solved = true;
        }

        for (auto nodeNeighbour : *nodeCurrent->getNeighbours()) {
          if (!nodeNeighbour->isVisited() && nodeNeighbour->isObstacle() == 0) {
            nodeNeighbour->setParentNode(nodeCurrent);
            nodeNeighbour->setVisited(true);
            nodeNeighbour->setFrontier(true);
            frontier_.push(nodeNeighbour);
          }
        }
      } else {
        solved = true;
      }
      ////////////////////////////

      // reset stop watch for next cycle
      lastUpdate = std::chrono::system_clock::now();
    }

    // sends an update method to the message queue using move semantics
    auto ftr = std::async(std::launch::async, &MessageQueue<bool>::send,
                          s_message_queue, std::move(solved));
    ftr.wait();

    if (solved) return;

    // sleep at every iteration to reduce CPU usage
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}
