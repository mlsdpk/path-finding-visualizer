#include "States/Algorithms/GraphBased/BFS/BFS.h"

namespace path_finding_visualizer {
namespace graph_based {

// Constructor
BFS::BFS(std::shared_ptr<gui::LoggerPanel> logger_panel)
    : GraphBased(logger_panel) {}

// Destructor
BFS::~BFS() {}

// override initAlgorithm() function
void BFS::initAlgorithm() {
  // initialize BFS by clearing frontier and add start node
  while (!frontier_.empty()) {
    frontier_.pop();
  }

  frontier_.push(nodeStart_);
}

// override updateNodes() function
void BFS::updateNodes() {
  if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && getKeyTime()) {
    int localY = ((mousePositionWindow_.x - init_grid_xy_.x) / grid_size_);
    int localX = ((mousePositionWindow_.y - init_grid_xy_.y) / grid_size_);

    if (localX >= 0 && localX < map_height_ / grid_size_) {
      if (localY >= 0 && localY < map_width_ / grid_size_) {
        // get the selected node
        std::shared_ptr<Node> selectedNode =
            nodes_[(map_width_ / grid_size_) * localX + localY];

        // check the position is Obstacle free or not
        bool isObstacle = false;
        if (selectedNode->isObstacle()) {
          isObstacle = true;
        }

        if (!is_solved_) {
          if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
            if (!isObstacle) {
              if (selectedNode != nodeEnd_) {
                nodeStart_->setStart(false);
                nodeStart_ = selectedNode;
                nodeStart_->setStart(true);
              }
            }
          } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
            if (!isObstacle) {
              if (selectedNode != nodeStart_) {
                nodeEnd_->setGoal(false);
                nodeEnd_ = selectedNode;
                nodeEnd_->setGoal(true);
              }
            }
          } else {
            selectedNode->setObstacle(!isObstacle);
          }
        } else {
          if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
            if (!isObstacle) {
              if (selectedNode != nodeStart_) {
                nodeEnd_->setGoal(false);
                nodeEnd_ = selectedNode;
                nodeEnd_->setGoal(true);
              }
            }
          }
        }
      }
    }
  }
}

// override renderNodes() function
void BFS::renderNodes(sf::RenderTexture &render_texture) {
  const auto texture_size = render_texture.getSize();

  init_grid_xy_.x = (texture_size.x / 2.) - (map_width_ / 2.);
  init_grid_xy_.y = (texture_size.y / 2.) - (map_height_ / 2.);

  for (int x = 0; x < map_height_ / grid_size_; x++) {
    for (int y = 0; y < map_width_ / grid_size_; y++) {
      float size = static_cast<float>(grid_size_);
      sf::RectangleShape rectangle(sf::Vector2f(size, size));
      rectangle.setOutlineThickness(2.f);
      rectangle.setOutlineColor(BGN_COL);
      rectangle.setPosition(init_grid_xy_.x + y * size,
                            init_grid_xy_.y + x * size);

      int nodeIndex = (map_width_ / grid_size_) * x + y;

      if (nodes_[nodeIndex]->isObstacle()) {
        rectangle.setFillColor(OBST_COL);
      } else if (nodes_[nodeIndex]->isPath()) {
        rectangle.setFillColor(PATH_COL);
        nodes_[nodeIndex]->setPath(false);
      } else if (nodes_[nodeIndex]->isFrontier()) {
        rectangle.setFillColor(FRONTIER_COL);
      } else if (nodes_[nodeIndex]->isVisited()) {
        rectangle.setFillColor(VISITED_COL);
      } else {
        rectangle.setFillColor(IDLE_COL);
      }

      if (nodes_[nodeIndex]->isStart()) {
        rectangle.setFillColor(START_COL);
      } else if (nodes_[nodeIndex]->isGoal()) {
        rectangle.setFillColor(END_COL);
      }
      render_texture.draw(rectangle);
    }
  }

  // visualizing path
  if (nodeEnd_ != nullptr) {
    std::shared_ptr<Node> current = nodeEnd_;
    while (current->getParentNode() != nullptr && !current->isStart()) {
      current->setPath(true);
      current = current->getParentNode();
    }
  }
}

void BFS::renderParametersGui() {}

void BFS::updatePlanner(bool &solved, Node &start_node, Node &end_node) {
  if (!frontier_.empty()) {
    std::shared_ptr<Node> node_current = frontier_.front();
    node_current->setFrontier(false);
    frontier_.pop();

    if (node_current->isGoal()) {
      solved = true;
    }

    for (auto node_neighbour : *node_current->getNeighbours()) {
      if (!node_neighbour->isVisited() && node_neighbour->isObstacle() == 0) {
        node_neighbour->setParentNode(node_current);
        node_neighbour->setVisited(true);
        node_neighbour->setFrontier(true);
        frontier_.push(node_neighbour);
      }
    }
  } else {
    solved = true;
  }
}

}  // namespace graph_based
}  // namespace path_finding_visualizer