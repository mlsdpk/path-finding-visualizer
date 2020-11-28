#include "Game.h"

int main()
{
  sf::VideoMode videoMode(800, 800);
  sf::RenderWindow window(videoMode, "Path-Planning Visualizer");

  // Initialize Game
  Game game(&window);

  // Game Loop
  while (game.running())
  {
    // Update
    game.update();

    // Render
    game.render();
  }
  return 0;
}
