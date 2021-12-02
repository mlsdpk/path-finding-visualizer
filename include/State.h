#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <stack>
#include <string>
#include <vector>

#include "Gui.h"

/*
  State Base Class
*/

namespace path_finding_visualizer {

class State {
 private:
 protected:
  std::shared_ptr<gui::LoggerPanel> logger_panel_;
  sf::Vector2f mousePositionWindow_;
  bool is_reset_;
  bool is_running_;

 public:
  // Constructor
  State(std::shared_ptr<gui::LoggerPanel> logger_panel);

  // Destructor
  virtual ~State();

  void setReset(bool is_reset) { is_reset_ = is_reset; }
  void setRunning(bool is_running) { is_running_ = is_running; }

  // Functions
  void updateMousePosition(const ImVec2 &mousePos);

  // virtual functions
  virtual void endState() = 0;
  virtual void update(const float &dt, const ImVec2 &mousePos) = 0;
  virtual void renderConfig() = 0;
  virtual void renderScene(sf::RenderTexture &render_texture) = 0;
};

}  // namespace path_finding_visualizer