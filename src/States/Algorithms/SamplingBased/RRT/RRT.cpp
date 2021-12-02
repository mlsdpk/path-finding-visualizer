#include "States/Algorithms/SamplingBased/RRT/RRT.h"

namespace path_finding_visualizer {
namespace sampling_based {

// Constructor
RRT::RRT(std::shared_ptr<gui::LoggerPanel> logger_panel,
         const std::string &name)
    : SamplingBased(logger_panel, name) {
  initParameters();
  initialize();
}

// Destructor
RRT::~RRT() {}

void RRT::initParameters() {
  // initialize default planner related params
  // TODO: default values should be read from file
  max_iterations_ = 1000;
  interpolation_dist_ = 0.005;
  range_ = 0.05;
  goal_radius_ = 0.1;
}

void RRT::initialize() {
  start_vertex_->x = 0.5;
  start_vertex_->y = 0.1;
  start_vertex_->parent = nullptr;

  goal_vertex_->x = 0.5;
  goal_vertex_->y = 0.9;
  goal_vertex_->parent = nullptr;

  vertices_.clear();
  edges_.clear();
}

// override initPlanner() function
void RRT::initPlanner() {
  // clear all the vertices & edges
  vertices_.clear();
  edges_.clear();

  // add start vertex to vertices
  vertices_.emplace_back(start_vertex_);
}

void RRT::renderPlannerData(sf::RenderTexture &render_texture) {
  // render edges
  std::unique_lock<std::mutex> lck(mutex_);
  for (const auto &edge : edges_) {
    double p1_y = utils::map(edge.first->y, 0.0, 1.0, init_grid_xy_.x,
                             init_grid_xy_.x + map_width_);
    double p1_x = utils::map(edge.first->x, 0.0, 1.0, init_grid_xy_.y,
                             init_grid_xy_.y + map_height_);
    double p2_y = utils::map(edge.second->y, 0.0, 1.0, init_grid_xy_.x,
                             init_grid_xy_.x + map_width_);
    double p2_x = utils::map(edge.second->x, 0.0, 1.0, init_grid_xy_.y,
                             init_grid_xy_.y + map_height_);

    sf::Vertex line[] = {sf::Vertex(sf::Vector2f(p1_y, p1_x), EDGE_COL),
                         sf::Vertex(sf::Vector2f(p2_y, p2_x), EDGE_COL)};

    render_texture.draw(line, 2, sf::Lines);
  }
  lck.unlock();

  // render path if available
  if (goal_vertex_->parent) {
    std::shared_ptr<Vertex> current = goal_vertex_;
    while (current->parent && current != start_vertex_) {
      double p1_y = utils::map(current->y, 0.0, 1.0, init_grid_xy_.x,
                               init_grid_xy_.x + map_width_);
      double p1_x = utils::map(current->x, 0.0, 1.0, init_grid_xy_.y,
                               init_grid_xy_.y + map_height_);
      double p2_y = utils::map(current->parent->y, 0.0, 1.0, init_grid_xy_.x,
                               init_grid_xy_.x + map_width_);
      double p2_x = utils::map(current->parent->x, 0.0, 1.0, init_grid_xy_.y,
                               init_grid_xy_.y + map_height_);

      utils::sfPath path(sf::Vector2f(p1_y, p1_x), sf::Vector2f(p2_y, p2_x),
                         4.f, PATH_COL);
      render_texture.draw(path);
      current = current->parent;
    }
  }

  // render start & goal vertices
  sf::CircleShape start_goal_circle(obst_size_ / 2.0);
  start_goal_circle.setOrigin(start_goal_circle.getRadius(),
                              start_goal_circle.getRadius());
  double start_y = utils::map(start_vertex_->y, 0.0, 1.0, init_grid_xy_.x,
                              init_grid_xy_.x + map_width_);
  double start_x = utils::map(start_vertex_->x, 0.0, 1.0, init_grid_xy_.y,
                              init_grid_xy_.y + map_height_);
  double goal_y = utils::map(goal_vertex_->y, 0.0, 1.0, init_grid_xy_.x,
                             init_grid_xy_.x + map_width_);
  double goal_x = utils::map(goal_vertex_->x, 0.0, 1.0, init_grid_xy_.y,
                             init_grid_xy_.y + map_height_);

  start_goal_circle.setPosition(start_y, start_x);
  start_goal_circle.setFillColor(START_COL);
  render_texture.draw(start_goal_circle);

  start_goal_circle.setPosition(goal_y, goal_x);
  start_goal_circle.setFillColor(GOAL_COL);
  render_texture.draw(start_goal_circle);
}

void RRT::renderParametersGui() {
  gui::inputDouble("range", &range_, 0.01, 1000.0, 0.01, 1.0,
                   "Maximum distance allowed between two vertices", "%.3f");
  gui::inputDouble("goal_radius", &goal_radius_, 0.01, 1000.0, 0.01, 1.0,
                   "Distance between vertex and goal to stop planning", "%.3f");
}

void RRT::updatePlanner(bool &solved, Vertex &start, Vertex &goal) {
  std::unique_lock<std::mutex> iter_no_lck(iter_no_mutex_);
  bool running = (curr_iter_no_ < max_iterations_);
  iter_no_lck.unlock();

  if (running) {
    std::shared_ptr<Vertex> x_rand = std::make_shared<Vertex>();
    std::shared_ptr<Vertex> x_nearest = std::make_shared<Vertex>();
    std::shared_ptr<Vertex> x_new = std::make_shared<Vertex>();

    sample(x_rand);
    nearest(x_rand, x_nearest);

    // find the distance between x_rand and x_nearest
    double d = distance(x_rand, x_nearest);

    // if this distance d > max_distance_, we need to find nearest state in
    // the direction of x_rand
    if (d > range_) {
      interpolate(x_nearest, x_rand, range_ / d, x_new);
    } else {
      x_new->x = x_rand->x;
      x_new->y = x_rand->y;
    }

    if (!isCollision(x_nearest, x_new)) {
      x_new->parent = x_nearest;

      std::unique_lock<std::mutex> lck(mutex_);
      vertices_.emplace_back(x_new);
      edges_.emplace_back(x_nearest, x_new);
      lck.unlock();

      if (inGoalRegion(x_new)) {
        goal.parent = std::move(x_new);
        solved = true;
      }
    }

    iter_no_lck.lock();
    curr_iter_no_++;
    iter_no_lck.unlock();
  } else {
    std::cout << "Iterations number reach max limit. Planning stopped." << '\n';
    solved = true;
  }
}

void RRT::sample(const std::shared_ptr<Vertex> &v) {
  std::uniform_real_distribution<> dis(0, 1);
  v->x = dis(rn_gen_);
  v->y = dis(rn_gen_);
}

bool RRT::isCollision(const std::shared_ptr<const Vertex> &from_v,
                      const std::shared_ptr<const Vertex> &to_v) {
  // check collison from from_v to to_v
  // interpolate vertices between from_v and to_v
  // assume from_v is collision free

  const double max_dist = distance(from_v, to_v);

  double d = interpolation_dist_;
  while (d < max_dist) {
    std::shared_ptr<Vertex> temp_v = std::make_shared<Vertex>();
    interpolate(from_v, to_v, d / max_dist, temp_v);

    double pixel_y = utils::map(temp_v->y, 0.0, 1.0, 0.0, map_width_);
    double pixel_x = utils::map(temp_v->x, 0.0, 1.0, 0.0, map_height_);
    for (const auto &obst : obstacles_) {
      if (obst->getGlobalBounds().contains(sf::Vector2f(pixel_y, pixel_x))) {
        return true;
      }
    }
    d += interpolation_dist_;
  }

  // now we check the destination vertex to_v
  double pixel_y = utils::map(to_v->y, 0.0, 1.0, 0.0, map_width_);
  double pixel_x = utils::map(to_v->x, 0.0, 1.0, 0.0, map_height_);
  for (const auto &obst : obstacles_) {
    if (obst->getGlobalBounds().contains(sf::Vector2f(pixel_y, pixel_x))) {
      return true;
    }
  }

  return false;
}

void RRT::interpolate(const std::shared_ptr<const Vertex> &from_v,
                      const std::shared_ptr<const Vertex> &to_v, const double t,
                      const std::shared_ptr<Vertex> &v) {
  v->x = from_v->x + (to_v->x - from_v->x) * t;
  v->y = from_v->y + (to_v->y - from_v->y) * t;
}

void RRT::nearest(const std::shared_ptr<const Vertex> &x_rand,
                  std::shared_ptr<Vertex> &x_near) {
  double minDist = std::numeric_limits<double>::infinity();

  for (const auto &v : vertices_) {
    double dist = distance(v, x_rand);
    if (dist < minDist) {
      minDist = dist;
      x_near = v;
    }
  }
}

double RRT::cost(std::shared_ptr<Vertex> v) {
  std::shared_ptr<Vertex> curr_p = std::move(v);
  double cost = 0.0;
  while (curr_p->parent) {
    cost += distance(curr_p, curr_p->parent);
    curr_p = curr_p->parent;
  }
  return cost;
}

double RRT::distance(const std::shared_ptr<const Vertex> &v1,
                     const std::shared_ptr<const Vertex> &v2) {
  return std::sqrt((v1->x - v2->x) * (v1->x - v2->x) +
                   (v1->y - v2->y) * (v1->y - v2->y));
}

bool RRT::inGoalRegion(const std::shared_ptr<const Vertex> &v) {
  if (distance(v, goal_vertex_) <= goal_radius_) return true;
  return false;
}

}  // namespace sampling_based
}  // namespace path_finding_visualizer
