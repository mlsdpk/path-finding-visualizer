#pragma once

#include <cmath>

#include "States/Algorithms/GraphBased/Node.h"

namespace path_finding_visualizer {
namespace graph_based {
namespace utils {

inline double distanceCost(const Node &n1, const Node &n2) {
  return std::sqrt(
      (n1.getPos().x - n2.getPos().x) * (n1.getPos().x - n2.getPos().x) +
      (n1.getPos().y - n2.getPos().y) * (n1.getPos().y - n2.getPos().y));
}

inline double costToGoHeuristics(const Node &n1, const Node &n2,
                                 bool use_manhattan = false) {
  if (use_manhattan)
    return fabs(n1.getPos().x - n2.getPos().x) +
           fabs(n1.getPos().y - n2.getPos().y);

  return std::sqrt(
      (n1.getPos().x - n2.getPos().x) * (n1.getPos().x - n2.getPos().x) +
      (n1.getPos().y - n2.getPos().y) * (n1.getPos().y - n2.getPos().y));
}

inline void addNeighbours(std::vector<std::shared_ptr<Node>> &nodes,
                          const unsigned int node_idx, const unsigned int width,
                          const unsigned int height,
                          bool use_eight_connectivity = false) {
  // TODO: Add error handling here

  // setup
  const int size = width * height;
  const int cell_minus_width = node_idx - width;
  const int cell_plus_width = node_idx + width;
  const int cell_mod_width = node_idx % width;
  const int top_right = width - 1;

  unsigned int top_idx, bottom_idx;

  // if not at top-edge and bottom-edge, we add both top and bottom neighbours
  if (cell_minus_width >= 0 && cell_plus_width < size) {
    top_idx = cell_minus_width;
    bottom_idx = cell_plus_width;
    nodes[node_idx]->setNeighbours(nodes[top_idx]);
    nodes[node_idx]->setNeighbours(nodes[bottom_idx]);
  }
  // otherwise, it can be at either top-edge or bottom-edge
  else {
    // if at top-edge, only add bottom neighbour
    if (cell_minus_width < 0) {
      bottom_idx = cell_plus_width;
      nodes[node_idx]->setNeighbours(nodes[bottom_idx]);
    }
    // otherwise, add top neighbour
    else {
      top_idx = cell_minus_width;
      nodes[node_idx]->setNeighbours(nodes[top_idx]);
    }
  }

  // if the cell is not at the right-edge of 2D grid,
  // we find all the right, top-right and bottom-right neighbours
  if (cell_mod_width != top_right) {
    nodes[node_idx]->setNeighbours(nodes[node_idx + 1]);  // right neighbour

    if (use_eight_connectivity) {
      // now find if node is at top edge or bottom edge or otherwise
      if (cell_minus_width < 0) {  // if at top-edge
        nodes[node_idx]->setNeighbours(
            nodes[bottom_idx + 1]);  // bottom-right neighbour
      }
      // if at bottom-edge
      else if (cell_plus_width >= size) {
        nodes[node_idx]->setNeighbours(
            nodes[top_idx + 1]);  // top-right neighbour
      }
      // otherwise, we add both
      else {
        nodes[node_idx]->setNeighbours(
            nodes[top_idx + 1]);  // top-right neighbour
        nodes[node_idx]->setNeighbours(
            nodes[bottom_idx + 1]);  // bottom-right neighbour
      }
    }
  }

  // if the cell is not at the left-edge of 2D grid,
  // we find all the left, top-left and bottom-left neighbours
  if (cell_mod_width != 0) {
    nodes[node_idx]->setNeighbours(nodes[node_idx - 1]);  // left neighbour

    if (use_eight_connectivity) {
      // now find if node is at top edge or bottom edge or otherwise
      if (cell_minus_width < 0) {  // if at top-edge
        nodes[node_idx]->setNeighbours(
            nodes[bottom_idx - 1]);  // bottom-left neighbour
      }
      // if at bottom-edge
      else if (cell_plus_width >= size) {
        nodes[node_idx]->setNeighbours(
            nodes[top_idx - 1]);  // top-left neighbour
      }
      // otherwise, we add both
      else {
        nodes[node_idx]->setNeighbours(
            nodes[top_idx - 1]);  // top-right neighbour
        nodes[node_idx]->setNeighbours(
            nodes[bottom_idx - 1]);  // bottom-right neighbour
      }
    }
  }
}

}  // namespace utils
}  // namespace graph_based
}  // namespace path_finding_visualizer