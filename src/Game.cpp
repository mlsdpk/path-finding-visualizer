#include "Game.h"

// Constructor
Game::Game(sf::RenderWindow* window, unsigned int frameLimit)
    : window_{window} {
  // setting frame limit
  window_->setFramerateLimit(frameLimit);

  // initial MainMenu State
  states_.push(std::make_unique<MainMenu_State>(window_, states_));
}

// Destructor
Game::~Game() {}

// Accessors
const bool Game::running() const { return window_->isOpen(); }

// Functions
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

void Game::updateDt() { dt_ = dtClock_.restart().asSeconds(); }

void Game::update() {
  pollEvents();
  updateDt();

  if (!states_.empty()) {
    states_.top()->update(dt_);

    if (states_.top()->getQuit()) {
      states_.top()->endState();
      states_.pop();
    }
  } else {
    // End the Application
    window_->close();
  }
}

void Game::render() {
  window_->clear(sf::Color::White);

  if (!states_.empty()) {
    states_.top()->render();
  }

  window_->display();
}
