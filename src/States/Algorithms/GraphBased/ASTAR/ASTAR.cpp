#include "ASTAR.h"

namespace path_finding_visualizer {
namespace graph_based {

// Constructor
ASTAR::ASTAR(std::shared_ptr<gui::LoggerPanel> logger_panel)
    : BFS(logger_panel) {}

// Destructor
ASTAR::~ASTAR() {}

// override initAlgorithm() function
void ASTAR::initAlgorithm() {
  // initialize ASTAR by clearing frontier and add start node
  while (!frontier_.empty()) {
    frontier_.pop();
  }

  use_manhattan_heuristics_ = (grid_connectivity_ == 0) ? true : false;

  nodeStart_->setGDistance(0.0);
  nodeStart_->setFDistance(utils::costToGoHeuristics(
      *nodeStart_, *nodeEnd_, use_manhattan_heuristics_));
  frontier_.push(nodeStart_);
}

void ASTAR::updatePlanner(bool &solved, Node &start_node, Node &end_node) {
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
                    utils::costToGoHeuristics(*node_current, *node_neighbour,
                                              use_manhattan_heuristics_);

      if (dist < node_neighbour->getGDistance()) {
        node_neighbour->setParentNode(node_current);
        node_neighbour->setGDistance(dist);

        // f = g + h
        double f_dist = node_current->getGDistance() +
                        utils::costToGoHeuristics(*node_neighbour, end_node,
                                                  use_manhattan_heuristics_);
        node_neighbour->setFDistance(f_dist);
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