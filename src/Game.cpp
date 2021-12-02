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
Game::Game(sf::RenderWindow* window, sf::RenderTexture* render_texture)
    : window_{window}, render_texture_{render_texture}, disable_run_{false} {
  logger_panel_ = std::make_shared<gui::LoggerPanel>();
  curr_planner_ = GRAPH_BASED_PLANNERS[0];
  // manually add BFS for now
  states_.push(std::make_unique<bfs_state_type>(logger_panel_));
  view_move_xy_.x = view_move_xy_.y = 0.f;
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
    // ImGui::SFML::ProcessEvent(ev_);
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

void Game::updateDt() { dt_ = dtClock_.getElapsedTime().asSeconds(); }

void Game::update() {
  pollEvents();
  updateDt();

  if (!states_.empty()) {
    states_.top()->update(dt_, mouse_pos_in_canvas_);
  } else {
    // End the Application
    window_->close();
  }
  ImGui::SFML::Update(*window_, dtClock_.restart());
}

void Game::renderNewPlannerMenu() {
  if (ImGui::BeginMenu("New Planner")) {
    if (ImGui::BeginMenu("Graph-based Planners")) {
      for (int n = 0; n < GRAPH_BASED_PLANNERS.size(); n++) {
        bool selected = (GRAPH_BASED_PLANNERS[n] == curr_planner_);
        if (ImGui::MenuItem(GRAPH_BASED_PLANNERS[n].c_str(), nullptr, selected,
                            !selected)) {
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
}

void Game::renderRunMenu(ImGuiIO& io) {
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
}

void Game::render() {
  window_->clear();
  render_texture_->clear(sf::Color::White);

  if (!states_.empty()) {
    // DOCKING STUFFS
    static bool opt_dockspace = true;
    static bool opt_padding = false;
    static bool opt_fullscreen = true;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent
    // window not dockable into, because it would be confusing to have two
    // docking targets within each others.
    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen) {
      const ImGuiViewport* viewport = ImGui::GetMainViewport();
      ImGui::SetNextWindowPos(viewport->WorkPos);
      ImGui::SetNextWindowSize(viewport->WorkSize);
      ImGui::SetNextWindowViewport(viewport->ID);
      ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
      ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
      window_flags |= ImGuiWindowFlags_NoTitleBar |
                      ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                      ImGuiWindowFlags_NoMove;
      window_flags |=
          ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    } else {
      dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
    }

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
      window_flags |= ImGuiWindowFlags_NoBackground;

    if (!opt_padding)
      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &opt_dockspace, window_flags);
    if (!opt_padding) ImGui::PopStyleVar();
    if (opt_fullscreen) ImGui::PopStyleVar(2);

    // Submit the DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
      ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
      ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    if (ImGui::BeginMenuBar()) {
      if (ImGui::BeginMenu("File")) {
        renderNewPlannerMenu();
        ImGui::Separator();
        if (ImGui::MenuItem("Exit", NULL, false)) window_->close();
        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("View")) {
        ImGui::MenuItem("Show Control Panel", nullptr, &show_control_panel_);
        ImGui::MenuItem("Show Console", nullptr, &show_console_);
        ImGui::MenuItem("Show Stats", nullptr, &show_stats_panel_);
        ImGui::EndMenu();
      }
      renderRunMenu(io);
      if (ImGui::BeginMenu("Help")) {
        ImGui::MenuItem("How To Use", nullptr, &show_how_to_use_window_);
        ImGui::MenuItem("About", nullptr, &show_about_window_);
        ImGui::EndMenu();
      }
      ImGui::EndMenuBar();
    }

    if (show_how_to_use_window_) {
      ImGui::Begin("How To Use");
      ImGui::Text("USAGE GUIDE:");
      ImGui::BulletText("Left-click to place/remove obstacle cells");
      ImGui::BulletText("Left-SHIFT+left-click to change starting cell");
      ImGui::BulletText("Left-CTRL+left-click to change end cell");
      ImGui::End();
    }

    if (show_about_window_) {
      ImGui::Begin("About");
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
      ImGui::End();
    }

    if (show_control_panel_) {
      ////////////////////////////////
      // Control Panel
      ////////////////////////////////
      ImGui::Begin("Control Panel");

      // render planner specific configurations
      states_.top()->renderConfig();

      ImGui::End();  // end Configurations
    }

    if (show_stats_panel_) {
      ImGui::Begin("Stats");
      ImGui::Text("Current Planner: %s", curr_planner_.c_str());
      ImGui::Spacing();
      ImGui::Spacing();
      ImGui::End();
    }

    //////////////////////////////////////////////////////////////////////
    //   Planning Scene Panel
    //////////////////////////////////////////////////////////////////////
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0.f, 0.f});
    ImGui::Begin("Planning Scene");

    const ImVec2 planning_scene_panel_size = ImGui::GetContentRegionAvail();
    render_texture_->create(static_cast<unsigned>(planning_scene_panel_size.x),
                            static_cast<unsigned>(planning_scene_panel_size.y));
    render_texture_->clear(sf::Color::White);

    sf::View view;
    view.setSize(
        sf::Vector2f(planning_scene_panel_size.x, planning_scene_panel_size.y));
    view.setCenter(
        sf::Vector2f((planning_scene_panel_size.x / 2.f) + view_move_xy_.x,
                     (planning_scene_panel_size.y / 2.f) + view_move_xy_.y));
    render_texture_->setView(view);
    states_.top()->renderScene(*render_texture_);

    ImGui::ImageButton(*render_texture_, 0);

    const bool is_hovered = ImGui::IsItemHovered();  // Hovered
    const bool is_active = ImGui::IsItemActive();    // Held

    // move the planning scene around by dragging mouse Right-click
    if (is_hovered && ImGui::IsMouseDragging(ImGuiMouseButton_Right)) {
      view_move_xy_.x -= io.MouseDelta.x;
      view_move_xy_.y -= io.MouseDelta.y;
    }

    // Update the current mouse position in planning scene panel
    ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();
    const ImVec2 origin(canvas_p0.x - view_move_xy_.x,
                        canvas_p0.y - view_move_xy_.y);
    mouse_pos_in_canvas_.x = io.MousePos.x - origin.x;
    mouse_pos_in_canvas_.y =
        io.MousePos.y - origin.y + planning_scene_panel_size.y;

    ImGui::End();
    ImGui::PopStyleVar();
    //////////////////////////////////////////////////////////////////////

    if (show_console_) {
      //////////////////////////
      //   Console Panel
      //////////////////////////
      ImGui::Begin("Console");
      ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.f);
      if (ImGui::Button("Clear##console_clear")) {
        logger_panel_->clear();
      }
      ImGui::PopStyleVar();
      ImGui::Spacing();
      ImGui::Separator();
      ImGui::End();
      logger_panel_->render("Console");
      //////////////////////////
    }

    ImGui::End();  // dockspace end

    // ImGui::ShowDemoWindow();

    ImGui::SFML::Render(*window_);
    window_->display();
    render_texture_->display();
  }
}

void Game::initGuiTheme() {
  // setup imgui style
  ImGuiIO& io = ImGui::GetIO();
  io.Fonts->AddFontFromFileTTF("../fonts/OpenSans/OpenSans-Regular.ttf", 18.0f);
  ImGui::SFML::UpdateFontTexture();

  // enable docking
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  ImGuiStyle* style = &ImGui::GetStyle();
  style->FramePadding = ImVec2(8.f, 4.f);

  // dark theme colors
  auto& colors = ImGui::GetStyle().Colors;
  colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.105f, 0.11f, 1.0f);

  // headers
  colors[ImGuiCol_Header] = ImVec4(0.2f, 0.205f, 0.21f, 1.0f);
  colors[ImGuiCol_HeaderHovered] = ImVec4(0.3f, 0.305f, 0.31f, 1.0f);
  colors[ImGuiCol_HeaderActive] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);

  // buttons
  colors[ImGuiCol_Button] = ImVec4(0.2f, 0.205f, 0.21f, 1.0f);
  colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.305f, 0.31f, 1.0f);
  colors[ImGuiCol_ButtonActive] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);

  // tabs
  colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
  colors[ImGuiCol_TabHovered] = ImVec4(0.38, 0.3805f, 0.381f, 1.0f);
  colors[ImGuiCol_TabActive] = ImVec4(0.28, 0.2805f, 0.281f, 1.0f);
  colors[ImGuiCol_TabUnfocused] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
  colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.2f, 0.205f, 0.21f, 1.0f);

  // frame background
  colors[ImGuiCol_FrameBg] = ImVec4(0.2f, 0.205f, 0.21f, 1.0f);
  colors[ImGuiCol_FrameBgHovered] = ImVec4(0.3f, 0.305f, 0.31f, 1.0f);
  colors[ImGuiCol_FrameBgActive] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);

  // sider
  colors[ImGuiCol_SliderGrab] = colors[ImGuiCol_Text];
  colors[ImGuiCol_SliderGrabActive] = colors[ImGuiCol_Text];

  // progress bar
  colors[ImGuiCol_PlotHistogram] = ImVec4(0.3f, 0.305f, 0.31f, 1.0f);

  // title
  colors[ImGuiCol_TitleBg] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
  colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
  colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
}

void Game::setGraphBasedPlanner(const int id) {
  switch (id) {
    case GRAPH_BASED_PLANNERS_IDS::BFS:
      // BFS
      states_.push(std::make_unique<bfs_state_type>(logger_panel_));
      break;
    case GRAPH_BASED_PLANNERS_IDS::DFS:
      // DFS
      states_.push(std::make_unique<dfs_state_type>(logger_panel_));
      break;
    case GRAPH_BASED_PLANNERS_IDS::DIJKSTRA:
      // Dijkstra
      states_.push(std::make_unique<dijkstra_state_type>(logger_panel_));
      break;
    case GRAPH_BASED_PLANNERS_IDS::AStar:
      // A-Star
      states_.push(std::make_unique<astar_state_type>(logger_panel_));
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
          logger_panel_, SAMPLING_BASED_PLANNERS[id]));
      break;
    case SAMPLING_BASED_PLANNERS_IDS::RRT_STAR:
      // RRTStar
      states_.push(std::make_unique<rrtstar_state_type>(
          logger_panel_, SAMPLING_BASED_PLANNERS[id]));
      break;
    default:
      break;
  }
}

}  // namespace path_finding_visualizer