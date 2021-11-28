#pragma once

#include <cmath>
#include <vector>

namespace path_finding_visualizer {
namespace sampling_based {
namespace utils {

inline double map(double x, double in_min, double in_max, double out_min,
                  double out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

inline std::vector<double> linspace(double start, double end, int num) {
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

class sfPath : public sf::Drawable {
 public:
  sfPath(const sf::Vector2f& point1, const sf::Vector2f& point2,
         float thickness, sf::Color color)
      : color(color), thickness(thickness) {
    sf::Vector2f direction = point2 - point1;
    sf::Vector2f unitDirection =
        direction /
        std::sqrt(direction.x * direction.x + direction.y * direction.y);
    sf::Vector2f unitPerpendicular(-unitDirection.y, unitDirection.x);

    sf::Vector2f offset = (thickness / 2.f) * unitPerpendicular;

    vertices[0].position = point1 + offset;
    vertices[1].position = point2 + offset;
    vertices[2].position = point2 - offset;
    vertices[3].position = point1 - offset;

    for (int i = 0; i < 4; ++i) vertices[i].color = color;
  }

  void draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(vertices, 4, sf::Quads);
  }

 private:
  sf::Vertex vertices[4];
  float thickness;
  sf::Color color;
};

}  // namespace utils
}  // namespace sampling_based
}  // namespace path_finding_visualizer