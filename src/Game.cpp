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
  disable_run_ = false;
  logger_panel_ = std::make_shared<LoggerPanel>();
  curr_planner_ = GRAPH_BASED_PLANNERS[0];
  // manually add BFS for now
  states_.push(
      std::make_unique<bfs_state_type>(window_, states_, logger_panel_));

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

void Game::renderMenuBar(ImGuiIO& io) {
  if (ImGui::BeginMenuBar()) {
    if (ImGui::BeginMenu("Planners")) {
      if (ImGui::BeginMenu("Graph-based Planners")) {
        for (int n = 0; n < GRAPH_BASED_PLANNERS.size(); n++) {
          bool selected = (GRAPH_BASED_PLANNERS[n] == curr_planner_);
          if (ImGui::MenuItem(GRAPH_BASED_PLANNERS[n].c_str(), nullptr,
                              selected, !selected)) {
            if (!selected) {
              // change the planner
              logger_panel_->clear();
              disable_run_ = false;
              setGraphBasedPlanner(n);
            }
            curr_planner_ = GRAPH_BASED_PLANNERS[n];
          }
        }
        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Sampling-based Planners")) {
        for (int n = 0; n < SAMPLING_BASED_PLANNERS.size(); n++) {
          bool selected = (SAMPLING_BASED_PLANNERS[n] == curr_planner_);
          if (ImGui::MenuItem(SAMPLING_BASED_PLANNERS[n].c_str(), nullptr,
                              selected, !selected)) {
            if (!selected) {
              // change the planner
              logger_panel_->clear();
              disable_run_ = false;
              setSamplingBasedPlanner(n);
            }
            curr_planner_ = SAMPLING_BASED_PLANNERS[n];
          }
        }
        ImGui::EndMenu();
      }

      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Run")) {
      {
        if (disable_run_) ImGui::BeginDisabled();
        bool clicked = ImGui::MenuItem("Start Planning");
        if (disable_run_) ImGui::EndDisabled();
        if (clicked) {
          logger_panel_->info("RUN button pressed. Planning started.");
          disable_run_ = true;
          if (!states_.empty()) {
            states_.top()->setRunning(true);
          }
        }
      }
      {
        if (!disable_run_) ImGui::BeginDisabled();
        bool clicked = ImGui::MenuItem("Reset Planner Data");
        if (!disable_run_) ImGui::EndDisabled();
        if (clicked) {
          logger_panel_->info("RESET button pressed. Planning resetted.");
          disable_run_ = false;
          if (!states_.empty()) {
            states_.top()->setReset(true);
          }
        }
      }
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Help")) {
      ImGui::MenuItem("How To Use", nullptr, &show_how_to_use_window_);
      ImGui::MenuItem("About", nullptr, &show_about_window_);
      ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
  }
}

void Game::render() {
  window_->clear(sf::Color::White);

  if (!states_.empty()) {
    ImGui::SetNextWindowPos(ImVec2(0.f, 0.f), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(332.f, 536.f), ImGuiCond_None);

    ImGui::Begin("path_finding_visualizer", nullptr,
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar |
                     ImGuiWindowFlags_MenuBar);

    ImGuiIO& io = ImGui::GetIO();
    renderMenuBar(io);

    ImGui::Text("Current Planner: %s", curr_planner_.c_str());
    ImGui::Spacing();
    ImGui::Spacing();

    states_.top()->render();

    if (show_how_to_use_window_) {
      if (ImGui::CollapsingHeader("How To Use")) {
        ImGui::Text("USAGE GUIDE:");
        ImGui::BulletText("Left-click to place/remove obstacle cells");
        ImGui::BulletText("Left-SHIFT+left-click to change starting cell");
        ImGui::BulletText("Left-CTRL+left-click to change end cell");
      }
    }

    if (show_about_window_) {
      if (ImGui::CollapsingHeader("About")) {
        ImGui::Text("Path-finding Visualizer (v1.0.0)");
        ImGui::Text("Developed and maintained by Phone Thiha Kyaw.");
        ImGui::Text("Email: mlsdphonethk @ gmail dot com");
        ImGui::Separator();
        ImGui::Text("ABOUT THIS VISUALIZER:");
        ImGui::Text(
            "This project involves minimal implementations of\nthe popular "
            "planning algorithms, including\nboth graph-based and "
            "sampling-based planners.");

        ImGui::Separator();

        ImGui::Text("AVAILABLE PLANNERS:");

        ImGui::BulletText("Graph-based Planners:");
        ImGui::Indent();
        ImGui::BulletText("Breadth-first search (BFS)");
        ImGui::BulletText("Depth-first search (DFS)");
        ImGui::BulletText("Dijkstra");
        ImGui::BulletText("A*");

        ImGui::Unindent();
        ImGui::BulletText("Sampling-based Planners:");
        ImGui::Indent();
        ImGui::BulletText("Rapidly-exploring random trees (RRT)");
        ImGui::BulletText("RRT*");
        ImGui::Unindent();
      }
    }

    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(0.f, 536.f), ImGuiCond_Once);
    ImGui::SetNextWindowSize(
        ImVec2(332.f, static_cast<float>(window_->getSize().y) - 536.f),
        ImGuiCond_None);

    ImGui::Begin("Console", nullptr,
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoCollapse);
    ImGui::End();

    // Actually call in the regular Log helper (which will Begin() into the same
    // window as we just did)
    logger_panel_->render("Console");
  }

  ImGui::SFML::Render(*window_);
  window_->display();
}

void Game::initGuiTheme() {
  // setup imgui style
  ImGuiIO& io = ImGui::GetIO();
  io.Fonts->AddFontFromFileTTF("../fonts/OpenSans/OpenSans-Regular.ttf", 18.0f);
  ImGui::SFML::UpdateFontTexture();

  ImGui::StyleColorsDark();

  ImGuiStyle* style = &ImGui::GetStyle();
  style->FramePadding = ImVec2(8.f, 8.f);
  style->ItemSpacing = ImVec2(8.0f, 4.0f);

  // dark theme colors
  auto& colors = ImGui::GetStyle().Colors;

  colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.105f, 0.11f, 1.0f);

  colors[ImGuiCol_Button] = ImVec4(0.2f, 0.205f, 0.21f, 1.0f);
  colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.305f, 0.31f, 1.0f);
  colors[ImGuiCol_ButtonActive] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);

  colors[ImGuiCol_HeaderHovered] = ImVec4(0.3f, 0.305f, 0.31f, 1.0f);
  colors[ImGuiCol_Header] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);

  colors[ImGuiCol_FrameBg] = ImVec4(0.2f, 0.205f, 0.21f, 1.0f);
  colors[ImGuiCol_FrameBgHovered] = ImVec4(0.3f, 0.305f, 0.31f, 1.0f);
  colors[ImGuiCol_FrameBgActive] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);

  colors[ImGuiCol_PopupBg] = ImVec4(0.2f, 0.205f, 0.21f, 1.0f);

  colors[ImGuiCol_SliderGrab] = colors[ImGuiCol_Text];
  colors[ImGuiCol_SliderGrabActive] = colors[ImGuiCol_Text];

  colors[ImGuiCol_PlotHistogram] = ImVec4(0.3f, 0.305f, 0.31f, 1.0f);
}

void Game::setGraphBasedPlanner(const int id) {
  switch (id) {
    case GRAPH_BASED_PLANNERS_IDS::BFS:
      // BFS
      states_.push(
          std::make_unique<bfs_state_type>(window_, states_, logger_panel_));
      break;
    case GRAPH_BASED_PLANNERS_IDS::DFS:
      // DFS
      states_.push(
          std::make_unique<dfs_state_type>(window_, states_, logger_panel_));
      break;
    case GRAPH_BASED_PLANNERS_IDS::DIJKSTRA:
      // Dijkstra
      states_.push(std::make_unique<dijkstra_state_type>(window_, states_,
                                                         logger_panel_));
      break;
    case GRAPH_BASED_PLANNERS_IDS::AStar:
      // A-Star
      states_.push(
          std::make_unique<astar_state_type>(window_, states_, logger_panel_));
      break;
    default:
      break;
  }
}

void Game::setSamplingBasedPlanner(const int id) {
  switch (id) {
    case SAMPLING_BASED_PLANNERS_IDS::RRT:
      // RRT
      states_.push(std::make_unique<rrt_state_type>(
          window_, states_, logger_panel_, SAMPLING_BASED_PLANNERS[id]));
      break;
    case SAMPLING_BASED_PLANNERS_IDS::RRT_STAR:
      // RRTStar
      states_.push(std::make_unique<rrtstar_state_type>(
          window_, states_, logger_panel_, SAMPLING_BASED_PLANNERS[id]));
      break;
    default:
      break;
  }
}

}  // namespace path_finding_visualizer