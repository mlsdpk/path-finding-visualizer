#pragma once

#include <imgui-SFML.h>
#include <imgui.h>

#include <boost/math/constants/constants.hpp>
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
  SamplingBased(sf::RenderWindow *window,
                std::stack<std::unique_ptr<State>> &states);

  // Destructor
  virtual ~SamplingBased();

  // Override Functions
  void endState() override;
  void updateKeybinds() override;
  void update(const float &dt) override;
  void render() override;

  void updateUserInput();
  void renderObstacles();
  void clearObstacles();
  void initVariables();
  void updateKeyTime(const float &dt);
  const bool getKeyTime();

  virtual void renderGui();

  // virtual functions
  // all the sampling-based planners need to override this functions

  // rendering function for algorithm specific
  virtual void renderPlannerData() = 0;

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

  // main algorithm function (runs in separate thread)
  virtual void solveConcurrently(
      std::shared_ptr<Vertex> start_point, std::shared_ptr<Vertex> goal_point,
      std::shared_ptr<MessageQueue<bool>> message_queue) = 0;

 protected:
  // key timers
  float key_time_;
  float key_time_max_;

  // Map related
  unsigned int obst_size_;
  unsigned int map_width_;
  unsigned int map_height_;
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

  // MessageQueue Object
  std::shared_ptr<MessageQueue<bool>> message_queue_;

  // logic flags
  bool is_running_;
  bool is_initialized_;
  bool is_reset_;
  bool is_solved_;
  bool is_stopped_;

  // threads & mutex
  std::thread t_;
  bool thread_joined_;
  std::mutex mutex_;
};

}  // namespace sampling_based
}  // namespace path_finding_visualizer