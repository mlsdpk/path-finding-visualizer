#include "DFS.h"

namespace path_finding_visualizer {
namespace graph_based {

// Constructor
DFS::DFS(std::shared_ptr<gui::LoggerPanel> logger_panel) : BFS(logger_panel) {}

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

void DFS::updatePlanner(bool &solved, Node &start_node, Node &end_node) {
  if (!frontier_.empty()) {
    std::shared_ptr<Node> node_current = frontier_.top();
    node_current->setFrontier(false);
    frontier_.pop();

    if (node_current->isGoal()) {
      solved = true;
    }

    for (auto node_neighbour : *node_current->getNeighbours()) {
      if (!node_neighbour->isVisited() && node_neighbour->isObstacle() == 0) {
        node_neighbour->setParentNode(node_current);
        node_neighbour->setVisited(true);
        node_neighbour->setFrontier(true);
        frontier_.push(node_neighbour);
      }
    }
  } else {
    solved = true;
  }
}

}  // namespace graph_based
}  // namespace path_finding_visualizer