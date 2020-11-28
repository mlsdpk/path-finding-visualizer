#include "MainMenu_State.h"

// Constructor
MainMenu_State::MainMenu_State(sf::RenderWindow* window, std::stack<State*>* states)
    : State(window, states) {
  initFonts();
  initButtons();

  background_.setSize(sf::Vector2f(window->getSize().x, window->getSize().y));
  background_.setFillColor(sf::Color::Black);
}

// Destructor
MainMenu_State::~MainMenu_State()
{
  for (auto it = buttons_.begin(); it != buttons_.end(); ++it) {
    delete it->second;
  }
}

void MainMenu_State::initFonts()
{
  if (!font_.loadFromFile("../fonts/ostrich-regular.ttf")) {
    throw("ERROR::MainMenuSTATE::COULD NOT LOAD FONT.");
  }
}

void MainMenu_State::initButtons()
{
  buttons_["START"] = new Button(100, 100, 150, 50,
      &font_, "START", 20,
      sf::Color(70, 70, 70, 200),
      sf::Color(150, 150, 150, 255),
      sf::Color(20, 20, 20, 200)
  );

  buttons_["EXIT"] = new Button(100, 300, 150, 50,
      &font_, "EXIT", 20,
      sf::Color(70, 70, 70, 200),
      sf::Color(150, 150, 150, 255),
      sf::Color(20, 20, 20, 200)
  );
}

void MainMenu_State::updateButtons()
{
  for (auto &it: buttons_) {
    it.second->update(sf::Vector2f(mousePositionWindow_));
  }

  // Quit the game
  if (buttons_["START"]->isPressed()) {
    states_->push(new BFS(window_, states_));
  }

  // Quit the game
  if (buttons_["EXIT"]->isPressed()) {
    quit_ = true;
  }
}

void MainMenu_State::renderButtons()
{
  for (auto &it: buttons_) {
    it.second->render(window_);
  }
}

void MainMenu_State::endState()
{
  std::cout << "Ending MainMenu State" << '\n';
}

void MainMenu_State::updateKeybinds()
{
  checkForQuit();
}

void MainMenu_State::update()
{
  updateMousePosition();
  updateKeybinds();
  updateButtons();
}

void MainMenu_State::render()
{
  renderButtons();
}
