#pragma once

#include <queue>

#include "SearchBased.h"
#include "Gui.h"
#include "Node.h"

// custom function for returning minimum distance node
// to be used in priority queue
struct MinimumDistanceASTAR {
  // operator overloading
  bool operator()(const std::shared_ptr<Node> &n1,
                  const std::shared_ptr<Node> &n2) const {
    return n1->getFDistance() > n2->getFDistance();
  }
};

class ASTAR : public SearchBased {
 private:
  // ASTAR related
  std::priority_queue<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>,
                      MinimumDistanceASTAR>
      frontier_;

  // override initialization Functions
  void initAlgorithm() override;

 public:
  // Constructor
  ASTAR(sf::RenderWindow *window, std::stack<std::unique_ptr<State>> &states);

  // Destructor
  virtual ~ASTAR();

  // override update functions
  void updateNodes() override;

  // override render functions
  void renderBackground() override;
  void renderNodes() override;

  // ASTAR algorithm function
  void solveConcurrently(
      std::shared_ptr<Node> nodeStart, std::shared_ptr<Node> nodeEnd,
      std::shared_ptr<MessageQueue<bool>> message_queue) override;

  // new utility function
  double L1_Distance(const std::shared_ptr<Node> &n1,
                     const std::shared_ptr<Node> &n2);
};
