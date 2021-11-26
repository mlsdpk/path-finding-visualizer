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
  mapWidth_ = 700;
  mapHeight_ = 700;

  message_queue_ = std::make_shared<MessageQueue<bool>>();

  Algorithm_running_ = false;
  Algorithm_initialized_ = false;
  Algorithm_reset_ = false;
  Algorithm_solved_ = false;
  thread_joined_ = true;
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

void GraphBased::initNodes() {
  nodes_.clear();

  for (int i = 0; i < (mapWidth_ / gridSize_) * (mapHeight_ / gridSize_); i++) {
    nodes_.emplace_back(std::make_shared<Node>());
  }

  // set all nodes to free obsts and respective positions
  for (int x = 0; x < mapHeight_ / gridSize_; x++) {
    for (int y = 0; y < mapWidth_ / gridSize_; y++) {
      int nodeIndex = (mapWidth_ / gridSize_) * x + y;
      nodes_[nodeIndex]->setPosition(sf::Vector2i(x, y));
      nodes_[nodeIndex]->setObstacle(false);
      nodes_[nodeIndex]->setVisited(false);
      nodes_[nodeIndex]->setFrontier(false);
      nodes_[nodeIndex]->setPath(false);
      nodes_[nodeIndex]->setParentNode(nullptr);
      nodes_[nodeIndex]->setGDistance(INFINITY);
      nodes_[nodeIndex]->setFDistance(INFINITY);
    }
  }

  // add all neighbours to respective nodes
  for (int x = 0; x < mapHeight_ / gridSize_; x++) {
    for (int y = 0; y < mapWidth_ / gridSize_; y++) {
      int nodeIndex = (mapWidth_ / gridSize_) * x + y;
      if (y > 0)
        nodes_[nodeIndex]->setNeighbours(
            nodes_[x * (mapWidth_ / gridSize_) + (y - 1)]);
      if (y < ((mapWidth_ / gridSize_) - 1))
        nodes_[nodeIndex]->setNeighbours(
            nodes_[x * (mapWidth_ / gridSize_) + (y + 1)]);
      if (x > 0)
        nodes_[nodeIndex]->setNeighbours(
            nodes_[(x - 1) * (mapWidth_ / gridSize_) + y]);
      if (x < ((mapHeight_ / gridSize_) - 1))
        nodes_[nodeIndex]->setNeighbours(
            nodes_[(x + 1) * (mapWidth_ / gridSize_) + y]);
    }
  }

  // initialize Start and End nodes ptrs (upper left and lower right corners)
  nodeStart_ = nodes_[(mapWidth_ / gridSize_) * 0 + 0];
  nodeStart_->setParentNode(nullptr);
  nodeEnd_ = nodes_[(mapWidth_ / gridSize_) * (mapHeight_ / gridSize_ - 1) +
                    (mapWidth_ / gridSize_ - 1)];
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

  if (Algorithm_reset_) {
    initNodes();
    Algorithm_running_ = false;
    Algorithm_initialized_ = false;
    Algorithm_reset_ = false;
    Algorithm_solved_ = false;

    message_queue_ = std::make_shared<MessageQueue<bool>>();
  }

  if (Algorithm_running_) {
    Algorithm_reset_ = false;

    // initialize Algorithm
    if (!Algorithm_initialized_) {
      initAlgorithm();

      // create thread
      // solve the algorithm concurrently
      t_ = std::thread(&GraphBased::solveConcurrently, this, nodeStart_,
                       nodeEnd_, message_queue_);

      thread_joined_ = false;
      Algorithm_initialized_ = true;
    }

    // check the algorithm is solved or not
    auto msg = message_queue_->receive();
    // if solved
    if (msg) {
      t_.join();
      thread_joined_ = true;
      Algorithm_running_ = false;
      Algorithm_solved_ = true;
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
    if (ImGui::Button("RESET", ImVec2(100.f, 40.f)) && !Algorithm_running_) {
      Algorithm_reset_ = true;
      std::cout << "RESET" << std::endl;
    }
    ImGui::SameLine();
    if (ImGui::Button("PAUSE", ImVec2(100.f, 40.f)))
      std::cout << "PAUSE" << std::endl;
    ImGui::SameLine();
    if (ImGui::Button("RUN", ImVec2(100.f, 40.f)) && !Algorithm_solved_) {
      std::cout << "RUN" << std::endl;
      Algorithm_running_ = true;
    }
  }

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  // grid size slider
  if (ImGui::SliderInt("Grid Size", &slider_grid_size_, 10, 100)) {
    Algorithm_reset_ = true;
    gridSize_ = slider_grid_size_;
  }

  // ImGui::Spacing();
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