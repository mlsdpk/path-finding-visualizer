#include "States/Algorithms/SamplingBased/SamplingBased.h"

namespace path_finding_visualizer {
namespace sampling_based {

// Constructor
SamplingBased::SamplingBased(sf::RenderWindow* window,
                             std::stack<std::unique_ptr<State>>& states)
    : State(window, states), key_time_max_{1.f}, key_time_{0.f} {
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

void SamplingBased::initVariables() {
  // these variables depend on the visualizer
  // for now, just use these and can improve it later
  map_width_ = 700;
  map_height_ = 700;
  obst_size_ = 20;

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

void SamplingBased::updateKeybinds() { checkForQuit(); }

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

void SamplingBased::update(const float& dt) {
  // from base classes
  updateKeyTime(dt);
  updateMousePosition();
  updateKeybinds();

  if (is_reset_) {
    is_running_ = false;
    is_initialized_ = false;
    is_reset_ = false;
    is_solved_ = false;

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

      // create thread
      // solve the algorithm concurrently
      t_ = std::thread(&SamplingBased::solveConcurrently, this, start_vertex_,
                       goal_vertex_, message_queue_);

      thread_joined_ = false;
      is_initialized_ = true;
    }

    // check the algorithm is solved or not
    auto solved = message_queue_->receive();
    // if solved
    if (solved) {
      t_.join();
      thread_joined_ = true;
      is_running_ = false;
      is_solved_ = true;
    }
  } else {
    // only allow mouse and key inputs
    // if the algorithm is not running
    updateUserInput();
  }
}

void SamplingBased::updateUserInput() {
  if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && getKeyTime()) {
    if (mousePositionWindow_.x > 350 && mousePositionWindow_.x < 1050 &&
        mousePositionWindow_.y > 18 && mousePositionWindow_.y < 718) {
      sf::Vector2f mousePos = sf::Vector2f(mousePositionWindow_);

      bool setObstacle = true;
      for (std::size_t i = 0, e = obstacles_.size(); i != e; ++i) {
        if (obstacles_[i]->getGlobalBounds().contains(mousePos)) {
          obstacles_.erase(obstacles_.begin() + i);
          setObstacle = false;
          break;
        }
      }

      if (!is_solved_) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
          if (setObstacle) {
            start_vertex_->y =
                utils::map(mousePositionWindow_.x - 350, 0.0, 700.0, 0.0, 1.0);
            start_vertex_->x =
                utils::map(mousePositionWindow_.y - 18, 0.0, 700.0, 0.0, 1.0);
          }
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
          if (setObstacle) {
            goal_vertex_->y =
                utils::map(mousePositionWindow_.x - 350, 0.0, 700.0, 0.0, 1.0);
            goal_vertex_->x =
                utils::map(mousePositionWindow_.y - 18, 0.0, 700.0, 0.0, 1.0);
          }
        } else {
          // add new obstacle
          if (setObstacle) {
            std::shared_ptr<sf::RectangleShape> obstShape =
                std::make_shared<sf::RectangleShape>(
                    sf::Vector2f(obst_size_, obst_size_));
            obstShape->setPosition(mousePos);
            obstShape->setFillColor(OBST_COL);
            obstacles_.emplace_back(std::move(obstShape));
          }
        }
      } else {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
          if (setObstacle) {
            goal_vertex_->y =
                utils::map(mousePositionWindow_.x - 350, 0.0, 700.0, 0.0, 1.0);
            goal_vertex_->x =
                utils::map(mousePositionWindow_.y - 18, 0.0, 700.0, 0.0, 1.0);

            // TODO: Find nearest node from goal point and set it as parent
          }
        }
      }
    }
  }
}

void SamplingBased::renderObstacles() {
  for (auto& shape : obstacles_) {
    window_->draw(*shape);
  }
}

void SamplingBased::clearObstacles() { obstacles_.clear(); }

void SamplingBased::renderGui() {
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

  {
    std::unique_lock<std::mutex> iter_no_lck(iter_no_mutex_);
    const float progress = static_cast<float>(
        utils::map(static_cast<double>(curr_iter_no_), 0.0,
                   static_cast<double>(max_iterations_), 0.0, 1.0));
    const std::string buf =
        std::to_string(curr_iter_no_) + "/" + std::to_string(max_iterations_);
    ImGui::ProgressBar(progress, ImVec2(-1.0f, 0.0f), buf.c_str());
  }

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  if (disable_gui_parameters_) ImGui::BeginDisabled();

  if (ImGui::InputInt("max_iterations", &max_iterations_, 1, 1000)) {
    if (max_iterations_ < 1) max_iterations_ = 1;
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
      initParameters();
    }
  }

  if (disable_gui_parameters_) ImGui::EndDisabled();
}

void SamplingBased::render() {
  renderObstacles();

  // virtual function renderPlannerData()
  // need to be implemented by derived class
  renderPlannerData();

  // render gui
  renderGui();
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