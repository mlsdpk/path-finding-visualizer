#pragma once

#include <math.h>

#include <queue>

#include "Dijkstra_Node.h"
#include "Gui.h"
#include "State.h"

// custom function for returning minimum distance node
// to be used in priority queue
struct MinimumDistanceNode {
  // operator overloading
  bool operator()(const Dijkstra_Node* n1, const Dijkstra_Node* n2) const {
    return n1->getDistance() > n2->getDistance();
  }
};

class Dijkstra : public State {
 private:
  // fonts
  sf::Font font1_, font2_;

  // background related
  sf::Text titleText_;
  sf::RectangleShape cellNamesBGN_;

  // colors
  sf::Color BGN_COL, FONT_COL, IDLE_COL, HOVER_COL, ACTIVE_COL, START_COL,
      END_BORDER_COL, VISITED_COL, FRONTIER_COL, OBST_COL, PATH_COL;

  // buttons
  std::map<std::string, gui::Button*> buttons_;

  // key timers
  float keyTime_;
  float keyTimeMax_;

  // Map Variables
  unsigned int gridSize_;
  unsigned int mapWidth_;
  unsigned int mapHeight_;

  // Dijkstra related
  Dijkstra_Node* nodes_;
  Dijkstra_Node* nodeStart_;
  Dijkstra_Node* nodeEnd_;
  std::priority_queue<Dijkstra_Node*, std::vector<Dijkstra_Node*>,
                      MinimumDistanceNode>
      frontier_;

  // logic flags
  bool dijkstra_running_;
  bool dijkstra_initialized_;
  bool dijkstra_reset_;
  bool dijkstra_solved_;

  // initialization Functions
  void initFonts();
  void initColors();
  void initBackground();
  void initButtons();
  void initVariables();
  void initNodes();
  void initDijkstra();

  void updateKeyTime(const float& dt);
  const bool getKeyTime();

 public:
  // Constructor
  Dijkstra(sf::RenderWindow* window, std::stack<State*>* states);

  // Destructor
  virtual ~Dijkstra();

  // Override Functions
  void endState();
  void updateKeybinds();
  void update(const float& dt);
  void render();

  // New update functions
  void updateButtons();
  void updateNodes();

  // New render functions
  void renderBackground();
  void renderButtons();
  void renderNodes();

  // Utility function
  float L1_Distance(Dijkstra_Node* n1, Dijkstra_Node* n2);

  // Dijkstra algorithm function
  void solve_Dijkstra();
};
