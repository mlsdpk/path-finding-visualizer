#include "RRT.h"

// Constructor
RRT::RRT(sf::RenderWindow *window, std::stack<std::unique_ptr<State>> &states)
    : SamplingBased(window, states, "RRT"),
      max_vertices_{2000},
      delta_q_{0.05},
      goal_radius_{0.1} {
  initialize();

  // read from config file and update the rrt parameters
  std::ifstream stream("../config/rrt.ini");
  std::string line;
  std::string key;
  std::string value;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "max_vertices")
          max_vertices_ = std::stof(value);
        else if (key == "delta_q")
          delta_q_ = std::stof(value);
        else if (key == "goal_radius")
          goal_radius_ = std::stof(value);
      }
    }
  }

  stream.close();

  initBackgroundText();
}

// Destructor
RRT::~RRT() {}

void RRT::initialize() {
  start_point_->x = 0.5;
  start_point_->y = 0.1;
  start_point_->parent = nullptr;

  goal_point_->x = 0.5;
  goal_point_->y = 0.9;
  goal_point_->parent = nullptr;

  vertices_.clear();
  edges_.clear();
}

// override initAlgorithm() function
void RRT::initAlgorithm() {
  // clear all the vertices & edges
  vertices_.clear();
  edges_.clear();

  // add start point to vertices
  vertices_.emplace_back(start_point_);
}

void RRT::initBackgroundText() {
  // Max vertices
  maxVerticesText_.setFont(font2_);
  maxVerticesText_.setFillColor(FONT_COL);
  maxVerticesText_.setString("MAX VERTICES: " + std::to_string(max_vertices_));
  maxVerticesText_.setCharacterSize(20);
  // sf::FloatRect textRect = maxVerticesText.getLocalBounds();
  // maxVerticesText.setOrigin(textRect.left + textRect.width / 2.0f,
  //                      textRect.top + textRect.height / 2.0f);
  maxVerticesText_.setPosition(sf::Vector2f(350, 17));

  // Max vertices
  noOfVerticesText_.setFont(font2_);
  noOfVerticesText_.setFillColor(FONT_COL);
  noOfVerticesText_.setString("NO: OF VERTICES: 0");
  noOfVerticesText_.setCharacterSize(20);
  // sf::FloatRect textRect = maxVerticesText.getLocalBounds();
  // maxVerticesText.setOrigin(textRect.left + textRect.width / 2.0f,
  //                      textRect.top + textRect.height / 2.0f);
  noOfVerticesText_.setPosition(sf::Vector2f(650, 17));
}

void RRT::updateBackgroundText() {
  maxVerticesText_.setString("MAX VERTICES: " + std::to_string(max_vertices_));

  std::unique_lock<std::mutex> lck(mutex_);
  noOfVerticesText_.setString("NO: OF VERTICES: " +
                              std::to_string(vertices_.size()));
  lck.unlock();
}

void RRT::renderBackground() {
  window_->clear(BGN_COL);

  sf::RectangleShape rectangle(sf::Vector2f(900, 640));
  rectangle.setPosition(300, 60);
  rectangle.setFillColor(IDLE_COL);
  window_->draw(rectangle);
  window_->draw(titleText_);

  updateBackgroundText();
  window_->draw(maxVerticesText_);
  window_->draw(noOfVerticesText_);
}

void RRT::renderAlgorithm() {
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

void RRT::solveConcurrently(std::shared_ptr<Point> start_point,
                            std::shared_ptr<Point> goal_point,
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
        x_new->parent = x_nearest;

        std::unique_lock<std::mutex> lck(mutex_);
        vertices_.emplace_back(x_new);
        edges_.emplace_back(x_nearest, x_new);
        lck.unlock();

        // check new node is inside goal radius
        double dist =
            sqrt((x_new->x - goalPoint->x) * (x_new->x - goalPoint->x) +
                 (x_new->y - goalPoint->y) * (x_new->y - goalPoint->y));

        // if distance is less than goal radius
        if (dist < goal_radius_) {
          goalPoint->parent = std::move(x_new);
          solved = true;
        }
      }

      std::unique_lock<std::mutex> lck(mutex_);
      if (vertices_.size() > max_vertices_ - 1) {
        std::cout << "Vertices reach maximum limit. Solution not found" << '\n';
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

void RRT::sample_free(Point &point) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(0, 1);

  point.x = dis(gen);
  point.y = dis(gen);
}

void RRT::nearest(std::shared_ptr<Point> &x_near, const Point &x_rand) {
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

void RRT::steer(std::shared_ptr<Point> &x_new,
                const std::shared_ptr<Point> &x_near, const Point &x_rand) {
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

bool RRT::isCollision(const std::shared_ptr<Point> &x_near,
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

std::vector<double> RRT::linspace(double start, double end, int num) {
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
