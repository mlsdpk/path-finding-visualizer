#include "SamplingBased.h"

// Constructor
SamplingBased::SamplingBased(sf::RenderWindow* window,
                             std::stack<std::unique_ptr<State>>& states,
                             std::string algo_name)
    : State(window, states), keyTimeMax_{1.f}, keyTime_{0.f} {
  initVariables();
  initFonts();
  initColors();
  initBackground(algo_name);
  initButtons();

  start_point_ = std::make_shared<Point>();
  goal_point_ = std::make_shared<Point>();
}

// Destructor
SamplingBased::~SamplingBased() {
  if (!thread_joined_) {
    std::unique_lock<std::mutex> lck(mutex_);
    Algorithm_stopped_ = false;
    lck.unlock();
    t_.join();
  }
}

void SamplingBased::initVariables() {
  // these variables depend on the visualizer
  // for now, just use these and can improve it later
  mapWidth_ = 900;
  mapHeight_ = 640;
  obstSize_ = 20;

  message_queue_ = std::make_shared<MessageQueue<bool>>();

  Algorithm_running_ = false;
  Algorithm_initialized_ = false;
  Algorithm_reset_ = false;
  Algorithm_solved_ = false;
  Algorithm_stopped_ = false;
  thread_joined_ = true;
}

void SamplingBased::initFonts() {
  if (!font1_.loadFromFile("../fonts/bungee-inline-regular.ttf")) {
    throw("ERROR::MainMenuSTATE::COULD NOT LOAD FONT1.");
  }
  if (!font2_.loadFromFile("../fonts/american-typewriter-regular.ttf")) {
    throw("ERROR::MainMenuSTATE::COULD NOT LOAD FONT2.");
  }
}

void SamplingBased::initColors() {
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

void SamplingBased::initBackground(const std::string& algo_name) {
  // Main title
  titleText_.setFont(font1_);
  titleText_.setString(algo_name);
  titleText_.setFillColor(FONT_COL);
  titleText_.setCharacterSize(20);
  sf::FloatRect textRect = titleText_.getLocalBounds();
  titleText_.setOrigin(textRect.left + textRect.width / 2.0f,
                       textRect.top + textRect.height / 2.0f);
  titleText_.setPosition(sf::Vector2f(mapWidth_ / 6.0f, 50.0f));
}

void SamplingBased::initButtons() {
  buttons_["RUN"] = std::make_unique<gui::Button>(
      150, 150, 150, 40, &font2_, "RUN", 18, IDLE_COL, HOVER_COL, ACTIVE_COL);

  buttons_["RESET"] = std::make_unique<gui::Button>(
      150, 210, 150, 40, &font2_, "RESET", 18, IDLE_COL, HOVER_COL, ACTIVE_COL);

  buttons_["MENU"] = std::make_unique<gui::Button>(
      150, 570, 150, 40, &font2_, "MENU", 18, IDLE_COL, HOVER_COL, ACTIVE_COL);
}

void SamplingBased::endState() {}

void SamplingBased::updateKeybinds() { checkForQuit(); }

void SamplingBased::updateButtons() {
  for (auto& it : buttons_) {
    it.second->update(sf::Vector2f(mousePositionWindow_));
  }

  // START the algorithm
  if (buttons_["RUN"]->isPressed() && getKeyTime() && !Algorithm_solved_) {
    Algorithm_running_ = true;
  }

  // RESET the nodes
  if (buttons_["RESET"]->isPressed() && getKeyTime()) {
    Algorithm_reset_ = true;
  }

  // Back to MainMenu
  if (buttons_["MENU"]->isPressed() && getKeyTime()) {
    quit_ = true;
  }
}

/**
 * Getter function for Algorithm key timer.
 *
 * @param none.
 * @return true if keytime > keytime_max else false.
 */
const bool SamplingBased::getKeyTime() {
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
void SamplingBased::updateKeyTime(const float& dt) {
  if (keyTime_ < keyTimeMax_) {
    keyTime_ += 5.f * dt;
  }
}

void SamplingBased::update(const float& dt) {
  // from base classes
  updateKeyTime(dt);
  updateMousePosition();
  updateKeybinds();
  updateButtons();

  if (Algorithm_reset_) {
    Algorithm_running_ = false;
    Algorithm_initialized_ = false;
    Algorithm_reset_ = false;
    Algorithm_solved_ = false;

    std::unique_lock<std::mutex> lck(mutex_);
    Algorithm_stopped_ = true;
    lck.unlock();

    if (!thread_joined_) {
      t_.join();
      thread_joined_ = true;
    }

    message_queue_ = std::make_shared<MessageQueue<bool>>();

    initialize();
  }

  if (Algorithm_running_) {
    Algorithm_reset_ = false;

    // initialize Algorithm
    if (!Algorithm_initialized_) {
      initAlgorithm();

      std::unique_lock<std::mutex> lck(mutex_);
      Algorithm_stopped_ = false;
      lck.unlock();

      // create thread
      // solve the algorithm concurrently
      t_ = std::thread(&SamplingBased::solveConcurrently, this, start_point_,
                       goal_point_, message_queue_);

      thread_joined_ = false;
      Algorithm_initialized_ = true;
    }

    // check the algorithm is solved or not
    auto solved = message_queue_->receive();
    // if solved
    if (solved) {
      t_.join();
      thread_joined_ = true;
      Algorithm_running_ = false;
      Algorithm_solved_ = true;
    }
  } else {
    // only allow mouse and key inputs
    // if the algorithm is not running
    updateUserInput();
  }
}

void SamplingBased::updateUserInput() {
  if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && getKeyTime()) {
    if (mousePositionWindow_.x > 300 && mousePositionWindow_.x < 1200 &&
        mousePositionWindow_.y > 60 && mousePositionWindow_.y < 700) {
      sf::Vector2f mousePos = sf::Vector2f(mousePositionWindow_);

      bool setObstacle = true;
      for (std::size_t i = 0, e = obstacles_.size(); i != e; ++i) {
        if (obstacles_[i]->getGlobalBounds().contains(mousePos)) {
          obstacles_.erase(obstacles_.begin() + i);
          setObstacle = false;
          break;
        }
      }

      if (!Algorithm_solved_) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
          if (setObstacle) {
            start_point_->y =
                map_(mousePositionWindow_.x - 300, 0.0, 900.0, 0.0, 1.0);
            start_point_->x =
                map_(mousePositionWindow_.y - 60, 0.0, 640.0, 0.0, 1.0);
          }
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
          if (setObstacle) {
            goal_point_->y =
                map_(mousePositionWindow_.x - 300, 0.0, 900.0, 0.0, 1.0);
            goal_point_->x =
                map_(mousePositionWindow_.y - 60, 0.0, 640.0, 0.0, 1.0);
          }
        } else {
          // add new obstacle
          if (setObstacle) {
            std::shared_ptr<sf::RectangleShape> obstShape =
                std::make_shared<sf::RectangleShape>(
                    sf::Vector2f(obstSize_, obstSize_));
            obstShape->setPosition(mousePos);
            obstShape->setFillColor(OBST_COL);
            obstacles_.emplace_back(std::move(obstShape));
          }
        }
      } else {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
          if (setObstacle) {
            goal_point_->y =
                map_(mousePositionWindow_.x - 300, 0.0, 900.0, 0.0, 1.0);
            goal_point_->x =
                map_(mousePositionWindow_.y - 60, 0.0, 640.0, 0.0, 1.0);

            // TODO: Find nearest node from goal point and set it as parent
          }
        }
      }
    }
  }
}

void SamplingBased::renderButtons() {
  for (auto& it : buttons_) {
    it.second->render(window_);
  }
}

void SamplingBased::renderObstacles() {
  for (auto& shape : obstacles_) {
    window_->draw(*shape);
  }
}

void SamplingBased::render() {
  // virtual function renderBackground()
  renderBackground();

  renderButtons();

  renderObstacles();

  // virtual function renderAlgorithm()
  // need to be implemented by derived class
  renderAlgorithm();
}

double SamplingBased::map_(double x, double in_min, double in_max,
                           double out_min, double out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
