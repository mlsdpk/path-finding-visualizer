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

static const std::vector<std::string> PLANNER_NAMES{"BFS", "DFS", "DIJKSTRA",
                                                    "A*"};
enum PLANNERS_IDS { BFS, DFS, DIJKSTRA, AStar };

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
  void renderGui();
  void setPlanner(const int id);

 private:
  sf::RenderWindow* window_;
  sf::Event ev_;
  sf::Clock dtClock_;
  float dt_;
  std::stack<std::unique_ptr<State>> states_;
  std::string curr_planner_;
};

}  // namespace path_finding_visualizer