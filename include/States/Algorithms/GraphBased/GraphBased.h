#pragma once

#include <imgui-SFML.h>
#include <imgui.h>

#include <condition_variable>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <vector>

#include "MessageQueue.h"
#include "State.h"
#include "States/Algorithms/GraphBased/Node.h"

namespace path_finding_visualizer {
namespace graph_based {

class GraphBased : public State {
 public:
  // Constructor
  GraphBased(sf::RenderWindow* window,
             std::stack<std::unique_ptr<State>>& states);

  // Destructor
  virtual ~GraphBased();

  // Override Functions
  void endState() override;
  void updateKeybinds() override;
  void update(const float& dt) override;
  void render() override;

  // virtual functions
  virtual void renderGui();
  virtual void renderNodes() = 0;
  virtual void updateNodes() = 0;
  virtual void initAlgorithm() = 0;
  virtual void solveConcurrently(
      std::shared_ptr<Node> nodeStart, std::shared_ptr<Node> nodeEnd,
      std::shared_ptr<MessageQueue<bool>> message_queue) = 0;

 protected:
  // colors
  sf::Color BGN_COL, FONT_COL, IDLE_COL, HOVER_COL, ACTIVE_COL, START_COL,
      END_COL, VISITED_COL, FRONTIER_COL, OBST_COL, PATH_COL;

  // key timers
  float keyTime_;
  float keyTimeMax_;

  // Map Variables
  int gridSize_;
  int slider_grid_size_;
  unsigned int mapWidth_;
  unsigned int mapHeight_;

  // Algorithm related
  std::string algo_name_;
  std::vector<std::shared_ptr<Node>> nodes_;
  std::shared_ptr<Node> nodeStart_;
  std::shared_ptr<Node> nodeEnd_;

  // MessageQueue Object
  std::shared_ptr<MessageQueue<bool>> message_queue_;

  // logic flags
  bool Algorithm_running_;
  bool Algorithm_initialized_;
  bool Algorithm_reset_;
  bool Algorithm_solved_;

  // threads
  std::thread t_;
  bool thread_joined_;

  // initialization Functions
  void initColors();
  void initVariables();
  void initNodes();

  void updateKeyTime(const float& dt);
  const bool getKeyTime();
};

}  // namespace graph_based
}  // namespace path_finding_visualizer
