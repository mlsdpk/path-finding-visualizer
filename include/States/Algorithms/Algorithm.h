#pragma once

#include <condition_variable>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <vector>

#include "Gui.h"
#include "MessageQueue.h"
#include "Node.h"
#include "State.h"

class Algorithm : public State {
 protected:
  // fonts
  sf::Font font1_, font2_;

  // background related
  sf::Text titleText_;
  sf::RectangleShape cellNamesBGN_;

  // colors
  sf::Color BGN_COL, FONT_COL, IDLE_COL, HOVER_COL, ACTIVE_COL, START_COL,
      END_BORDER_COL, VISITED_COL, FRONTIER_COL, OBST_COL, PATH_COL;

  // buttons
  std::map<std::string, std::unique_ptr<gui::Button>> buttons_;

  // key timers
  float keyTime_;
  float keyTimeMax_;

  // Map Variables
  unsigned int gridSize_;
  unsigned int mapWidth_;
  unsigned int mapHeight_;

  // Algorithm related
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
  void initFonts();
  void initColors();
  void initBackground(const std::string& algo_name);
  void initButtons();
  void initVariables();
  void initNodes();

  void updateKeyTime(const float& dt);
  const bool getKeyTime();

 public:
  // Constructor
  Algorithm(sf::RenderWindow* window,
            std::stack<std::unique_ptr<State>>& states, std::string algo_name);

  // Destructor
  virtual ~Algorithm();

  // Override Functions
  void endState() override;
  void updateKeybinds() override;
  void update(const float& dt) override;
  void render() override;

  // New update functions
  void updateButtons();

  // New render functions
  void renderBackground();
  void renderButtons();

  // virtual functions
  virtual void renderNodes() = 0;
  virtual void updateNodes() = 0;
  virtual void initAlgorithm() = 0;
  virtual void solveConcurrently(
      std::shared_ptr<Node> nodeStart, std::shared_ptr<Node> nodeEnd,
      std::shared_ptr<MessageQueue<bool>> message_queue) = 0;
};
