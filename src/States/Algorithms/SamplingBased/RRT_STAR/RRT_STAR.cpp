#include "States/Algorithms/SamplingBased/RRT_STAR/RRT_STAR.h"

namespace path_finding_visualizer {
namespace sampling_based {

// Constructor
RRT_STAR::RRT_STAR(std::shared_ptr<gui::LoggerPanel> logger_panel,
                   const std::string &name)
    : RRT(logger_panel, name) {
  initParameters();
  initialize();
}

// Destructor
RRT_STAR::~RRT_STAR() {}

void RRT_STAR::initParameters() {
  // initialize default planner related params
  // TODO: default values should be read from file
  max_iterations_ = 2500;
  interpolation_dist_ = 0.005;
  range_ = 0.05;
  goal_radius_ = 0.1;
  rewire_factor_ = 1.1;
  update_goal_every_ = 100u;
}

void RRT_STAR::initialize() {
  start_vertex_->x = 0.5;
  start_vertex_->y = 0.1;
  start_vertex_->parent = nullptr;

  goal_vertex_->x = 0.5;
  goal_vertex_->y = 0.9;
  goal_vertex_->parent = nullptr;

  vertices_.clear();
  edges_.clear();
  x_soln_.clear();

  // update rewiring lower bounds
  // r_rrt > [(2*(1+1/d))*(area/unitNBallVolume)]^(1/d)
  r_rrt_ = rewire_factor_ *
           std::pow(2 * (1.0 + 1.0 / 2.0) * (1.0 / M_PI), 1.0 / 2.0);
}

// override initPlanner() function
void RRT_STAR::initPlanner() {
  // clear all the vertices & edges
  vertices_.clear();
  edges_.clear();
  x_soln_.clear();
  // add start point to vertices
  vertices_.emplace_back(start_vertex_);
}

void RRT_STAR::renderParametersGui() {
  gui::inputDouble("range", &range_, 0.01, 1000.0, 0.01, 1.0,
                   "Maximum distance allowed between two vertices", "%.3f");
  gui::inputDouble("rewire_factor", &rewire_factor_, 1.0, 2.0, 0.01, 0.1,
                   "Rewiring factor", "%.2f");
  gui::inputDouble("goal_radius", &goal_radius_, 0.01, 1000.0, 0.01, 1.0,
                   "Distance between vertex and goal to stop planning", "%.3f");
}

void RRT_STAR::updatePlanner(bool &solved, Vertex &start, Vertex &goal) {
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

    // if this distance d > delta_q, we need to find nearest state in the
    // direction of x_rand
    if (d > range_) {
      interpolate(x_nearest, x_rand, range_ / d, x_new);
    } else {
      x_new->x = x_rand->x;
      x_new->y = x_rand->y;
    }

    if (!isCollision(x_nearest, x_new)) {
      // find all the nearest neighbours inside radius
      std::vector<std::shared_ptr<Vertex>> X_near;
      near(x_new, X_near);

      std::unique_lock<std::mutex> lck(mutex_);
      vertices_.emplace_back(x_new);
      lck.unlock();

      // choose parent
      std::shared_ptr<Vertex> x_min = x_nearest;
      for (const auto &x_near : X_near) {
        double c_new = cost(x_near) + distance(x_near, x_new);
        if (c_new < cost(x_min) + distance(x_min, x_new)) {
          if (!isCollision(x_near, x_new)) {
            x_min = x_near;
          }
        }
      }
      x_new->parent = x_min;

      lck.lock();
      edges_.emplace_back(x_new->parent, x_new);
      lck.unlock();

      // rewiring
      for (const auto &x_near : X_near) {
        double c_near = cost(x_new) + distance(x_new, x_near);
        if (c_near < cost(x_near)) {
          if (!isCollision(x_near, x_new)) {
            lck.lock();
            edges_.erase(std::remove(edges_.begin(), edges_.end(),
                                     std::make_pair(x_near->parent, x_near)),
                         edges_.end());
            x_near->parent = x_new;
            edges_.emplace_back(x_new, x_near);
            lck.unlock();
          }
        }
      }

      // add into x_soln if the vertex is within the goal radius
      if (inGoalRegion(x_new)) {
        x_soln_.emplace_back(x_new);
      }
    }

    // update the best parent for the goal vertex every n iterations
    iter_no_lck.lock();
    bool update_solution_path = (curr_iter_no_ % update_goal_every_ == 0);
    iter_no_lck.unlock();
    if (update_solution_path) {
      if (x_soln_.size() > 0) {
        std::shared_ptr<Vertex> best_goal_parent;
        double min_goal_parent_cost = std::numeric_limits<double>::infinity();

        for (const auto &v : x_soln_) {
          double c = cost(v);
          if (c < min_goal_parent_cost) {
            min_goal_parent_cost = c;
            best_goal_parent = v;
          }
        }
        goal.parent = best_goal_parent;
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

void RRT_STAR::near(const std::shared_ptr<const Vertex> &x_new,
                    std::vector<std::shared_ptr<Vertex>> &X_near) {
  std::lock_guard<std::mutex> lock(mutex_);
  double r = std::min(
      r_rrt_ * std::pow(std::log(static_cast<double>(vertices_.size())) /
                            (static_cast<double>(vertices_.size())),
                        1.0 / 2.0),
      range_);

  for (const auto &v : vertices_) {
    if (distance(v, x_new) < r) {
      X_near.emplace_back(v);
    }
  }
}

}  // namespace sampling_based
}  // namespace path_finding_visualizer