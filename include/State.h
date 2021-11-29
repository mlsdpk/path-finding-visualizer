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

#include "LoggerPanel.h"

/*
  State Base Class
*/

namespace path_finding_visualizer {

class State {
 private:
 protected:
  std::stack<std::unique_ptr<State>> &states_;
  sf::RenderWindow *window_;
  std::shared_ptr<LoggerPanel> logger_panel_;
  sf::Vector2i mousePositionWindow_;
  bool quit_;
  bool is_reset_;
  bool is_running_;

 public:
  // Constructor
  State(sf::RenderWindow *window, std::stack<std::unique_ptr<State>> &states,
        std::shared_ptr<LoggerPanel> logger_panel);

  // Destructor
  virtual ~State();

  // Accessors
  const bool getQuit() const;

  void setReset(bool is_reset) { is_reset_ = is_reset; }
  void setRunning(bool is_running) { is_running_ = is_running; }

  // Functions
  virtual void checkForQuit();
  virtual void updateMousePosition();

  // virtual functions
  virtual void endState() = 0;
  virtual void updateKeybinds() = 0;
  virtual void update(const float &dt) = 0;
  virtual void render() = 0;
};

}  // namespace path_finding_visualizer