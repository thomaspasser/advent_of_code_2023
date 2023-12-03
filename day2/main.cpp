#include <fstream>
#include <iostream>
#include <numeric>
#include <ranges>
#include <sstream>
#include <string>
#include <vector>

std::vector<std::string> const color_names{"red", "green", "blue"};
std::vector<int> const color_maxes{12, 13, 14};

bool valid_game(std::string const &line) {
  // Find colon in Game #:
  int const offset = line.find(":");

  std::istringstream stream1(line.substr(offset + 1));
  std::string draw, color;
  int n;
  while (std::getline(stream1, draw, ';')) {
    std::istringstream stream2(draw);
    std::string amount_color;
    while (std::getline(stream2, amount_color, ',')) {
      std::istringstream stream3(amount_color);
      stream3 >> n >> color;

      // Check if invalid
      for (size_t i = 0; auto const &c : color_names) {
        if (c == color) {
          if (n > color_maxes[i]) {
            return false;
          }
          break;
        }
        ++i;
      }
    }
  }
  // If we are still here, it is valid
  return true;
}

int game_power(std::string const &line) {
  std::array<int, 3> quantities{0, 0, 0};

  // Find colon in Game #:
  int const offset = line.find(":");

  std::istringstream stream1(line.substr(offset + 1));
  std::string draw, color;
  int n;
  while (std::getline(stream1, draw, ';')) {
    std::istringstream stream2(draw);
    std::string amount_color;
    while (std::getline(stream2, amount_color, ',')) {
      std::istringstream stream3(amount_color);
      stream3 >> n >> color;

      // Check if larger
      for (size_t i = 0; auto const &c : color_names) {
        if (c == color) {
          if (n > quantities[i]) {
            quantities[i] = n;
          }
          break;
        }
        ++i;
      }
    }
  }

  return quantities[0] * quantities[1] * quantities[2];
}

int main(int argc, char const *argv[]) {
  std::ifstream f("../../day2/input.txt");

  std::vector<std::string> lines;

  if (f.is_open()) {
    std::string line;
    while (std::getline(f, line)) {
      lines.push_back(line);
    }
    f.close();
  } else {
    std::cout << "File read problem" << std::endl;
  }

  // Part 1
  auto values = lines | std::ranges::views::transform(
                            [line_num = 1](std::string line) mutable {
                              const int res{valid_game(line) ? line_num : 0};
                              ++line_num;
                              return res;
                            });
  int const sum{std::accumulate(values.begin(), values.end(), 0)};

  // Part 2
  auto const values2{lines | std::ranges::views::transform(game_power)};
  int const sum2{std::accumulate(values2.begin(), values2.end(), 0)};

  std::cout << sum << std::endl;  // 2632
  std::cout << sum2 << std::endl; // 69629
}
