#include <imgui-SFML.h>
#include <imgui.h>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include "Game.h"

int main() {
  sf::VideoMode videoMode(1600u, 960u);
  sf::RenderWindow window(
      videoMode, "Path-Finding Visualizer",
      sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize);
  // setting frame limit
  window.setFramerateLimit(100u);

  sf::RenderTexture render_texture;
  if (!render_texture.create(700u, 700u)) {
  }

  ImGui::SFML::Init(window, false);

  // Initialize Game
  path_finding_visualizer::Game game(&window, &render_texture);

  // Game Loop
  while (game.running()) {
    // Update
    game.update();

    // Render
    game.render();
  }

  ImGui::SFML::Shutdown();
  return 0;
}
