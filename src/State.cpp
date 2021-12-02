#include "State.h"

namespace path_finding_visualizer {

State::State(std::shared_ptr<gui::LoggerPanel> logger_panel)
    : logger_panel_{logger_panel} {}

State::~State() {}

void State::updateMousePosition(const ImVec2& mousePos) {
  mousePositionWindow_.x = mousePos.x;
  mousePositionWindow_.y = mousePos.y;
}

}  // namespace path_finding_visualizer