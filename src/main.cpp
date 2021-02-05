#include "Game.h"

int main() {
  sf::VideoMode videoMode(1200, 700);
  sf::RenderWindow window(videoMode, "Path-Planning Visualizer",
                          sf::Style::Titlebar | sf::Style::Close);

  // Initialize Game
  Game game(&window, 100);

  // Game Loop
  while (game.running()) {
    // Update
    game.update();

    // Render
    game.render();
  }
  return 0;
}
