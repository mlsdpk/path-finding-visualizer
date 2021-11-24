#pragma once

#include <queue>

#include "GraphBased.h"
#include "Node.h"

namespace path_finding_visualizer {
namespace graph_based {

class BFS : public GraphBased {
 public:
  // Constructor
  BFS(sf::RenderWindow *window, std::stack<std::unique_ptr<State>> &states);

  // Destructor
  virtual ~BFS();

  // override initialization Functions
  virtual void initAlgorithm() override;

  // override update functions
  virtual void updateNodes() override;

  // override render functions
  virtual void renderNodes() override;

  // BFS algorithm function
  virtual void solveConcurrently(
      std::shared_ptr<Node> nodeStart, std::shared_ptr<Node> nodeEnd,
      std::shared_ptr<MessageQueue<bool>> message_queue) override;

 private:
  // BFS related
  std::queue<std::shared_ptr<Node>> frontier_;
};

}  // namespace graph_based
}  // namespace path_finding_visualizer