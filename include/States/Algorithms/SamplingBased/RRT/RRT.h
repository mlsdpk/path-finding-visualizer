#pragma once

#include <random>
#include <set>

#include "States/Algorithms/SamplingBased/SamplingBased.h"

namespace path_finding_visualizer {
namespace sampling_based {

class RRT : public SamplingBased {
 public:
  // Constructor
  RRT(std::shared_ptr<gui::LoggerPanel> logger_panel, const std::string &name);

  // Destructor
  virtual ~RRT();

  // override initialization functions
  virtual void initialize() override;
  virtual void initPlanner() override;
  virtual void initParameters() override;

  // override render functions
  virtual void renderPlannerData(sf::RenderTexture &render_texture) override;
  virtual void renderParametersGui() override;

  // override main update function
  virtual void updatePlanner(bool &solved, Vertex &start,
                             Vertex &goal) override;

  /**
   * @brief Randomly sample a vertex
   * @param v Sampled vertex
   */
  void sample(const std::shared_ptr<Vertex> &v);

  /**
   * @brief Find the nearest neighbour in a tree
   * @param v Nearest vertex
   */
  void nearest(const std::shared_ptr<const Vertex> &x_rand,
               std::shared_ptr<Vertex> &x_near);

  /**
   * @brief The cost to come of a vertex (g-value)
   */
  double cost(std::shared_ptr<Vertex> v);

  /**
   * @brief The euclidean distance between two vertices
   */
  double distance(const std::shared_ptr<const Vertex> &v1,
                  const std::shared_ptr<const Vertex> &v2);

  /**
   * @brief Check whether collision or not between two vertices
   * This function assumes from_v vertex is collision-free
   * @param from_v Starting vertex
   * @param to_v Ending vertex
   * @return true if there is a collision otherwise false
   */
  bool isCollision(const std::shared_ptr<const Vertex> &from_v,
                   const std::shared_ptr<const Vertex> &to_v);

  /**
   * @brief Find the new interpolated vertex from from_v vertex to to_v
   * vertex
   * @param from_v Starting vertex
   * @param to_v Ending vertex
   * @param t Interpolation distance
   * @param v New vertex
   */
  void interpolate(const std::shared_ptr<const Vertex> &from_v,
                   const std::shared_ptr<const Vertex> &to_v, const double t,
                   const std::shared_ptr<Vertex> &v);

  /**
   * @brief Check whether a vertex lies within goal radius or not
   */
  bool inGoalRegion(const std::shared_ptr<const Vertex> &v);

 protected:
  /**
   * @brief Interpolation distance during collsion checking
   */
  double interpolation_dist_;

  /**
   * @brief Maximum distance allowed between two vertices
   */
  double range_;

  /**
   * @brief Distance between vertex and goal
   */
  double goal_radius_;
};

}  // namespace sampling_based
}  // namespace path_finding_visualizer