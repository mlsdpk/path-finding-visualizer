#pragma once

#include <stack>

#include "SearchBased.h"
#include "Gui.h"
#include "Node.h"

class DFS : public SearchBased {
 private:
  // DFS related
  std::stack<std::shared_ptr<Node>> frontier_;

  // override initialization Functions
  void initAlgorithm() override;

 public:
  // Constructor
  DFS(sf::RenderWindow *window, std::stack<std::unique_ptr<State>> &states);

  // Destructor
  virtual ~DFS();

  // override update functions
  void updateNodes() override;

  // override render functions
  void renderBackground() override;
  void renderNodes() override;

  // DFS algorithm function
  void solveConcurrently(
      std::shared_ptr<Node> nodeStart, std::shared_ptr<Node> nodeEnd,
      std::shared_ptr<MessageQueue<bool>> message_queue) override;
};
