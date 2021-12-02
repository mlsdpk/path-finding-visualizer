#pragma once

#include <stack>

#include "States/Algorithms/GraphBased/BFS/BFS.h"

namespace path_finding_visualizer {
namespace graph_based {

class DFS : public BFS {
 public:
  // Constructor
  DFS(std::shared_ptr<gui::LoggerPanel> logger_panel);

  // Destructor
  virtual ~DFS();

  // override initialization Functions
  void initAlgorithm() override;

  // override main update function
  virtual void updatePlanner(bool &solved, Node &start_node,
                             Node &end_node) override;

 private:
  // DFS related
  std::stack<std::shared_ptr<Node>> frontier_;
};

}  // namespace graph_based
}  // namespace path_finding_visualizer