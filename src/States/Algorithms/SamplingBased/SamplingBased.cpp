#include "States/Algorithms/SamplingBased/SamplingBased.h"

namespace path_finding_visualizer {
namespace sampling_based {

// Constructor
SamplingBased::SamplingBased(std::shared_ptr<gui::LoggerPanel> logger_panel,
                             const std::string& name)
    : State(logger_panel), key_time_max_{1.f}, key_time_{0.f} {
  logger_panel_->info("Initialize " + name + " planner");
  initVariables();

  start_vertex_ = std::make_shared<Vertex>();
  goal_vertex_ = std::make_shared<Vertex>();

  std::random_device rd;
  rn_gen_ = std::mt19937(rd());
}

// Destructor
SamplingBased::~SamplingBased() {
  if (!thread_joined_) {
    std::unique_lock<std::mutex> lck(mutex_);
    is_stopped_ = false;
    lck.unlock();
    t_.join();
  }
}

void SamplingBased::initMapVariables() {
  map_width_ = 700;
  map_height_ = 700;
  obst_size_ = 20;
}

void SamplingBased::initVariables() {
  initMapVariables();

  message_queue_ = std::make_shared<MessageQueue<bool>>();

  is_running_ = false;
  is_initialized_ = false;
  is_reset_ = false;
  is_solved_ = false;
  is_stopped_ = false;
  thread_joined_ = true;
  disable_gui_parameters_ = false;
  disable_run_ = false;
}

void SamplingBased::endState() {}

/**
 * Getter function for Algorithm key timer.
 *
 * @param none.
 * @return true if keytime > keytime_max else false.
 */
const bool SamplingBased::getKeyTime() {
  if (key_time_ >= key_time_max_) {
    key_time_ = 0.f;
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
void SamplingBased::updateKeyTime(const float& dt) {
  if (key_time_ < key_time_max_) {
    key_time_ += 5.f * dt;
  }
}

void SamplingBased::update(const float& dt, const ImVec2& mousePos) {
  // from base classes
  updateKeyTime(dt);
  updateMousePosition(mousePos);

  if (is_reset_) {
    is_running_ = false;
    is_initialized_ = false;
    is_reset_ = false;
    is_solved_ = false;
    disable_gui_parameters_ = false;

    std::unique_lock<std::mutex> lck(mutex_);
    is_stopped_ = true;
    lck.unlock();

    if (!thread_joined_) {
      t_.join();
      thread_joined_ = true;
    }

    message_queue_ = std::make_shared<MessageQueue<bool>>();

    initialize();

    std::unique_lock<std::mutex> iter_no_lck(iter_no_mutex_);
    curr_iter_no_ = 0u;
  }

  if (is_running_) {
    is_reset_ = false;

    // initialize Algorithm
    if (!is_initialized_) {
      initPlanner();

      std::unique_lock<std::mutex> lck(mutex_);
      is_stopped_ = false;
      lck.unlock();

      logger_panel_->info("Planning started with " +
                          std::to_string(max_iterations_) + " iterations.");

      // create thread
      // solve the algorithm concurrently
      t_ = std::thread(&SamplingBased::solveConcurrently, this, start_vertex_,
                       goal_vertex_, message_queue_);

      thread_joined_ = false;
      is_initialized_ = true;
      disable_gui_parameters_ = true;
    }

    // check the algorithm is solved or not
    auto solved = message_queue_->receive();
    // if solved
    if (solved) {
      t_.join();
      thread_joined_ = true;
      is_running_ = false;
      is_solved_ = true;
      logger_panel_->info(
          "Iterations number reach max limit. Planning stopped.");
    }
  } else {
    // only allow mouse and key inputs
    // if the algorithm is not running
    updateUserInput();
  }
}

void SamplingBased::updateUserInput() {
  if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && getKeyTime()) {
    if (mousePositionWindow_.x > init_grid_xy_.x + obst_size_ / 2 &&
        mousePositionWindow_.x <
            init_grid_xy_.x + map_width_ - obst_size_ / 2 &&
        mousePositionWindow_.y > init_grid_xy_.y + obst_size_ / 2 &&
        mousePositionWindow_.y <
            init_grid_xy_.y + map_height_ - obst_size_ / 2) {
      bool setObstacle = true;
      sf::Vector2f relative_mouse_pos =
          sf::Vector2f(mousePositionWindow_.x - init_grid_xy_.x,
                       mousePositionWindow_.y - init_grid_xy_.y);

      for (std::size_t i = 0, e = obstacles_.size(); i != e; ++i) {
        if (obstacles_[i]->getGlobalBounds().contains(relative_mouse_pos)) {
          obstacles_.erase(obstacles_.begin() + i);
          setObstacle = false;
          break;
        }
      }

      if (!is_solved_) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
          if (setObstacle) {
            start_vertex_->y =
                utils::map(mousePositionWindow_.x, init_grid_xy_.x,
                           init_grid_xy_.x + map_width_, 0.0, 1.0);
            start_vertex_->x =
                utils::map(mousePositionWindow_.y, init_grid_xy_.y,
                           init_grid_xy_.y + map_height_, 0.0, 1.0);
          }
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
          if (setObstacle) {
            goal_vertex_->y =
                utils::map(mousePositionWindow_.x, init_grid_xy_.x,
                           init_grid_xy_.x + map_width_, 0.0, 1.0);
            goal_vertex_->x =
                utils::map(mousePositionWindow_.y, init_grid_xy_.y,
                           init_grid_xy_.y + map_height_, 0.0, 1.0);
          }
        } else {
          // add new obstacle
          if (setObstacle) {
            std::shared_ptr<sf::RectangleShape> obstShape =
                std::make_shared<sf::RectangleShape>(
                    sf::Vector2f(obst_size_, obst_size_));
            obstShape->setPosition(
                sf::Vector2f(relative_mouse_pos.x - obst_size_ / 2.,
                             relative_mouse_pos.y - obst_size_ / 2.));
            obstShape->setFillColor(OBST_COL);
            obstacles_.emplace_back(std::move(obstShape));
          }
        }
      } else {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
          if (setObstacle) {
            goal_vertex_->y =
                utils::map(mousePositionWindow_.x, init_grid_xy_.x,
                           init_grid_xy_.x + map_width_, 0.0, 1.0);
            goal_vertex_->x =
                utils::map(mousePositionWindow_.y, init_grid_xy_.y,
                           init_grid_xy_.y + map_height_, 0.0, 1.0);

            // TODO: Find nearest node from goal point and set it as parent
          }
        }
      }
    }
  }
}

void SamplingBased::renderMap(sf::RenderTexture& render_texture) {
  sf::RectangleShape planning_map(sf::Vector2f(map_width_, map_height_));
  planning_map.setFillColor(sf::Color(255, 255, 255));
  planning_map.setOutlineThickness(5);
  planning_map.setOutlineColor(sf::Color(0, 0, 0));
  planning_map.setPosition(sf::Vector2f(init_grid_xy_.x, init_grid_xy_.y));
  render_texture.draw(planning_map);
}

void SamplingBased::renderObstacles(sf::RenderTexture& render_texture) {
  for (auto& shape : obstacles_) {
    sf::RectangleShape obst(sf::Vector2f(obst_size_, obst_size_));
    obst.setPosition(sf::Vector2f(init_grid_xy_.x + shape->getPosition().x,
                                  init_grid_xy_.y + shape->getPosition().y));
    obst.setFillColor(OBST_COL);
    render_texture.draw(obst);
  }
}

void SamplingBased::clearObstacles() { obstacles_.clear(); }

void SamplingBased::renderGui() {
  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.f);
  {
    std::unique_lock<std::mutex> iter_no_lck(iter_no_mutex_);
    const float progress = static_cast<float>(
        utils::map(static_cast<double>(curr_iter_no_), 0.0,
                   static_cast<double>(max_iterations_), 0.0, 1.0));
    const std::string buf =
        std::to_string(curr_iter_no_) + "/" + std::to_string(max_iterations_);
    ImGui::Text("Planning Progress:");
    ImGui::SameLine();
    gui::HelpMarker(
        "Shows the current iteration number of the planning progress");
    ImGui::Spacing();
    ImGui::ProgressBar(progress, ImVec2(-1.0f, 0.0f), buf.c_str());
  }
  ImGui::PopStyleVar();
  ImGui::Spacing();

  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.f, 8.f));
  if (ImGui::CollapsingHeader("Edit", ImGuiTreeNodeFlags_DefaultOpen)) {
    if (disable_gui_parameters_) ImGui::BeginDisabled();
    ImGui::Indent(8.f);

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(2.f, 4.f));

    ImGui::Text("Map:");
    ImGui::SameLine();
    gui::HelpMarker(
        "Set width and height of the planning map.\nInternally these values "
        "are mapped between 0 and 1.");
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.f, 2.f));

    if (gui::inputInt("width", &map_width_, 500, 10000, 100, 1000)) {
    }
    ImGui::PopStyleVar();
    if (gui::inputInt("height", &map_height_, 500, 10000, 100, 1000)) {
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
    if (ImGui::Button("Restore Defaults##edit_restore")) {
      initMapVariables();
    }

    ImGui::Unindent(8.f);
    ImGui::PopStyleVar(2);
    if (disable_gui_parameters_) ImGui::EndDisabled();
    ImGui::Spacing();
  }
  ImGui::PopStyleVar();

  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.f, 8.f));
  if (ImGui::CollapsingHeader("Configuration",
                              ImGuiTreeNodeFlags_DefaultOpen)) {
    if (disable_gui_parameters_) ImGui::BeginDisabled();
    ImGui::Indent(8.f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(2.f, 4.f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.f, 2.f));

    gui::inputInt("max_iterations", &max_iterations_, 1, 100000, 1, 1000,
                  "Maximum number of iterations to run the planner");

    // virtual function renderParametersGui()
    // need to be implemented by derived class
    renderParametersGui();

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.f, 8.f));
    ImGui::Spacing();
    ImGui::PopStyleVar();
    if (ImGui::Button("Restore Defaults##config_restore")) {
      initParameters();
      logger_panel_->info(
          "Planner related parameters resetted to default ones.");
    }

    ImGui::Unindent(8.f);
    ImGui::PopStyleVar(3);
    if (disable_gui_parameters_) ImGui::EndDisabled();
  }
  ImGui::PopStyleVar();
}

void SamplingBased::renderConfig() {
  // render gui
  renderGui();
}

void SamplingBased::renderScene(sf::RenderTexture& render_texture) {
  const auto texture_size = render_texture.getSize();
  init_grid_xy_.x = (texture_size.x / 2.) - (map_width_ / 2.);
  init_grid_xy_.y = (texture_size.y / 2.) - (map_height_ / 2.);

  renderMap(render_texture);
  renderObstacles(render_texture);
  // virtual function renderPlannerData()
  // need to be implemented by derived class
  renderPlannerData(render_texture);
}

void SamplingBased::solveConcurrently(
    std::shared_ptr<Vertex> start_point, std::shared_ptr<Vertex> goal_point,
    std::shared_ptr<MessageQueue<bool>> message_queue) {
  // copy assignment
  // thread-safe due to shared_ptrs
  std::shared_ptr<Vertex> start_vertex = start_point;
  std::shared_ptr<Vertex> goal_vertex = goal_point;
  std::shared_ptr<MessageQueue<bool>> s_message_queue = message_queue;

  bool solved = false;

  double cycle_duration = 1;  // duration of a single simulation cycle in ms
  // init stop watch
  auto last_update = std::chrono::system_clock::now();

  while (true) {
    // compute time difference to stop watch
    long time_since_last_update =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now() - last_update)
            .count();

    if (time_since_last_update >= cycle_duration) {
      // run the main algorithm
      updatePlanner(solved, *start_vertex, *goal_vertex);

      // reset stop watch for next cycle
      last_update = std::chrono::system_clock::now();
    }

    // sends an update method to the message queue using move semantics
    auto ftr = std::async(std::launch::async, &MessageQueue<bool>::send,
                          s_message_queue, std::move(solved));
    ftr.wait();

    if (solved) return;

    std::lock_guard<std::mutex> lock(mutex_);
    if (is_stopped_) return;

    // sleep at every iteration to reduce CPU usage
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

}  // namespace sampling_based
}  // namespace path_finding_visualizer