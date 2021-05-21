#pragma once

#include <queue>

#include "SearchBased.h"
#include "Gui.h"
#include "Node.h"

// custom function for returning minimum distance node
// to be used in priority queue
struct MinimumDistanceDIJKSTRA {
  // operator overloading
  bool operator()(const std::shared_ptr<Node> &n1,
                  const std::shared_ptr<Node> &n2) const {
    return n1->getGDistance() > n2->getGDistance();
  }
};

class DIJKSTRA : public SearchBased {
 private:
  // DIJKSTRA related
  std::priority_queue<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>,
                      MinimumDistanceDIJKSTRA>
      frontier_;

  // override initialization Functions
  void initAlgorithm() override;

 public:
  // Constructor
  DIJKSTRA(sf::RenderWindow *window,
           std::stack<std::unique_ptr<State>> &states);

  // Destructor
  virtual ~DIJKSTRA();

  // override update functions
  void updateNodes() override;

  // override render functions
  void renderBackground() override;
  void renderNodes() override;

  // DIJKSTRA algorithm function
  void solveConcurrently(
      std::shared_ptr<Node> nodeStart, std::shared_ptr<Node> nodeEnd,
      std::shared_ptr<MessageQueue<bool>> message_queue) override;

  // new utility function
  double L1_Distance(const std::shared_ptr<Node> &n1,
                     const std::shared_ptr<Node> &n2);
};
