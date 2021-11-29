#pragma once

#include <queue>

#include "States/Algorithms/GraphBased/BFS/BFS.h"
#include "States/Algorithms/GraphBased/Utils.h"

namespace path_finding_visualizer {
namespace graph_based {

// custom function for returning minimum distance node
// to be used in priority queue
struct MinimumDistanceDIJKSTRA {
  // operator overloading
  bool operator()(const std::shared_ptr<Node> &n1,
                  const std::shared_ptr<Node> &n2) const {
    return n1->getGDistance() > n2->getGDistance();
  }
};

class DIJKSTRA : public BFS {
 public:
  // Constructor
  DIJKSTRA(sf::RenderWindow *window, std::stack<std::unique_ptr<State>> &states,
           std::shared_ptr<LoggerPanel> logger_panel);

  // Destructor
  virtual ~DIJKSTRA();

  // Overriden functions
  virtual void initAlgorithm() override;
  void solveConcurrently(
      std::shared_ptr<Node> nodeStart, std::shared_ptr<Node> nodeEnd,
      std::shared_ptr<MessageQueue<bool>> message_queue) override;

 protected:
  // DIJKSTRA related
  std::priority_queue<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>,
                      MinimumDistanceDIJKSTRA>
      frontier_;
};

}  // namespace graph_based
}  // namespace path_finding_visualizer