#include "MainMenu_State.h"

// Constructor
MainMenu_State::MainMenu_State(sf::RenderWindow* window, std::stack<State*>* states)
    : State(window, states) {
  initColors();
  initFonts();
  initBackground();
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

void MainMenu_State::initColors() {
  BGN_COL = sf::Color(246, 229, 245, 255);
  FONT_COL = sf::Color(78, 95, 131, 255);
  IDLE_COL = sf::Color(251, 244, 249, 255);
  HOVER_COL = sf::Color(245, 238, 243, 255);
}

void MainMenu_State::initFonts() {
  if (!font1_.loadFromFile("../fonts/bungee-inline-regular.ttf")) {
    throw("ERROR::MainMenuSTATE::COULD NOT LOAD FONT1.");
  }
  if (!font2_.loadFromFile("../fonts/american-typewriter-regular.ttf")) {
    throw("ERROR::MainMenuSTATE::COULD NOT LOAD FONT2.");
  }
}

void MainMenu_State::initBackground() {
  // Main title
  backgroundText_.setFont(font1_);
  backgroundText_.setString("PATHFINDING VISUALIZER");
  backgroundText_.setFillColor(FONT_COL);
  backgroundText_.setCharacterSize(50);

  sf::FloatRect textRect = backgroundText_.getLocalBounds();
  backgroundText_.setOrigin(textRect.left + textRect.width/2.0f,
    textRect.top  + textRect.height/2.0f);
  backgroundText_.setPosition(
    sf::Vector2f(window_->getSize().x/2.0f,
                 window_->getSize().y/10.0f)
  );

  // Version title
  versionText_.setFont(font2_);
  versionText_.setString("v1.0.0");
  versionText_.setFillColor(FONT_COL);
  versionText_.setCharacterSize(25);

  textRect = versionText_.getLocalBounds();
  versionText_.setOrigin(textRect.left + textRect.width/2.0f,
    textRect.top  + textRect.height/2.0f);
  versionText_.setPosition(
    sf::Vector2f(window_->getSize().x - window_->getSize().x/4.2f,
                 window_->getSize().y/6.5f)
  );
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
  int x = window_->getSize().x / 2;
  int y = window_->getSize().y / 2 - 150;

  testDDL_ = new gui::DropDownList(x, y,  250, 50, &font2_, "SELECT ALGORITHM", algo_vec_, 4);

  buttons_["EXIT"] = new gui::Button(x, y+410, 150, 50,
      &font2_, "EXIT", 20,
      IDLE_COL,
      HOVER_COL,
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
  window_->clear(BGN_COL);

  // render main title
  window_->draw(backgroundText_);

  // render version title
  window_->draw(versionText_);
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
