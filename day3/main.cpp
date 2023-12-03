#include <fstream>
#include <iostream>
#include <numeric>
#include <ranges>
#include <string>
#include <vector>

char const *digits = "123456789";
std::string symbols{"*#&+$/@%=-"};

struct PointNumber {
  size_t index;
  size_t size;
  int value;
};

struct PointSymbol {
  size_t index;
  char symbol;
};

struct ParsedLine {
  std::vector<PointNumber> numbers;
  std::vector<PointSymbol> symbols;
};

auto parse_line(std::string line) {
  // Parse into two vector
  ParsedLine out;

  size_t pos = 0;
  while ((pos = line.find_first_of(digits, pos)) != std::string::npos) {
    int const num = atoi(&line[pos]);
    size_t const len = std::to_string(num).length();
    out.numbers.push_back({pos, len, num});
    pos += len;
  }

  pos = 0;
  while ((pos = line.find_first_of(symbols, pos)) != std::string::npos) {
    out.symbols.push_back({pos, line[pos]});
    pos += 1;
  }

  return out;
}

int line_partno_sum(ParsedLine const &line,
                    std::optional<ParsedLine *> prev_line,
                    std::optional<ParsedLine *> next_line) {
  int linesum = 0;
  for (auto const &num : line.numbers) {
    // Check same line
    if (std::find_if(line.symbols.cbegin(), line.symbols.cend(),
                     [num](PointSymbol const &sym) {
                       return (sym.index == num.index - 1) ||
                              (sym.index == num.index + num.size);
                     }) != line.symbols.cend()) {
      linesum += num.value;
    } else if (prev_line.has_value() &&
               std::find_if(prev_line.value()->symbols.cbegin(),
                            prev_line.value()->symbols.cend(),
                            [num](PointSymbol const &sym) {
                              size_t const start =
                                  num.index > 0 ? num.index - 1 : 0;
                              return (sym.index >= start) &&
                                     (sym.index <= num.index + num.size);
                            }) != prev_line.value()->symbols.cend()) {
      linesum += num.value;
    } else if (next_line.has_value() &&
               std::find_if(next_line.value()->symbols.cbegin(),
                            next_line.value()->symbols.cend(),
                            [num](PointSymbol const &sym) {
                              size_t const start =
                                  num.index > 0 ? num.index - 1 : 0;
                              return (sym.index >= start) &&
                                     (sym.index <= num.index + num.size);
                            }) != next_line.value()->symbols.cend()) {
      linesum += num.value;
    }
  }

  return linesum;
}

int line_gear_sum(ParsedLine const &line, std::optional<ParsedLine *> prev_line,
                  std::optional<ParsedLine *> next_line) {
  int linesum = 0;
  for (auto const &sym : line.symbols) {
    if (sym.symbol != '*') {
      continue;
    }
    // Find all numbers that are next to
    std::vector<int> next_to;
    for (auto const &num : line.numbers) {
      if (num.index + num.size == sym.index || num.index == sym.index + 1) {
        next_to.push_back(num.value);
      }
    }
    if (prev_line.has_value()) {
      for (auto const &num : prev_line.value()->numbers) {
        size_t const start = num.index > 0 ? num.index - 1 : 0;
        if (sym.index >= start && sym.index <= num.index + num.size) {
          next_to.push_back(num.value);
        }
      }
    }
    if (next_line.has_value()) {
      for (auto const &num : next_line.value()->numbers) {
        size_t const start = num.index > 0 ? num.index - 1 : 0;
        if (sym.index >= start && sym.index <= num.index + num.size) {
          next_to.push_back(num.value);
        }
      }
    }
    // If that is two numbers
    if (next_to.size() == 2) {
      linesum += next_to[0] * next_to[1];
    }
  }
  return linesum;
}

int main(int argc, char const *argv[]) {
  std::ifstream f("../../day3/input.txt");

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

  std::vector<ParsedLine> parsed_lines;
  std::transform(lines.cbegin(), lines.cend(), std::back_inserter(parsed_lines),
                 parse_line);
  int sum = 0;
  int sum2 = 0;
  for (size_t i = 0; i < parsed_lines.size(); i++) {
    auto prev = (i == 0) ? std::nullopt
                         : std ::optional<ParsedLine *>{&parsed_lines[i - 1]};
    auto next = (i == parsed_lines.size() - 1)
                    ? std::nullopt
                    : std ::optional<ParsedLine *>{&parsed_lines[i + 1]};
    sum += line_partno_sum(parsed_lines[i], prev, next);
    sum2 += line_gear_sum(parsed_lines[i], prev, next);
  }
  std::cout << sum << std::endl;
  std::cout << sum2 << std::endl;
}
