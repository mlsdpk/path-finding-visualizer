#pragma once

#include <imgui-SFML.h>
#include <imgui.h>

#include <condition_variable>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <vector>

#include "Gui.h"
#include "MessageQueue.h"
#include "State.h"
#include "States/Algorithms/GraphBased/Node.h"
#include "States/Algorithms/GraphBased/Utils.h"

namespace path_finding_visualizer {
namespace graph_based {

class GraphBased : public State {
 public:
  // Constructor
  GraphBased(std::shared_ptr<gui::LoggerPanel> logger_panel);

  // Destructor
  virtual ~GraphBased();

  // Override Functions
  void endState() override;
  void update(const float& dt, const ImVec2& mousePos) override;
  void renderConfig() override;
  void renderScene(sf::RenderTexture& render_texture) override;

  // virtual functions
  virtual void clearObstacles();
  virtual void renderGui();
  // render planner specific parameters
  virtual void renderParametersGui() = 0;
  virtual void renderNodes(sf::RenderTexture& render_texture) = 0;
  virtual void updateNodes() = 0;
  virtual void initAlgorithm() = 0;
  // pure virtual function need to be implemented by graph-based planners
  virtual void updatePlanner(bool& solved, Node& node_start,
                             Node& node_end) = 0;

  void solveConcurrently(std::shared_ptr<Node> nodeStart,
                         std::shared_ptr<Node> nodeEnd,
                         std::shared_ptr<MessageQueue<bool>> message_queue);
  void updateKeyTime(const float& dt);
  const bool getKeyTime();

 protected:
  // initialization Functions
  void initColors();
  void initVariables();
  void initGridMapParams();
  void initNodes(bool reset = true, bool reset_neighbours_only = false);

  // colors
  sf::Color BGN_COL, FONT_COL, IDLE_COL, HOVER_COL, ACTIVE_COL, START_COL,
      END_COL, VISITED_COL, FRONTIER_COL, OBST_COL, PATH_COL;

  // key timers
  float keyTime_;
  float keyTimeMax_;

  // Map Variables
  int no_of_grid_rows_;
  int no_of_grid_cols_;
  int grid_size_;
  int ui_grid_size_;
  sf::Vector2f init_grid_xy_;
  // 0 = 4 connected grid, 1 = 8 connected grid
  int grid_connectivity_;
  unsigned int map_width_;
  unsigned int map_height_;

  // Algorithm related
  std::string algo_name_;
  std::vector<std::shared_ptr<Node>> nodes_;
  std::shared_ptr<Node> nodeStart_;
  std::shared_ptr<Node> nodeEnd_;

  // MessageQueue Object
  std::shared_ptr<MessageQueue<bool>> message_queue_;

  // logic flags
  bool is_initialized_;
  bool is_solved_;
  bool disable_run_;
  bool disable_gui_parameters_;

  // threads
  std::thread t_;
  bool thread_joined_;
};

}  // namespace graph_based
}  // namespace path_finding_visualizer
