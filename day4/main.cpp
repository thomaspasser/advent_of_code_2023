#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>
#include <ranges>
#include <sstream>
#include <vector>

int main(int argc, char const *argv[]) {
  std::ifstream f("../../day4/input.txt");
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

  constexpr auto line_sum{[](std::string const &line) {
    // Find : and |
    size_t const colon_pos{line.find(':')};
    size_t const bar_pos{line.find('|')};

    std::string tmp;
    std::stringstream ss(line.substr(colon_pos + 2, bar_pos - colon_pos - 2));
    std::vector<int> winning_numbers;
    while (std::getline(ss, tmp, ' ')) {
      if (tmp.length() == 0)
        continue;
      winning_numbers.push_back(std::stoi(tmp));
    }

    std::vector<int> your_numbers;
    std::stringstream ss2(line.substr(bar_pos + 2));
    while (std::getline(ss2, tmp, ' ')) {
      if (tmp.length() == 0)
        continue;
      your_numbers.push_back(std::stoi(tmp));
    }

    long int const num_wins{std::count_if(
        your_numbers.begin(), your_numbers.end(), [winning_numbers](int num) {
          return std::find(winning_numbers.begin(), winning_numbers.end(),
                           num) != winning_numbers.end();
        })};

    return num_wins;
  }};

  auto const num_wins{lines | std::ranges::views::transform(line_sum)};
  auto const values{num_wins | std::ranges::views::transform([](int num_win) {
                      return num_win > 0 ? std::pow(2, num_win - 1)
                                         : 0; // 0, 1, 2, 4, 8, 16
                    })};
  int const sum{std::accumulate(values.begin(), values.end(), 0)};

  std::cout << sum << std::endl;

  // Part two
  std::vector<int> card_quant(num_wins.size(), 1);
  for (size_t i{0}; i < num_wins.size(); i++) {
    auto const add_val{card_quant[i]};
    auto const start{card_quant.begin() + i + 1};
    auto const end{(start + num_wins[i] >= card_quant.end())
                       ? card_quant.end()
                       : start + num_wins[i]};
    std::for_each(start, end, [add_val](int &q) { q += add_val; });
  }

  int const sum2{std::accumulate(card_quant.begin(), card_quant.end(), 0)};
  std::cout << sum2 << std::endl;
}
