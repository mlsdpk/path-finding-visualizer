#pragma once

#include <imgui-SFML.h>
#include <imgui.h>

#include <iostream>
#include <map>
#include <stack>
#include <string>
#include <vector>

#include "State.h"

namespace path_finding_visualizer {

static const std::vector<std::string> GRAPH_BASED_PLANNERS{"BFS", "DFS",
                                                           "DIJKSTRA", "A*"};
static const std::vector<std::string> SAMPLING_BASED_PLANNERS{"RRT", "RRT*"};
enum GRAPH_BASED_PLANNERS_IDS { BFS, DFS, DIJKSTRA, AStar };
enum SAMPLING_BASED_PLANNERS_IDS { RRT, RRT_STAR };

class Game {
 public:
  // Constructors
  Game(sf::RenderWindow* window);

  // Destructors
  virtual ~Game();

  // Accessors
  const bool running() const;

  // Functions
  void pollEvents();
  void updateDt();
  void update();
  void render();
  void initGuiTheme();
  void renderMenuBar(ImGuiIO& io);
  void setGraphBasedPlanner(const int id);
  void setSamplingBasedPlanner(const int id);
  void showHowToUseWindow();
  void showAboutWindow();

 private:
  sf::RenderWindow* window_;
  sf::Event ev_;
  sf::Clock dtClock_;
  float dt_;
  std::stack<std::unique_ptr<State>> states_;
  std::string curr_planner_;
  std::shared_ptr<LoggerPanel> logger_panel_;
  bool disable_run_;
  bool show_how_to_use_window_{false};
  bool show_about_window_{false};
};

}  // namespace path_finding_visualizer