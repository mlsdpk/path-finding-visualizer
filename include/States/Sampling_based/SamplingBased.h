#pragma once

#include <boost/math/constants/constants.hpp>
#include <condition_variable>
#include <future>
#include <memory>
#include <mutex>
#include <vector>

#include "Gui.h"
#include "MessageQueue.h"
#include "State.h"

struct Point {
  double x, y;
  std::shared_ptr<Point> parent{nullptr};
};

class SamplingBased : public State {
 protected:
  // fonts
  sf::Font font1_, font2_;

  // background related
  sf::Text titleText_;

  // colors
  sf::Color BGN_COL, FONT_COL, IDLE_COL, HOVER_COL, ACTIVE_COL, START_COL,
      END_COL, VISITED_COL, FRONTIER_COL, OBST_COL, PATH_COL;

  // buttons
  std::map<std::string, std::unique_ptr<gui::Button>> buttons_;

  // key timers
  float keyTime_;
  float keyTimeMax_;

  // Map related
  unsigned int obstSize_;
  unsigned int mapWidth_;
  unsigned int mapHeight_;
  std::vector<std::shared_ptr<sf::RectangleShape>> obstacles_;

  // Algorithm related
  std::shared_ptr<Point> start_point_;
  std::shared_ptr<Point> goal_point_;

  // MessageQueue Object
  std::shared_ptr<MessageQueue<bool>> message_queue_;

  // logic flags
  bool Algorithm_running_;
  bool Algorithm_initialized_;
  bool Algorithm_reset_;
  bool Algorithm_solved_;
  bool Algorithm_stopped_;

  // threads & mutex
  std::thread t_;
  bool thread_joined_;
  std::mutex mutex_;

  // initialization Functions
  void initFonts();
  void initColors();
  void initBackground(const std::string& algo_name);
  void initButtons();
  void initVariables();

  void updateKeyTime(const float& dt);
  const bool getKeyTime();

 public:
  // Constructor
  SamplingBased(sf::RenderWindow* window,
                std::stack<std::unique_ptr<State>>& states,
                std::string algo_name);

  // Destructor
  virtual ~SamplingBased();

  // Override Functions
  void endState() override;
  void updateKeybinds() override;
  void update(const float& dt) override;
  void render() override;

  // New update & render functions
  void updateButtons();
  void updateUserInput();
  void renderButtons();
  void renderObstacles();

  // new functions
  double map_(double x, double in_min, double in_max, double out_min,
              double out_max);

  // virtual functions
  // all the sampling-based planners need to override this functions

  // function for background rendering
  virtual void renderBackground() = 0;

  // rendering function for algorithm specific
  virtual void renderAlgorithm() = 0;

  // initialization function
  // this function runs at start of the program & when user presses RESET
  // buttons
  virtual void initialize() = 0;

  // algorithm related initialization
  // this function runs when user presses RUN buttons
  virtual void initAlgorithm() = 0;

  // main algorithm function (runs in separate thread)
  virtual void solveConcurrently(
      std::shared_ptr<Point> start_point, std::shared_ptr<Point> goal_point,
      std::shared_ptr<MessageQueue<bool>> message_queue) = 0;
};
