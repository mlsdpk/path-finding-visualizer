#pragma once

#include <queue>

#include "States/Algorithms/GraphBased/BFS/BFS.h"
#include "States/Algorithms/GraphBased/Utils.h"

namespace path_finding_visualizer {
namespace graph_based {

// custom function for returning minimum distance node
// to be used in priority queue
struct MinimumDistanceASTAR {
  // operator overloading
  bool operator()(const std::shared_ptr<Node> &n1,
                  const std::shared_ptr<Node> &n2) const {
    return n1->getFDistance() > n2->getFDistance();
  }
};

class ASTAR : public BFS {
 public:
  // Constructor
  ASTAR(sf::RenderWindow *window, std::stack<std::unique_ptr<State>> &states);

  // Destructor
  virtual ~ASTAR();

  // Overriden functions
  virtual void initAlgorithm() override;
  virtual void solveConcurrently(
      std::shared_ptr<Node> nodeStart, std::shared_ptr<Node> nodeEnd,
      std::shared_ptr<MessageQueue<bool>> message_queue) override;

 protected:
  // ASTAR related
  std::priority_queue<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>,
                      MinimumDistanceASTAR>
      frontier_;

  bool use_manhattan_heuristics_{true};
};

}  // namespace graph_based
}  // namespace path_finding_visualizer