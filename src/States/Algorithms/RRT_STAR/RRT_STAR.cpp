#include "RRT_STAR.h"

// Constructor
RRT_STAR::RRT_STAR(sf::RenderWindow *window,
                   std::stack<std::unique_ptr<State>> &states)
    : SamplingBased(window, states, "RRT_STAR"),
      max_vertices_{5000},
      delta_q_{0.05},
      rewireFactor_{1.1},
      maxDistance_{0.1},
      goal_radius_{0.1} {
  initialize();
}

// Destructor
RRT_STAR::~RRT_STAR() {}

void RRT_STAR::initialize() {
  start_point_->x = 0.0;
  start_point_->y = 0.0;
  start_point_->parent = nullptr;

  goal_point_->x = 1.0;
  goal_point_->y = 1.0;
  goal_point_->parent = nullptr;

  vertices_.clear();
  edges_.clear();

  // r_rrt > [(2*(1+1/d))*(area/unitNBallVolume)]^(1/d)
  r_rrt_ =
      rewireFactor_ * std::pow(2 * (1.0 + 1.0 / 2.0) *
                                   (1.0 / boost::math::constants::pi<double>()),
                               1.0 / 2.0);
}

// override initAlgorithm() function
void RRT_STAR::initAlgorithm() {
  // clear all the vertices & edges
  vertices_.clear();
  edges_.clear();

  // add start point to vertices
  vertices_.emplace_back(start_point_);
}

void RRT_STAR::renderBackground() {
  window_->clear(BGN_COL);

  sf::RectangleShape rectangle(sf::Vector2f(900, 640));
  rectangle.setPosition(300, 60);
  rectangle.setFillColor(IDLE_COL);
  window_->draw(rectangle);
  window_->draw(titleText_);
}

void RRT_STAR::renderAlgorithm() {
  // render vertices & edges
  sf::CircleShape circle(obstSize_ / 12.0);
  circle.setOrigin(circle.getRadius(), circle.getRadius());
  circle.setFillColor(FRONTIER_COL);

  std::unique_lock<std::mutex> lck(mutex_);
  for (const auto &edge : edges_) {
    double p1_y = map_(edge.first->y, 0.0, 1.0, 0.0, 900.0);
    double p1_x = map_(edge.first->x, 0.0, 1.0, 0.0, 640.0);
    double p2_y = map_(edge.second->y, 0.0, 1.0, 0.0, 900.0);
    double p2_x = map_(edge.second->x, 0.0, 1.0, 0.0, 640.0);

    circle.setPosition(300 + p1_y, 60 + p1_x);
    window_->draw(circle);

    circle.setPosition(300 + p2_y, 60 + p2_x);
    window_->draw(circle);

    sf::Vertex line[] = {
        sf::Vertex(sf::Vector2f(300 + p1_y, 60 + p1_x), VISITED_COL),
        sf::Vertex(sf::Vector2f(300 + p2_y, 60 + p2_x), VISITED_COL)};

    window_->draw(line, 2, sf::Lines);
  }
  lck.unlock();

  // draw path if available
  if (goal_point_->parent) {
    std::shared_ptr<Point> current = goal_point_;
    while (current->parent && current != start_point_) {
      double p1_y = map_(current->y, 0.0, 1.0, 0.0, 900.0);
      double p1_x = map_(current->x, 0.0, 1.0, 0.0, 640.0);
      double p2_y = map_(current->parent->y, 0.0, 1.0, 0.0, 900.0);
      double p2_x = map_(current->parent->x, 0.0, 1.0, 0.0, 640.0);

      sf::Vertex line[] = {
          sf::Vertex(sf::Vector2f(300 + p1_y, 60 + p1_x), FONT_COL),
          sf::Vertex(sf::Vector2f(300 + p2_y, 60 + p2_x), FONT_COL)};

      window_->draw(line, 2, sf::Lines);
      current = current->parent;
    }
  }

  // draw start & goal points
  sf::CircleShape startGoalCircle(obstSize_ / 4.0);
  startGoalCircle.setOrigin(startGoalCircle.getRadius(),
                            startGoalCircle.getRadius());
  double start_y = map_(start_point_->y, 0.0, 1.0, 0.0, 900.0);
  double start_x = map_(start_point_->x, 0.0, 1.0, 0.0, 640.0);
  double goal_y = map_(goal_point_->y, 0.0, 1.0, 0.0, 900.0);
  double goal_x = map_(goal_point_->x, 0.0, 1.0, 0.0, 640.0);

  startGoalCircle.setPosition(300 + start_y, 60 + start_x);
  startGoalCircle.setFillColor(START_COL);
  window_->draw(startGoalCircle);

  startGoalCircle.setPosition(300 + goal_y, 60 + goal_x);
  startGoalCircle.setFillColor(END_COL);
  window_->draw(startGoalCircle);
}

void RRT_STAR::solveConcurrently(
    std::shared_ptr<Point> start_point, std::shared_ptr<Point> goal_point,
    std::shared_ptr<MessageQueue<bool>> message_queue) {
  // copy assignment
  // thread-safe due to shared_ptrs
  std::shared_ptr<Point> startPoint = start_point;
  std::shared_ptr<Point> goalPoint = goal_point;
  std::shared_ptr<MessageQueue<bool>> s_message_queue = message_queue;

  bool solved = false;

  double cycleDuration = 1;  // duration of a single simulation cycle in ms
  // init stop watch
  auto lastUpdate = std::chrono::system_clock::now();

  while (true) {
    // compute time difference to stop watch
    long timeSinceLastUpdate =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now() - lastUpdate)
            .count();

    if (timeSinceLastUpdate >= cycleDuration) {
      ////////////////////////////
      // run the main algorithm //
      ////////////////////////////

      Point x_rand;
      std::shared_ptr<Point> x_nearest = std::make_shared<Point>();
      std::shared_ptr<Point> x_new = std::make_shared<Point>();

      sample_free(x_rand);
      nearest(x_nearest, x_rand);
      steer(x_new, x_nearest, x_rand);

      if (!isCollision(x_nearest, x_new)) {
        std::vector<std::shared_ptr<Point>> X_near;
        near(X_near, x_new);

        std::unique_lock<std::mutex> lck(mutex_);
        vertices_.emplace_back(x_new);
        lck.unlock();

        ///////////////////
        // Choose parent //
        ///////////////////
        std::shared_ptr<Point> x_min = x_nearest;
        for (const auto &x_near : X_near) {
          double c_new = cost(x_near) + l2_Distance(x_near, x_new);
          if (c_new < cost(x_min) + l2_Distance(x_min, x_new)) {
            if (!isCollision(x_near, x_new)) {
              x_min = x_near;
            }
          }
        }
        x_new->parent = x_min;

        lck.lock();
        edges_.emplace_back(x_new->parent, x_new);
        lck.unlock();
        ////////////////////////////

        //////////////
        // Rewiring //
        //////////////
        for (const auto &x_near : X_near) {
          double c_near = cost(x_new) + l2_Distance(x_new, x_near);
          if (c_near < cost(x_near)) {
            if (!isCollision(x_near, x_new)) {
              lck.lock();
              edges_.erase(std::remove(edges_.begin(), edges_.end(),
                                       make_pair(x_near->parent, x_near)),
                           edges_.end());
              x_near->parent = x_new;
              edges_.emplace_back(x_new, x_near);
              lck.unlock();
            }
          }
        }
      }

      // only find path for fix amount of time
      std::unique_lock<std::mutex> lck(mutex_);
      if (vertices_.size() % 500 == 0) {
        // find nearest vertices
        std::vector<std::shared_ptr<Point>> nearestVertices;
        for (const auto &v : vertices_) {
          double dist = l2_Distance(v, goalPoint);
          if (dist < goal_radius_) {
            nearestVertices.emplace_back(v);
          }
        }

        if (nearestVertices.size() > 0) {
          std::shared_ptr<Point> bestVertex;
          double bestCost = std::numeric_limits<double>::infinity();
          for (const auto &v : nearestVertices) {
            double c = cost(v);
            if (c < bestCost) {
              bestCost = c;
              bestVertex = v;
            }
          }

          goalPoint->parent = bestVertex;
        }
      }

      // if number of vertices more than max vertices
      // stop the algorithm
      if (vertices_.size() > max_vertices_) {
        std::cout << "Vertices reach maximum limit." << '\n';
        solved = true;
      }
      lck.unlock();

      ////////////////////////////

      // reset stop watch for next cycle
      lastUpdate = std::chrono::system_clock::now();
    }

    // sends an update method to the message queue using move semantics
    auto ftr = std::async(std::launch::async, &MessageQueue<bool>::send,
                          s_message_queue, std::move(solved));
    ftr.wait();
    if (solved) return;

    std::lock_guard<std::mutex> lock(mutex_);
    if (Algorithm_stopped_) return;

    // sleep at every iteration to reduce CPU usage
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

void RRT_STAR::sample_free(Point &point) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(0, 1);

  point.x = dis(gen);
  point.y = dis(gen);
}

void RRT_STAR::nearest(std::shared_ptr<Point> &x_near, const Point &x_rand) {
  double minDist = std::numeric_limits<double>::infinity();

  std::lock_guard<std::mutex> lock(mutex_);

  for (const auto &v : vertices_) {
    double dist = std::sqrt((v->x - x_rand.x) * (v->x - x_rand.x) +
                            (v->y - x_rand.y) * (v->y - x_rand.y));
    if (dist < minDist) {
      minDist = dist;
      x_near = v;
    }
  }
}

void RRT_STAR::near(std::vector<std::shared_ptr<Point>> &X_near,
                    const std::shared_ptr<Point> &x_new) {
  std::lock_guard<std::mutex> lock(mutex_);

  double r = std::min(r_rrt_ * std::pow(log((double)(vertices_.size())) /
                                            ((double)(vertices_.size())),
                                        1.0 / 2.0),
                      maxDistance_);

  for (const auto &v : vertices_) {
    if (l2_Distance(v, x_new) < r) {
      X_near.emplace_back(v);
    }
  }
}

void RRT_STAR::steer(std::shared_ptr<Point> &x_new,
                     const std::shared_ptr<Point> &x_near,
                     const Point &x_rand) {
  // distance between x_near and x_rand
  double dist = sqrt((x_near->x - x_rand.x) * (x_near->x - x_rand.x) +
                     (x_near->y - x_rand.y) * (x_near->y - x_rand.y));

  // clip between delta_q and dist
  dist = std::min(delta_q_, dist);

  // angle(radians) between x_near and x_rand
  double angle = std::atan2(x_rand.y - x_near->y, x_rand.x - x_near->x);

  // get new node (x,y)
  x_new->x = x_near->x + dist * std::cos(angle);
  x_new->y = x_near->y + dist * std::sin(angle);
}

bool RRT_STAR::isCollision(const std::shared_ptr<Point> &x_near,
                           const std::shared_ptr<Point> &x_new) {
  // generate sample points along x_near and x_new
  double x0 = x_near->x;
  double y0 = x_near->y;
  double x1 = x_new->x;
  double y1 = x_new->y;

  double dist = std::sqrt((x0 - x1) * (x0 - x1) + (y0 - y1) * (y0 - y1));

  int n = static_cast<int>(round(dist / 0.01));

  std::vector<double> xs = linspace(x0, x1, n);
  std::vector<double> ys = linspace(y0, y1, n);

  std::lock_guard<std::mutex> lock(mutex_);

  for (auto i = 0; i < xs.size(); i++) {
    double pixel_y = map_(ys[i], 0.0, 1.0, 0.0, 900.0);
    double pixel_x = map_(xs[i], 0.0, 1.0, 0.0, 640.0);
    for (const auto &obst : obstacles_) {
      if (obst->getGlobalBounds().contains(
              sf::Vector2f(pixel_y + 300, pixel_x + 60))) {
        return true;
      }
    }
  }
  return false;
}

std::vector<double> RRT_STAR::linspace(double start, double end, int num) {
  std::vector<double> linspaced;

  if (num == 0) {
    linspaced.push_back(start);
    return linspaced;
  } else if (num == 1) {
    linspaced.push_back(start);
    linspaced.push_back(end);
    return linspaced;
  }

  double delta = (end - start) / (num);

  for (int i = 0; i < num; i++) {
    linspaced.push_back(start + delta * i);
  }
  linspaced.push_back(end);
  return linspaced;
}

double RRT_STAR::cost(std::shared_ptr<Point> p) {
  std::shared_ptr<Point> curr_p = std::move(p);
  double cost = 0.0;
  while (curr_p->parent) {
    cost += l2_Distance(curr_p, curr_p->parent);
    curr_p = curr_p->parent;
  }
  return cost;
}

double RRT_STAR::l2_Distance(const std::shared_ptr<Point> &p1,
                             const std::shared_ptr<Point> &p2) {
  return std::sqrt(std::pow(p2->x - p1->x, 2.0) + std::pow(p2->y - p1->y, 2.0));
}
