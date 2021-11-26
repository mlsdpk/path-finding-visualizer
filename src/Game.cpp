#include "Game.h"

#include "States/Algorithms/GraphBased/ASTAR/ASTAR.h"
#include "States/Algorithms/GraphBased/BFS/BFS.h"
#include "States/Algorithms/GraphBased/DFS/DFS.h"
#include "States/Algorithms/GraphBased/DIJKSTRA/DIJKSTRA.h"
#include "States/Algorithms/SamplingBased/RRT/RRT.h"
#include "States/Algorithms/SamplingBased/RRT_STAR/RRT_STAR.h"

using bfs_state_type = path_finding_visualizer::graph_based::BFS;
using dfs_state_type = path_finding_visualizer::graph_based::DFS;
using dijkstra_state_type = path_finding_visualizer::graph_based::DIJKSTRA;
using astar_state_type = path_finding_visualizer::graph_based::ASTAR;
using rrt_state_type = path_finding_visualizer::sampling_based::RRT;
using rrtstar_state_type = path_finding_visualizer::sampling_based::RRT_STAR;

namespace path_finding_visualizer {

// Constructor
Game::Game(sf::RenderWindow* window) : window_{window} {
  curr_planner_ = PLANNER_NAMES[0];
  // manually add BFS for now
  states_.push(std::make_unique<bfs_state_type>(window_, states_));

  initGuiTheme();
}

// Destructor
Game::~Game() {}

// Accessors
const bool Game::running() const { return window_->isOpen(); }

// Functions
void Game::pollEvents() {
  // Event polling
  while (window_->pollEvent(ev_)) {
    ImGui::SFML::ProcessEvent(ev_);
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

  ImGui::SFML::Update(*window_, dtClock_.restart());
}

void Game::render() {
  window_->clear(sf::Color::White);

  if (!states_.empty()) {
    // guiTheme();
    ImGui::SetNextWindowPos(ImVec2(0.f, 0.f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(
        ImVec2(332.f, static_cast<float>(window_->getSize().y)),
        ImGuiCond_None);

    ImGui::Begin("path_finding_visualizer", nullptr,
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    renderGui();
    states_.top()->render();
    ImGui::End();
  }

  ImGui::SFML::Render(* window_);
  window_->display();
}

void Game::initGuiTheme() {
  ImGuiStyle* style = &ImGui::GetStyle();

  style->FramePadding = ImVec2(8.f, 8.f);

  // style->Colors[ImGuiCol_Text] = ImColor(78, 95, 131, 255);
  // style->Colors[ImGuiCol_WindowBg] = ImColor(246, 229, 245, 255);

  // style->Colors[ImGuiCol_FrameBg] = ImColor(251, 244, 249, 255);
  // style->Colors[ImGuiCol_PopupBg] = ImColor(251, 244, 249, 255);
  // style->Colors[ImGuiCol_Button] = ImColor(251, 244, 249, 255);
  // style->Colors[ImGuiCol_ButtonHovered] =
  //     style->Colors[ImGuiCol_FrameBgHovered];
  // style->Colors[ImGuiCol_ButtonActive] =
  // style->Colors[ImGuiCol_FrameBgActive]; style->Colors[ImGuiCol_SliderGrab] =
  // ImColor(78, 95, 131, 255);
}

void Game::setPlanner(const int id) {
  switch (id) {
    case PLANNERS_IDS::BFS:
      // BFS
      states_.push(std::make_unique<bfs_state_type>(window_, states_));
      break;
    case PLANNERS_IDS::DFS:
      // DFS
      states_.push(std::make_unique<dfs_state_type>(window_, states_));
      break;
    case PLANNERS_IDS::DIJKSTRA:
      // Dijkstra
      states_.push(std::make_unique<dijkstra_state_type>(window_, states_));
      break;
    case PLANNERS_IDS::AStar:
      // A-Star
      states_.push(std::make_unique<astar_state_type>(window_, states_));
      break;
    case PLANNERS_IDS::RRT:
      // RRT
      states_.push(std::make_unique<rrt_state_type>(window_, states_));
      break;
    case PLANNERS_IDS::RRT_STAR:
      // RRTStar
      states_.push(std::make_unique<rrtstar_state_type>(window_, states_));
      break;
    default:
      break;
  }
}

void Game::renderGui() {
  if (ImGui::BeginCombo("Select Planner", curr_planner_.c_str())) {
    for (int n = 0; n < PLANNER_NAMES.size(); n++) {
      bool is_selected = (curr_planner_ == PLANNER_NAMES[n]);
      if (ImGui::Selectable(PLANNER_NAMES[n].c_str(), is_selected)) {
        if (PLANNER_NAMES[n] != curr_planner_) {
          // change the planner
          setPlanner(n);
        }
        curr_planner_ = PLANNER_NAMES[n];
      }

      if (is_selected)
        ImGui::SetItemDefaultFocus();  // Set the initial focus when opening the
                                       // combo (scrolling + for keyboard
                                       // navigation support in the upcoming
                                       // navigation branch)
    }
    ImGui::EndCombo();
  }
  ImGui::Spacing();
}

}  // namespace path_finding_visualizer