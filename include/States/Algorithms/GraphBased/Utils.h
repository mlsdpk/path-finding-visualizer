#pragma once

#include "States/Algorithms/GraphBased/Node.h"

namespace path_finding_visualizer {
namespace graph_based {
namespace utils {

inline double L1_Distance(const std::shared_ptr<Node> &n1,
                          const std::shared_ptr<Node> &n2) {
  return fabs(n1->getPos().x - n2->getPos().x) +
         fabs(n1->getPos().y - n2->getPos().y);
}

}  // namespace utils
}  // namespace graph_based
}  // namespace path_finding_visualizer