#pragma once

#include <random>

#include "States/Algorithms/SamplingBased/RRT/RRT.h"

namespace path_finding_visualizer {
namespace sampling_based {

class RRT_STAR : public RRT {
 public:
  // Constructor
  RRT_STAR(std::shared_ptr<gui::LoggerPanel> logger_panel,
           const std::string& name);

  // Destructor
  virtual ~RRT_STAR();

  virtual void renderParametersGui() override;

  // override initialization functions
  virtual void initialize() override;
  virtual void initPlanner() override;
  virtual void initParameters() override;

  // override algorithm function
  virtual void updatePlanner(bool& solved, Vertex& start,
                             Vertex& goal) override;

  /**
   * @brief Find all the nearest neighbours inside the radius of particular
   * vertex provided
   * @param x_new Target vertex
   * @param X_near Vector of nearest neighbours
   */
  void near(const std::shared_ptr<const Vertex>& x_new,
            std::vector<std::shared_ptr<Vertex>>& X_near);

  /**
   * @brief Calculate r_rrt_ based on current measure
   */
  void updateRewiringLowerBounds();

 protected:
  /**
   * @brief Find best goal parent at every n iteration
   */
  unsigned int update_goal_every_;

  /**
   * @brief Rewiring lower bound constant
   */
  double r_rrt_;

  /**
   * @brief Rewiring factor
   */
  double rewire_factor_;

  /**
   * @brief Measure (i.e., n-dimensional volume) of the current state space
   */
  double current_measure_;

  /**
   * @brief Vertices that lie within the goal radius
   */
  std::vector<std::shared_ptr<Vertex>> x_soln_;
};

}  // namespace sampling_based
}  // namespace path_finding_visualizer