#include "MainMenu_State.h"

// Constructor
MainMenu_State::MainMenu_State(sf::RenderWindow* window):
  State(window)
{
  initFonts();
}

// Destructor
MainMenu_State::~MainMenu_State()
{

}

void MainMenu_State::initFonts()
{
  // if (!font_.loadFromFile("fonts/wasd.ttf")) {
  //   throw("ERROR::MainMenuSTATE::COULD NOT LOAD FONT.")
  // }
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
}

void MainMenu_State::render()
{

}
