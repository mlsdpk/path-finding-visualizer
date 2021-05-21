#pragma once

#include <queue>

#include "Gui.h"
#include "Node.h"
#include "SearchBased.h"

class BFS : public SearchBased {
 private:
  // BFS related
  std::queue<std::shared_ptr<Node>> frontier_;

  // override initialization Functions
  void initAlgorithm() override;

 public:
  // Constructor
  BFS(sf::RenderWindow *window, std::stack<std::unique_ptr<State>> &states);

  // Destructor
  virtual ~BFS();

  // override update functions
  void updateNodes() override;

  // override render functions
  void renderBackground() override;
  void renderNodes() override;

  // BFS algorithm function
  void solveConcurrently(
      std::shared_ptr<Node> nodeStart, std::shared_ptr<Node> nodeEnd,
      std::shared_ptr<MessageQueue<bool>> message_queue) override;
};
