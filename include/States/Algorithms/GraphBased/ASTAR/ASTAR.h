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
  ASTAR(std::shared_ptr<gui::LoggerPanel> logger_panel);

  // Destructor
  virtual ~ASTAR();

  // Overriden functions
  virtual void initAlgorithm() override;

  // override main update function
  virtual void updatePlanner(bool &solved, Node &start_node,
                             Node &end_node) override;

 protected:
  // ASTAR related
  std::priority_queue<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>,
                      MinimumDistanceASTAR>
      frontier_;

  bool use_manhattan_heuristics_{true};
};

}  // namespace graph_based
}  // namespace path_finding_visualizer