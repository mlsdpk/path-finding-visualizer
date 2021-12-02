#include "DIJKSTRA.h"

namespace path_finding_visualizer {
namespace graph_based {

// Constructor
DIJKSTRA::DIJKSTRA(std::shared_ptr<gui::LoggerPanel> logger_panel)
    : BFS(logger_panel) {}

// Destructor
DIJKSTRA::~DIJKSTRA() {}

// override initAlgorithm() function
void DIJKSTRA::initAlgorithm() {
  // initialize DIJKSTRA by clearing frontier and add start node
  while (!frontier_.empty()) {
    frontier_.pop();
  }
  nodeStart_->setGDistance(0.0);
  frontier_.push(nodeStart_);
}

void DIJKSTRA::updatePlanner(bool &solved, Node &start_node, Node &end_node) {
  if (!frontier_.empty()) {
    std::shared_ptr<Node> node_current = frontier_.top();
    node_current->setFrontier(false);
    node_current->setVisited(true);
    frontier_.pop();

    if (node_current->isGoal()) {
      solved = true;
    }

    for (auto node_neighbour : *node_current->getNeighbours()) {
      if (node_neighbour->isVisited() || node_neighbour->isObstacle()) {
        continue;
      }

      double dist = node_current->getGDistance() +
                    utils::distanceCost(*node_current, *node_neighbour);

      if (dist < node_neighbour->getGDistance()) {
        node_neighbour->setParentNode(node_current);
        node_neighbour->setGDistance(dist);

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