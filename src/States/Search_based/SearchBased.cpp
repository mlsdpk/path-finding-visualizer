#include "SearchBased.h"

// Constructor
SearchBased::SearchBased(sf::RenderWindow* window,
                     std::stack<std::unique_ptr<State>>& states,
                     std::string algo_name)
    : State(window, states), keyTimeMax_{1.f}, keyTime_{0.f} {
  initVariables();
  initNodes();
  initFonts();
  initColors();
  initBackground(algo_name);
  initButtons();
}

// Destructor
SearchBased::~SearchBased() {
  if (!thread_joined_) {
    // std::cout << "thread joined" << '\n';
    t_.join();
  }
}

void SearchBased::initVariables() {
  // these variables depend on the visualizer
  // for now, just use these and can improve it later
  gridSize_ = 20;
  mapWidth_ = 900;
  mapHeight_ = 640;

  for (int i = 0; i < (mapWidth_ / gridSize_) * (mapHeight_ / gridSize_); i++) {
    nodes_.emplace_back(std::make_shared<Node>());
  }

  message_queue_ = std::make_shared<MessageQueue<bool>>();

  Algorithm_running_ = false;
  Algorithm_initialized_ = false;
  Algorithm_reset_ = false;
  Algorithm_solved_ = false;
  thread_joined_ = true;
}

void SearchBased::initFonts() {
  if (!font1_.loadFromFile("../fonts/bungee-inline-regular.ttf")) {
    throw("ERROR::MainMenuSTATE::COULD NOT LOAD FONT1.");
  }
  if (!font2_.loadFromFile("../fonts/american-typewriter-regular.ttf")) {
    throw("ERROR::MainMenuSTATE::COULD NOT LOAD FONT2.");
  }
}

void SearchBased::initColors() {
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

void SearchBased::initBackground(const std::string& algo_name) {
  // Main title
  titleText_.setFont(font1_);
  titleText_.setString(algo_name);
  titleText_.setFillColor(FONT_COL);
  titleText_.setCharacterSize(20);
  sf::FloatRect textRect = titleText_.getLocalBounds();
  titleText_.setOrigin(textRect.left + textRect.width / 2.0f,
                       textRect.top + textRect.height / 2.0f);
  titleText_.setPosition(sf::Vector2f(mapWidth_ / 6.0f, 50.0f));

  // Cell Names Background
  cellNamesBGN_.setPosition(sf::Vector2f(mapWidth_ / 3.0f, 0.f));
  cellNamesBGN_.setSize(sf::Vector2f(mapWidth_, 60));
  cellNamesBGN_.setFillColor(IDLE_COL);

  // Cell Names Shapes and Texts
  std::vector<sf::Color> shapeColors{OBST_COL, IDLE_COL, VISITED_COL,
                                     FRONTIER_COL};
  std::vector<std::string> shapeTexts{"Obstacle cell", "Empty cell",
                                      "Visited cell", "Frontier cell"};

  float initialX = 50.0;
  float xOffset = 200.0;
  for (int i = 0; i < shapeColors.size(); i++) {
    // shape
    sf::RectangleShape rectangle(sf::Vector2f(20.f, 20.f));
    rectangle.setPosition(
        sf::Vector2f(mapWidth_ / 3.0f + initialX + xOffset * i, 20.f));
    rectangle.setFillColor(shapeColors[i]);
    if (shapeColors[i] == IDLE_COL) {
      rectangle.setOutlineThickness(2.f);
      rectangle.setOutlineColor(BGN_COL);
    }

    // text
    sf::Text text;
    text.setFont(font2_);
    text.setString(shapeTexts[i]);
    text.setFillColor(FONT_COL);
    text.setCharacterSize(20);
    text.setPosition(
        sf::Vector2f(mapWidth_ / 3.0f + initialX + 40.f + xOffset * i, 18.f));

    cellNamesShapes_.push_back(rectangle);
    cellNamesTexts_.push_back(text);
  }
}

void SearchBased::initButtons() {
  buttons_["RUN"] = std::make_unique<gui::Button>(
      150, 150, 150, 40, &font2_, "RUN", 18, IDLE_COL, HOVER_COL, ACTIVE_COL);

  buttons_["RESET"] = std::make_unique<gui::Button>(
      150, 210, 150, 40, &font2_, "RESET", 18, IDLE_COL, HOVER_COL, ACTIVE_COL);

  buttons_["MENU"] = std::make_unique<gui::Button>(
      150, 570, 150, 40, &font2_, "MENU", 18, IDLE_COL, HOVER_COL, ACTIVE_COL);
}

void SearchBased::initNodes() {
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

void SearchBased::endState() {}

void SearchBased::updateKeybinds() { checkForQuit(); }

void SearchBased::updateButtons() {
  for (auto& it : buttons_) {
    it.second->update(sf::Vector2f(mousePositionWindow_));
  }

  // START the algorithm
  if (buttons_["RUN"]->isPressed() && getKeyTime() && !Algorithm_solved_) {
    Algorithm_running_ = true;
  }

  // RESET the nodes
  if (buttons_["RESET"]->isPressed() && getKeyTime() && !Algorithm_running_) {
    Algorithm_reset_ = true;
  }

  // Back to MainMenu
  if (buttons_["MENU"]->isPressed() && getKeyTime()) {
    quit_ = true;
  }
}

/**
 * Getter function for SearchBased key timer.
 *
 * @param none.
 * @return true if keytime > keytime_max else false.
 */
const bool SearchBased::getKeyTime() {
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
void SearchBased::updateKeyTime(const float& dt) {
  if (keyTime_ < keyTimeMax_) {
    keyTime_ += 5.f * dt;
  }
}

void SearchBased::update(const float& dt) {
  // from base classes
  updateKeyTime(dt);
  updateMousePosition();
  updateKeybinds();
  updateButtons();

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

    // initialize SearchBased
    if (!Algorithm_initialized_) {
      initAlgorithm();

      // create thread
      // solve the algorithm concurrently
      t_ = std::thread(&SearchBased::solveConcurrently, this, nodeStart_,
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

/*
  Rendering Stuffs
*/

void SearchBased::renderButtons() {
  for (auto& it : buttons_) {
    it.second->render(window_);
  }
}

void SearchBased::render() {
  // virtual function renderBackground()
  renderBackground();

  renderButtons();

  // virtual function renderNodes()
  // need to be implemented by derived class
  renderNodes();
}
