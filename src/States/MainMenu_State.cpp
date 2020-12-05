#include "MainMenu_State.h"

// Constructor
MainMenu_State::MainMenu_State(sf::RenderWindow* window, std::stack<State*>* states)
    : State(window, states) {
  initBackground();
  initFonts();
  initAlgorithms();
  initButtons();
}

// Destructor
MainMenu_State::~MainMenu_State() {
  for (auto it = buttons_.begin(); it != buttons_.end(); ++it) {
    delete it->second;
  }

  delete testDDL_;
}

void MainMenu_State::initBackground() {
  background_.setSize(sf::Vector2f(window_->getSize().x, window_->getSize().y));

  if (!backgroundTexture_.loadFromFile("../figures/main_menu.jpeg")) {
    throw("ERROR::MainMenuSTATE::COULD NOT LOAD FIGURE.");
  }

  background_.setTexture(&backgroundTexture_);
}

void MainMenu_State::initFonts() {
  if (!font_.loadFromFile("../fonts/ostrich-regular.ttf")) {
    throw("ERROR::MainMenuSTATE::COULD NOT LOAD FONT.");
  }
}

void MainMenu_State::initAlgorithms() {
  std::ifstream ifs("../config/algorithms.ini");

  if (ifs.is_open()) {
    std::string key = "";
    while (ifs >> key) {
      algo_vec_.push_back(key);
    }
  }

  ifs.close();
}

void MainMenu_State::initButtons() {
  int x = window_->getSize().x / 2 - 200;
  int y = window_->getSize().y / 2 - 150;

  testDDL_ = new gui::DropDownList(x, y,  150, 50, &font_, "Select Algorithm", algo_vec_, 4);

  buttons_["EXIT"] = new gui::Button(x, y+400, 150, 50,
      &font_, "EXIT", 25,
      sf::Color(255, 255, 255, 255),
      sf::Color(240, 240, 240, 200),
      sf::Color(240, 240, 240, 200)
  );
}

void MainMenu_State::updateButtons(const float &dt) {
  for (auto &it: buttons_) {
    it.second->update(sf::Vector2f(mousePositionWindow_));
  }

  testDDL_->update(sf::Vector2f(mousePositionWindow_), dt);

  if (testDDL_->hasActiveButton()) {

    std::string algo = testDDL_->getActiveButton()->getText();

    int index = -1;
    for (size_t i = 0; i < algo_vec_.size(); i++) {
      if (algo == algo_vec_[i]) {
        index = i;
        break;
      }
    }

    switch (index) {
      case 0:
        // BFS
        states_->push(new BFS(window_, states_));
        break;
      case 1:
        // DFS
        break;
      case 2:
        // A-Star
        break;
      case 3:
        // Dijkstra
        break;
      default:
        break;
    }
  }

  // Quit the game
  if (buttons_["EXIT"]->isPressed()) {
    quit_ = true;
  }
}

void MainMenu_State::renderButtons() {
  for (auto &it: buttons_) {
    it.second->render(window_);
  }

  testDDL_->render(window_);
}

void MainMenu_State::renderBackground() {
  window_->draw(background_);
}

void MainMenu_State::endState() {
  std::cout << "Ending MainMenu State" << '\n';
}

void MainMenu_State::updateKeybinds() {
  checkForQuit();
}

void MainMenu_State::update(const float &dt) {
  updateMousePosition();
  updateKeybinds();
  updateButtons(dt);
}

void MainMenu_State::render() {
  renderBackground();
  renderButtons();
}
