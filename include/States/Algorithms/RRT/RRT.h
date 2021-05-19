#pragma once

#include <random>
#include <set>

#include "SamplingBased.h"

class RRT : public SamplingBased {
 private:
  // vertices & edges
  std::vector<std::shared_ptr<Point>> vertices_;
  std::vector<std::pair<std::shared_ptr<Point>, std::shared_ptr<Point>>> edges_;

  int max_vertices_;
  double delta_q_;
  double goal_radius_;

 public:
  // Constructor
  RRT(sf::RenderWindow *window, std::stack<std::unique_ptr<State>> &states);

  // Destructor
  virtual ~RRT();

  // override initialization functions
  void initialize() override;
  void initAlgorithm() override;

  // override render functions
  void renderBackground() override;
  void renderAlgorithm() override;

  // override algorithm function
  void solveConcurrently(
      std::shared_ptr<Point> start_point, std::shared_ptr<Point> goal_point,
      std::shared_ptr<MessageQueue<bool>> message_queue) override;

  // new functions
  void sample_free(Point &point);
  void nearest(std::shared_ptr<Point> &x_near, const Point &x_rand);
  void steer(std::shared_ptr<Point> &x_new,
             const std::shared_ptr<Point> &x_near, const Point &x_rand);
  bool isCollision(const std::shared_ptr<Point> &x_near,
                   const std::shared_ptr<Point> &x_new);
  std::vector<double> linspace(double start, double end, int num);
};
