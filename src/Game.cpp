#include "Game.h"

void Game::initWindow(sf::RenderWindow* window) {
  /*
    @return void

    - initialize window and set FPS limit to 100
  */

  window_ = window;
  window_->setFramerateLimit(100);
}

void Game::initStates() {
  /*
    @return void

    - initialization of state (BFS)
  */

  states_.push(new MainMenu_State(window_, &states_));
}

// Constructor
Game::Game(sf::RenderWindow* window) {
  // initialize window
  initWindow(window);

  // initialize all variables
  // initVariables();

  // initialize states
  initStates();
}

// Destructor
Game::~Game() {
  while (!states_.empty()) {
    delete states_.top();
    states_.pop();
  }
}

// Accessors
const bool Game::running() const { return window_->isOpen(); }

void Game::pollEvents() {
  // Event polling
  while (window_->pollEvent(ev_)) {
    switch (ev_.type) {
      case sf::Event::Closed:
        window_->close();
        break;
      case sf::Event::KeyPressed:
        if (ev_.key.code == sf::Keyboard::Escape) window_->close();
        break;
    }
  }
}

// Functions
void Game::updateDt() { dt_ = dtClock_.restart().asSeconds(); }

void Game::update() {
  pollEvents();
  updateDt();

  if (!states_.empty()) {
    states_.top()->update(dt_);

    if (states_.top()->getQuit()) {
      states_.top()->endState();
      delete states_.top();
      states_.pop();
    }
  } else {
    // End the Application
    window_->close();
  }
}

void Game::render() {
  /*
    @return void

    Render the game objects
  */

  window_->clear(sf::Color::White);

  if (!states_.empty()) {
    states_.top()->render();
  }

  window_->display();
}
