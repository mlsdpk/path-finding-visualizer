#include "GraphBased.h"

namespace path_finding_visualizer {
namespace graph_based {

// Constructor
GraphBased::GraphBased(sf::RenderWindow* window,
                       std::stack<std::unique_ptr<State>>& states)
    : State(window, states), keyTimeMax_{1.f}, keyTime_{0.f} {
  initVariables();
  initNodes();
  initColors();
}

// Destructor
GraphBased::~GraphBased() {
  if (!thread_joined_) {
    t_.join();
  }
}

void GraphBased::initVariables() {
  // these variables depend on the visualizer
  // for now, just use these and can improve it later
  slider_grid_size_ = 20;
  gridSize_ = slider_grid_size_;
  grid_connectivity_ = 0;
  mapWidth_ = 700;
  mapHeight_ = 700;

  message_queue_ = std::make_shared<MessageQueue<bool>>();

  is_running_ = false;
  is_initialized_ = false;
  is_reset_ = false;
  is_solved_ = false;
  thread_joined_ = true;
  disable_run_ = false;
  disable_gui_parameters_ = false;
}

void GraphBased::initColors() {
  BGN_COL = sf::Color(246, 229, 245, 255);
  FONT_COL = sf::Color(78, 95, 131, 255);
  IDLE_COL = sf::Color(251, 244, 249, 255);
  HOVER_COL = sf::Color(245, 238, 243, 255);
  ACTIVE_COL = sf::Color(232, 232, 232);
  START_COL = sf::Color(67, 246, 130, 255);
  END_COL = sf::Color(242, 103, 101);
  VISITED_COL = sf::Color(198, 202, 229, 255);
  FRONTIER_COL = sf::Color(242, 204, 209, 255);
  OBST_COL = sf::Color(186, 186, 186, 255);
  PATH_COL = sf::Color(190, 242, 227, 255);
}

void GraphBased::initNodes(bool reset, bool reset_neighbours_only) {
  if (reset) {
    nodes_.clear();

    for (int i = 0; i < (mapWidth_ / gridSize_) * (mapHeight_ / gridSize_);
         i++) {
      nodes_.emplace_back(std::make_shared<Node>());
    }
  }

  // set all nodes to free obsts and respective positions
  for (int x = 0; x < mapHeight_ / gridSize_; x++) {
    for (int y = 0; y < mapWidth_ / gridSize_; y++) {
      int nodeIndex = (mapWidth_ / gridSize_) * x + y;
      if (reset) {
        nodes_[nodeIndex]->setPosition(sf::Vector2i(x, y));
        nodes_[nodeIndex]->setObstacle(false);
      }
      nodes_[nodeIndex]->setVisited(false);
      nodes_[nodeIndex]->setFrontier(false);
      nodes_[nodeIndex]->setPath(false);
      nodes_[nodeIndex]->setParentNode(nullptr);
      nodes_[nodeIndex]->setGDistance(INFINITY);
      nodes_[nodeIndex]->setFDistance(INFINITY);
    }
  }

  // add neighbours based on 4 or 8 connectivity grid
  if (reset || reset_neighbours_only) {
    for (int x = 0; x < mapHeight_ / gridSize_; x++) {
      for (int y = 0; y < mapWidth_ / gridSize_; y++) {
        int nodeIndex = (mapWidth_ / gridSize_) * x + y;
        nodes_[nodeIndex]->clearNeighbours();
        utils::addNeighbours(nodes_, nodeIndex,
                             static_cast<unsigned>(mapWidth_ / gridSize_),
                             static_cast<unsigned>(mapHeight_ / gridSize_),
                             [](int connectivity) {
                               return (connectivity == 1) ? true : false;
                             }(grid_connectivity_));
      }
    }
  }

  if (reset) {
    // initialize Start and End nodes ptrs (upper left and lower right corners)
    nodeStart_ = nodes_[(mapWidth_ / gridSize_) * 0 + 0];
    nodeStart_->setParentNode(nullptr);
    nodeEnd_ = nodes_[(mapWidth_ / gridSize_) * (mapHeight_ / gridSize_ - 1) +
                      (mapWidth_ / gridSize_ - 1)];
  }
}

void GraphBased::endState() {}

void GraphBased::updateKeybinds() { checkForQuit(); }

/**
 * Getter function for Algorithm key timer.
 *
 * @param none.
 * @return true if keytime > keytime_max else false.
 */
const bool GraphBased::getKeyTime() {
  if (keyTime_ >= keyTimeMax_) {
    keyTime_ = 0.f;
    return true;
  }
  return false;
}

/**
 * Update the current time of key timer.
 *
 * @param dt delta time.
 * @return void.
 */
void GraphBased::updateKeyTime(const float& dt) {
  if (keyTime_ < keyTimeMax_) {
    keyTime_ += 5.f * dt;
  }
}

void GraphBased::update(const float& dt) {
  // from base classes
  updateKeyTime(dt);
  updateMousePosition();
  updateKeybinds();
  // updateButtons();

  if (is_reset_) {
    initNodes(false, false);
    is_running_ = false;
    is_initialized_ = false;
    is_reset_ = false;
    is_solved_ = false;

    message_queue_ = std::make_shared<MessageQueue<bool>>();
  }

  if (is_running_) {
    is_reset_ = false;

    // initialize Algorithm
    if (!is_initialized_) {
      initAlgorithm();

      // create thread
      // solve the algorithm concurrently
      t_ = std::thread(&GraphBased::solveConcurrently, this, nodeStart_,
                       nodeEnd_, message_queue_);

      thread_joined_ = false;
      is_initialized_ = true;
    }

    // check the algorithm is solved or not
    auto msg = message_queue_->receive();
    // if solved
    if (msg) {
      t_.join();
      thread_joined_ = true;
      is_running_ = false;
      is_solved_ = true;
    }
  } else {
    // only allow mouse and key inputs
    // if the algorithm is not running

    // virtual function updateNodes()
    updateNodes();
  }
}

void GraphBased::clearObstacles() {
  for (int x = 0; x < mapHeight_ / gridSize_; x++) {
    for (int y = 0; y < mapWidth_ / gridSize_; y++) {
      int nodeIndex = (mapWidth_ / gridSize_) * x + y;
      nodes_[nodeIndex]->setObstacle(false);
    }
  }
}

void GraphBased::renderGui() {
  // buttons
  {
    // RESET button
    {
      if (!disable_run_ || is_running_) ImGui::BeginDisabled();
      bool clicked = ImGui::Button("RESET", ImVec2(100.f, 40.f));
      if (!disable_run_ || is_running_) ImGui::EndDisabled();
      if (clicked && !is_running_) {
        is_reset_ = true;
        disable_gui_parameters_ = false;
        disable_run_ = false;
      }
    }

    ImGui::SameLine();

    // TODO: PAUSE button
    // always disabled (not implemented yet)
    {
      if (true) ImGui::BeginDisabled();
      bool clicked = ImGui::Button("PAUSE", ImVec2(100.f, 40.f));
      if (true) ImGui::EndDisabled();
    }

    ImGui::SameLine();

    // RUN button
    {
      if (disable_run_) ImGui::BeginDisabled();
      bool clicked = ImGui::Button("RUN", ImVec2(100.f, 40.f));
      if (disable_run_) ImGui::EndDisabled();
      if (clicked && !is_solved_) {
        is_running_ = true;
        disable_gui_parameters_ = true;
        disable_run_ = true;
      }
    }
  }

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  if (disable_gui_parameters_) ImGui::BeginDisabled();

  // grid size slider
  if (ImGui::SliderInt("Grid Size", &slider_grid_size_, 10, 100)) {
    gridSize_ = slider_grid_size_;
    initNodes(true, false);
  }

  ImGui::Spacing();

  // radio buttons for choosing 4 or 8 connected grids
  {
    bool a, b;
    a = ImGui::RadioButton("4-connected", &grid_connectivity_, 0);
    ImGui::SameLine();
    b = ImGui::RadioButton("8-connected", &grid_connectivity_, 1);
    if (a || b) {
      initNodes(false, true);
    }
  }

  ImGui::Spacing();
  // virtual function renderParametersGui()
  // need to be implemented by derived class
  renderParametersGui();
  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();
  {
    if (ImGui::Button("CLEAR OBSTACLES", ImVec2(154.f, 40.f))) {
      clearObstacles();
    }
    ImGui::SameLine();
    if (ImGui::Button("RESET PARAMETERS", ImVec2(154.f, 40.f))) {
    }
  }

  if (disable_gui_parameters_) ImGui::EndDisabled();
}

void GraphBased::render() {
  // virtual function renderNodes()
  // need to be implemented by derived class
  renderNodes();

  // render gui
  renderGui();
}

}  // namespace graph_based
}  // namespace path_finding_visualizer