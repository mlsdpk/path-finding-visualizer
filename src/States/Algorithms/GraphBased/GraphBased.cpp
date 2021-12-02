#include "GraphBased.h"

namespace path_finding_visualizer {
namespace graph_based {

// Constructor
GraphBased::GraphBased(std::shared_ptr<gui::LoggerPanel> logger_panel)
    : State(logger_panel), keyTimeMax_{1.f}, keyTime_{0.f} {
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
  initGridMapParams();
  grid_connectivity_ = 0;

  message_queue_ = std::make_shared<MessageQueue<bool>>();

  is_running_ = false;
  is_initialized_ = false;
  is_reset_ = false;
  is_solved_ = false;
  thread_joined_ = true;
  disable_run_ = false;
  disable_gui_parameters_ = false;
}

void GraphBased::initGridMapParams() {
  ui_grid_size_ = 20;
  grid_size_ = ui_grid_size_;
  no_of_grid_rows_ = no_of_grid_cols_ = 10;
  map_width_ = no_of_grid_cols_ * grid_size_;
  map_height_ = no_of_grid_rows_ * grid_size_;
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
  map_width_ = no_of_grid_cols_ * grid_size_;
  map_height_ = no_of_grid_rows_ * grid_size_;

  if (reset) {
    nodes_.clear();

    for (int i = 0; i < (map_width_ / grid_size_) * (map_height_ / grid_size_);
         i++) {
      nodes_.emplace_back(std::make_shared<Node>());
    }
  }

  // set all nodes to free obsts and respective positions
  for (int x = 0; x < map_height_ / grid_size_; x++) {
    for (int y = 0; y < map_width_ / grid_size_; y++) {
      int nodeIndex = (map_width_ / grid_size_) * x + y;
      if (reset) {
        nodes_[nodeIndex]->setPosition(sf::Vector2i(x, y));
        nodes_[nodeIndex]->setObstacle(false);
        nodes_[nodeIndex]->setStart(false);
        nodes_[nodeIndex]->setGoal(false);
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
    for (int x = 0; x < map_height_ / grid_size_; x++) {
      for (int y = 0; y < map_width_ / grid_size_; y++) {
        int nodeIndex = (map_width_ / grid_size_) * x + y;
        nodes_[nodeIndex]->clearNeighbours();
        utils::addNeighbours(nodes_, nodeIndex,
                             static_cast<unsigned>(map_width_ / grid_size_),
                             static_cast<unsigned>(map_height_ / grid_size_),
                             [](int connectivity) {
                               return (connectivity == 1) ? true : false;
                             }(grid_connectivity_));
      }
    }
  }

  if (reset) {
    // initialize Start and End nodes ptrs (upper left and lower right corners)
    nodeStart_ = nodes_[(map_width_ / grid_size_) * 0 + 0];
    nodeStart_->setParentNode(nullptr);
    nodeStart_->setStart(true);
    nodeEnd_ =
        nodes_[(map_width_ / grid_size_) * (map_height_ / grid_size_ - 1) +
               (map_width_ / grid_size_ - 1)];
    nodeEnd_->setGoal(true);
  }
}

void GraphBased::endState() {}

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

void GraphBased::update(const float& dt, const ImVec2& mousePos) {
  // from base classes
  updateKeyTime(dt);
  updateMousePosition(mousePos);

  if (is_reset_) {
    initNodes(false, false);
    is_running_ = false;
    is_initialized_ = false;
    is_reset_ = false;
    is_solved_ = false;
    disable_gui_parameters_ = false;

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
      disable_gui_parameters_ = true;
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
  for (int x = 0; x < map_height_ / grid_size_; x++) {
    for (int y = 0; y < map_width_ / grid_size_; y++) {
      int nodeIndex = (map_width_ / grid_size_) * x + y;
      nodes_[nodeIndex]->setObstacle(false);
    }
  }
}

void GraphBased::renderGui() {
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.f, 8.f));
  if (ImGui::CollapsingHeader("Edit", ImGuiTreeNodeFlags_DefaultOpen)) {
    if (disable_gui_parameters_) ImGui::BeginDisabled();
    ImGui::Indent(8.f);

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(2.f, 4.f));

    ImGui::Text("Gridmap:");
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.f, 2.f));

    if (gui::inputInt("rows", &no_of_grid_rows_, 5, 1000, 1, 10,
                      "Number of rows in the gridmap"))
      initNodes(true, false);

    if (gui::inputInt("cols", &no_of_grid_cols_, 5, 1000, 1, 10,
                      "Number of columns in the gridmap"))
      initNodes(true, false);

    ImGui::PopStyleVar();

    if (gui::inputInt("grid size", &ui_grid_size_, 10, 100, 1, 10,
                      "The size of a grid. Set this size larger to zoom in the "
                      "gridmap.")) {
      grid_size_ = ui_grid_size_;
      initNodes(true, false);
    }

    ImGui::Text("Random Obstacles:");
    ImGui::SameLine();
    gui::HelpMarker("TODO: Randomly generate obstacles in the gridmap");
    static int rand_obsts_no = 10;
    if (ImGui::InputInt("##rand_obst_input", &rand_obsts_no, 1, 10,
                        ImGuiInputTextFlags_EnterReturnsTrue)) {
      // TODO:
    }
    ImGui::SameLine();
    if (ImGui::Button("Generate")) {
      // TODO:
    }

    if (ImGui::Button("Clear Obstacles")) {
      clearObstacles();
    }
    ImGui::SameLine();
    if (ImGui::Button("Restore Defaults")) {
      initGridMapParams();
      initNodes(true, false);
    }

    ImGui::PopStyleVar(2);
    ImGui::Unindent(8.f);

    if (disable_gui_parameters_) ImGui::EndDisabled();
    ImGui::Spacing();
  }

  if (ImGui::CollapsingHeader("Configurations",
                              ImGuiTreeNodeFlags_DefaultOpen)) {
    if (disable_gui_parameters_) ImGui::BeginDisabled();

    ImGui::Indent(8.f);

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(2.f, 4.f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.f, 2.f));

    // radio buttons for choosing 4 or 8 connected grids
    {
      bool a, b;
      a = ImGui::RadioButton("4-connected", &grid_connectivity_, 0);
      ImGui::SameLine();
      b = ImGui::RadioButton("8-connected", &grid_connectivity_, 1);
      if (a || b) {
        initNodes(false, true);
      }
      ImGui::SameLine();
      gui::HelpMarker(
          "4-connected and 8-connected represent the number of neighbours of a "
          "grid.\nPlanners relying on heuristics usually use Manhattan "
          "distance (L1-norm) for 4-connected grids\nand Euclidean Distance "
          "(L2-norm) for 8-connected grids.");
    }

    // virtual function renderParametersGui()
    // need to be implemented by derived class
    renderParametersGui();

    ImGui::PopStyleVar(3);
    ImGui::Unindent(8.f);

    if (disable_gui_parameters_) ImGui::EndDisabled();
  }
  ImGui::PopStyleVar();
}

void GraphBased::renderConfig() { renderGui(); }

void GraphBased::renderScene(sf::RenderTexture& render_texture) {
  // virtual function renderNodes()
  // need to be implemented by derived class
  renderNodes(render_texture);
}

void GraphBased::solveConcurrently(
    std::shared_ptr<Node> nodeStart, std::shared_ptr<Node> nodeEnd,
    std::shared_ptr<MessageQueue<bool>> message_queue) {
  // copy assignment
  // thread-safe due to shared_ptrs
  std::shared_ptr<Node> s_nodeStart = nodeStart;
  std::shared_ptr<Node> s_nodeEnd = nodeEnd;
  std::shared_ptr<MessageQueue<bool>> s_message_queue = message_queue;

  bool solved = false;

  double cycleDuration = 1.0;  // duration of a single simulation cycle in ms
  // init stop watch
  auto lastUpdate = std::chrono::system_clock::now();

  while (true) {
    // compute time difference to stop watch
    long timeSinceLastUpdate =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now() - lastUpdate)
            .count();

    if (timeSinceLastUpdate >= cycleDuration) {
      updatePlanner(solved, *s_nodeStart, *s_nodeEnd);

      // reset stop watch for next cycle
      lastUpdate = std::chrono::system_clock::now();
    }

    // sends an update method to the message queue using move semantics
    auto ftr = std::async(std::launch::async, &MessageQueue<bool>::send,
                          s_message_queue, std::move(solved));
    ftr.wait();

    if (solved) return;

    // sleep at every iteration to reduce CPU usage
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

}  // namespace graph_based
}  // namespace path_finding_visualizer