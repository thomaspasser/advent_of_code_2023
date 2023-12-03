#include <fstream>
#include <iostream>
#include <numeric>
#include <ranges>
#include <string>
#include <vector>

std::vector<std::string> numbers = {"one", "two",   "three", "four", "five",
                                    "six", "seven", "eight", "nine"};

int main(int argc, char const *argv[]) {
  std::ifstream f("../../day1/input.txt");

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

  auto line_to_value = [](std::string line) {
    auto digit_chars =
        line | std::ranges::views::filter([](char c) { return isdigit(c); });
    auto digits = digit_chars |
                  std::ranges::views::transform([](char c) { return c - '0'; });
    return digits.front() * 10 + digits.back();
  };

  auto const values = lines | std::ranges::views::transform(line_to_value);
  int const sum = std::accumulate(values.begin(), values.end(), 0);
  std::cout << sum << std::endl;

  // Part two
  auto line_to_value2 = [](std::string line) {
    std::vector<int> digits;
    for (size_t start_pos = 0; start_pos < line.length(); start_pos++) {
      if (isdigit(line[start_pos])) {
        digits.push_back(line[start_pos] - '0');
        continue;
      }
      for (size_t i = 0; auto number : numbers) {
        if (line.substr(start_pos, number.length()) == number) {
          digits.push_back(i + 1);
          break;
        }
        ++i;
      }
    }
    return (digits.front() * 10 + digits.back());
  };

  auto const values2 = lines | std::ranges::views::transform(line_to_value2);
  int const sum2 = std::accumulate(values2.begin(), values2.end(), 0);
  std::cout << sum2 << std::endl;
}
