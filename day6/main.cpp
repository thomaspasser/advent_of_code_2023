#include <fstream>
#include <iostream>
#include <numeric>
#include <ranges>
#include <sstream>
#include <string>
#include <vector>

int main(int argc, char const *argv[]) {
  std::ifstream f("../../day6/input.txt");

  std::string line1, line2;
  std::getline(f, line1);
  std::getline(f, line2);

  std::istringstream stream1(line1);

  std::string temp;
  stream1 >> temp;

  std::vector<long int> times;
  std::vector<long int> distances;
  long int buf;
  while (stream1 >> buf) {
    times.push_back(buf);
  }

  std::istringstream stream2(line2);
  stream2 >> line2;
  while (stream2 >> buf) {
    distances.push_back(buf);
  }

  // Can't be bothered to merge numbers for part two
  // We just adjust the input file.
  long int product{1};
  for (long int i = 0; i < times.size(); i++) {
    long int n_further = 0;
    for (long int j = 1; j < times[i]; j++) {
      long int speed = j;
      long int dist = (times[i] - j) * speed;
      if (dist > distances[i]) {
        n_further++;
      }
    }

    std::cout << n_further << " ways to beat." << std::endl;
    product *= n_further;
  }

  std::cout << product << std::endl;
}
