#include "States/Algorithms/GraphBased/BFS/BFS.h"

namespace path_finding_visualizer {
namespace graph_based {

// Constructor
BFS::BFS(sf::RenderWindow* window, std::stack<std::unique_ptr<State>>& states)
    : GraphBased(window, states) {}

// Destructor
BFS::~BFS() {}

// override initAlgorithm() function
void BFS::initAlgorithm() {
  // initialize BFS by clearing frontier and add start node
  while (!frontier_.empty()) {
    frontier_.pop();
  }

  frontier_.push(nodeStart_);
}

// override updateNodes() function
void BFS::updateNodes() {
  if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && getKeyTime()) {
    int localY = ((mousePositionWindow_.x - 350) / gridSize_);
    int localX = ((mousePositionWindow_.y - 18) / gridSize_);

    if (localX >= 0 && localX < mapHeight_ / gridSize_) {
      if (localY >= 0 && localY < mapWidth_ / gridSize_) {
        // get the selected node
        std::shared_ptr<Node> selectedNode =
            nodes_[(mapWidth_ / gridSize_) * localX + localY];

        // check the position is Obstacle free or not
        bool isObstacle = false;
        if (selectedNode->isObstacle()) {
          isObstacle = true;
        }

        if (!Algorithm_solved_) {
          if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
            if (!isObstacle) {
              if (selectedNode != nodeEnd_) nodeStart_ = selectedNode;
            }
          } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
            if (!isObstacle) {
              if (selectedNode != nodeStart_) nodeEnd_ = selectedNode;
            }
          } else {
            selectedNode->setObstacle(!isObstacle);
          }
        } else {
          if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
            if (!isObstacle) {
              if (selectedNode != nodeStart_) nodeEnd_ = selectedNode;
            }
          }
        }
      }
    }
  }
}

// override renderNodes() function
void BFS::renderNodes() {
  for (int x = 0; x < mapHeight_ / gridSize_; x++) {
    for (int y = 0; y < mapWidth_ / gridSize_; y++) {
      float size = static_cast<float>(gridSize_);
      sf::RectangleShape rectangle(sf::Vector2f(size, size));
      rectangle.setOutlineThickness(2.f);
      rectangle.setOutlineColor(BGN_COL);
      rectangle.setPosition(350 + y * size, 18 + x * size);

      int nodeIndex = (mapWidth_ / gridSize_) * x + y;

      if (nodes_[nodeIndex]->isObstacle()) {
        rectangle.setFillColor(OBST_COL);
      } else if (nodes_[nodeIndex]->isPath()) {
        rectangle.setFillColor(PATH_COL);
        nodes_[nodeIndex]->setPath(false);
      } else if (nodes_[nodeIndex]->isFrontier()) {
        rectangle.setFillColor(FRONTIER_COL);
      } else if (nodes_[nodeIndex]->isVisited()) {
        rectangle.setFillColor(VISITED_COL);
      } else {
        rectangle.setFillColor(IDLE_COL);
      }

      if (nodes_[nodeIndex] == nodeStart_) {
        rectangle.setFillColor(START_COL);
      } else if (nodes_[nodeIndex] == nodeEnd_) {
        rectangle.setFillColor(END_COL);
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

void BFS::renderParametersGui() {}

void BFS::solveConcurrently(std::shared_ptr<Node> nodeStart,
                            std::shared_ptr<Node> nodeEnd,
                            std::shared_ptr<MessageQueue<bool>> message_queue) {
  // copy assignment
  // thread-safe due to shared_ptrs
  std::shared_ptr<Node> s_nodeStart = nodeStart;
  std::shared_ptr<Node> s_nodeEnd = nodeEnd;
  std::shared_ptr<MessageQueue<bool>> s_message_queue = message_queue;

  bool solved = false;

  double cycleDuration = 1.0;  // duration of a single simulation cycle in ms
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
        std::shared_ptr<Node> nodeCurrent = frontier_.front();
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

}  // namespace graph_based
}  // namespace path_finding_visualizer