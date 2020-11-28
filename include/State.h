#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <stack>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

/*
  State Base Class
*/

class State
{
private:

protected:
  sf::RenderWindow* window_;
  sf::Vector2i mousePositionWindow_;
  bool quit_;

public:
  // Constructor
  State(sf::RenderWindow* window);

  // Destructor
  virtual ~State();

  // Accessors
  const bool& getQuit() const;

  // Functions
  virtual void checkForQuit();
  virtual void updateMousePosition();

  // virtual functions
  virtual void endState() = 0;
  virtual void updateKeybinds() = 0;
  virtual void update() = 0;
  virtual void render() = 0;
};
