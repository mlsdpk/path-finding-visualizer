#include "DFS.h"

namespace path_finding_visualizer {
namespace graph_based {

// Constructor
DFS::DFS(sf::RenderWindow* window, std::stack<std::unique_ptr<State>>& states)
    : BFS(window, states) {}

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

}  // namespace graph_based
}  // namespace path_finding_visualizer