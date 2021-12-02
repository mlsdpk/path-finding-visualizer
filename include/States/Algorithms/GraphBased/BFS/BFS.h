#pragma once

#include <queue>

#include "GraphBased.h"
#include "Node.h"

namespace path_finding_visualizer {
namespace graph_based {

class BFS : public GraphBased {
 public:
  // Constructor
  BFS(std::shared_ptr<gui::LoggerPanel> logger_panel);

  // Destructor
  virtual ~BFS();

  // override initialization Functions
  virtual void initAlgorithm() override;

  // override update functions
  virtual void updateNodes() override;

  // override render functions
  virtual void renderNodes(sf::RenderTexture &render_texture) override;
  virtual void renderParametersGui() override;

  // override main update function
  virtual void updatePlanner(bool &solved, Node &start_node,
                             Node &end_node) override;

 private:
  // BFS related
  std::queue<std::shared_ptr<Node>> frontier_;
};

}  // namespace graph_based
}  // namespace path_finding_visualizer