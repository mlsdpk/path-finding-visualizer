#pragma once

#include <stack>

#include "States/Algorithms/GraphBased/BFS/BFS.h"

namespace path_finding_visualizer {
namespace graph_based {

class DFS : public BFS {
 public:
  // Constructor
  DFS(sf::RenderWindow *window, std::stack<std::unique_ptr<State>> &states,
      std::shared_ptr<LoggerPanel> logger_panel);

  // Destructor
  virtual ~DFS();

  // override initialization Functions
  void initAlgorithm() override;

  // DFS algorithm function
  void solveConcurrently(
      std::shared_ptr<Node> nodeStart, std::shared_ptr<Node> nodeEnd,
      std::shared_ptr<MessageQueue<bool>> message_queue) override;

 private:
  // DFS related
  std::stack<std::shared_ptr<Node>> frontier_;
};

}  // namespace graph_based
}  // namespace path_finding_visualizer