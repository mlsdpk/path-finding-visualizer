#pragma once

#include <imgui-SFML.h>
#include <imgui.h>

#include <condition_variable>
#include <future>
#include <memory>
#include <mutex>
#include <random>
#include <vector>

#include "MessageQueue.h"
#include "State.h"
#include "States/Algorithms/SamplingBased/Utils.h"

namespace path_finding_visualizer {
namespace sampling_based {

static const sf::Color OBST_COL = sf::Color(0, 0, 0, 255);
static const sf::Color START_COL = sf::Color(0, 255, 0, 255);
static const sf::Color GOAL_COL = sf::Color(255, 0, 0, 255);
static const sf::Color EDGE_COL = sf::Color(0, 0, 255, 255);
static const sf::Color PATH_COL = sf::Color(255, 0, 255, 255);

struct Vertex {
  double x, y;
  std::shared_ptr<Vertex> parent{nullptr};
};

class SamplingBased : public State {
 public:
  // Constructor
  SamplingBased(std::shared_ptr<gui::LoggerPanel> logger_panel,
                const std::string &name);

  // Destructor
  virtual ~SamplingBased();

  // Override Functions
  void endState() override;
  void update(const float &dt, const ImVec2 &mousePos) override;
  void renderConfig() override;
  void renderScene(sf::RenderTexture &render_texture) override;

  void updateUserInput();
  void renderMap(sf::RenderTexture &render_texture);
  void renderObstacles(sf::RenderTexture &render_texture);
  void clearObstacles();
  void initMapVariables();
  void initVariables();
  void updateKeyTime(const float &dt);
  const bool getKeyTime();

  virtual void renderGui();

  // virtual functions
  // all the sampling-based planners need to override this functions

  // rendering function for algorithm specific
  virtual void renderPlannerData(sf::RenderTexture &render_texture) = 0;

  // render planner specific parameters
  virtual void renderParametersGui() = 0;

  // initialization function
  // this function runs at start of the program & when user presses RESET
  // buttons
  virtual void initialize() = 0;

  // planner related initialization
  // this function runs when user presses RUN buttons
  virtual void initPlanner() = 0;
  virtual void initParameters() = 0;

  // pure virtual function need to be implemented by (single-query)
  // sampling-based planners
  virtual void updatePlanner(bool &solved, Vertex &start, Vertex &goal) = 0;

  // main algorithm function (runs in separate thread)
  void solveConcurrently(std::shared_ptr<Vertex> start_point,
                         std::shared_ptr<Vertex> goal_point,
                         std::shared_ptr<MessageQueue<bool>> message_queue);

 protected:
  // key timers
  float key_time_;
  float key_time_max_;

  // Map related
  sf::Vector2f init_grid_xy_;
  unsigned int obst_size_;
  int map_width_;
  int map_height_;
  std::vector<std::shared_ptr<sf::RectangleShape>> obstacles_;

  /**
   * @brief Random number generator
   */
  std::mt19937 rn_gen_;

  // planner related
  // vertices & edges
  std::vector<std::shared_ptr<Vertex>> vertices_;
  std::vector<std::pair<std::shared_ptr<Vertex>, std::shared_ptr<Vertex>>>
      edges_;
  std::shared_ptr<Vertex> start_vertex_;
  std::shared_ptr<Vertex> goal_vertex_;

  /**
   * @brief Maximum number of iterations to run the algorithm
   */
  int max_iterations_;

  std::mutex iter_no_mutex_;
  unsigned int curr_iter_no_{0u};

  // MessageQueue Object
  std::shared_ptr<MessageQueue<bool>> message_queue_;

  // logic flags
  bool is_initialized_;
  bool is_solved_;
  bool is_stopped_;
  bool disable_run_;
  bool disable_gui_parameters_;

  // threads & mutex
  std::thread t_;
  bool thread_joined_;
  std::mutex mutex_;
};

}  // namespace sampling_based
}  // namespace path_finding_visualizer