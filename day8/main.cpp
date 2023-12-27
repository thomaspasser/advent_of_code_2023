#include <fstream>
#include <iostream>
#include <numeric>
#include <ranges>
#include <set>
#include <sstream>
#include <string>
#include <vector>

struct Node {
  std::string start;
  std::string left_dest;
  std::string right_dest;
};

auto find_index(std::string const &name, std::vector<Node> const &nodes) {
  auto node =
      std::find_if(nodes.cbegin(), nodes.cend(),
                   [name](Node const &node) { return node.start == name; });
  if (node == nodes.cend()) {
    // None found, should not happen
    return -1;
  }
  return static_cast<int>(std::distance(nodes.cbegin(), node));
};

int count_steps(std::string dirs, std::vector<Node> const &nodes,
                std::string const &start, bool all_z) {

  // Find start node
  int node_idx = find_index(start, nodes);
  if (node_idx == -1) {
    return -1;
  }

  // Traverse from start_node until we find "ZZZ"
  int n_steps{0};
  int dirs_index{0};
  while (1) {
    Node const &loc = nodes[node_idx];
    auto const &search =
        (dirs[dirs_index] == 'L') ? loc.left_dest : loc.right_dest;
    n_steps++;
    node_idx = find_index(search, nodes);
    if (all_z && nodes[node_idx].start == "ZZZ" ||
        nodes[node_idx].start[2] == 'Z') {
      // std::cout << "End: " << nodes[node_idx].start << std::endl;
      break;
    }

    // Handle index into directions string `dirs`
    dirs_index++;
    if (dirs_index > dirs.size() - 1) {
      // Wrap back
      dirs_index = 0;
    }
  }
  return n_steps;
}
void part_one(std::string dirs, std::vector<Node> const &nodes) {
  int n_steps = count_steps(dirs, nodes, "AAA", true);
  std::cout << n_steps << std::endl;
}

int main(int argc, char const *argv[]) {
  std::ifstream f("../../day8/input.txt");

  std::string dirs;
  std::getline(f, dirs);

  // Parse file
  std::vector<Node> nodes;
  for (std::string temp; std::getline(f, temp);) {
    // So ugly..
    std::istringstream stream(temp);

    std::string start, eq, temp_left, temp_right;
    stream >> start >> eq >> temp_left >> temp_right;
    // std::cout << node << std::endl;

    if (eq.size() == 0) // should be '='
      continue;

    nodes.push_back({start, temp_left.substr(1, temp_left.size() - 2),
                     temp_right.substr(0, temp_left.size() - 2)});
  }

  part_one(dirs, nodes);

  // Part two
  // Find all start nodes
  // They all end with 'A'

  std::vector<int> idxs;
  auto it = nodes.cbegin();
  while ((it = std::find_if(it, nodes.cend(), [](Node const &node) {
            return node.start[2] == 'A';
          })) != nodes.end()) {
    idxs.push_back(std::distance(nodes.cbegin(), it));
    it++;
  }

  std::vector<int> n_steps;
  for (int const &j : idxs) {
    // std::cout << "Start: " << nodes[j].start << std::endl;
    n_steps.push_back(count_steps(dirs, nodes, nodes[j].start, false));
  }

  std::vector<int> last_done(idxs.size(), 0);

  // For some reason they loop immediately.
  // So we just need to find the LCM of the loop lengths
  long int prod{1};
  for (int const &n : n_steps) {
    prod = std::lcm(prod, n);
  }
  std::cout << prod << std::endl;

  // int it2 = 0;
  // int dirs_index2{0};
  // while (1) {
  //   // update all of them
  //   for (int i = 0; i < idxs.size(); i++) {
  //     int const &j = idxs[i];
  //     Node &loc = nodes[j];
  //     auto const &search =
  //         (dirs[dirs_index2] == 'L') ? loc.left_dest : loc.right_dest;
  //     idxs[i] = find_index(search, nodes);
  //   }

  //   it2++;

  //   // Check if all ends with Z
  //   bool all_done{true};
  //   for (int i = 0; i < idxs.size(); i++) {
  //     int const &j = idxs[i];
  //     if (nodes[j].start[2] != 'Z') {
  //       all_done = false;
  //     } else {
  //       std::cout << "it " << it2 << " Done: " << i << " " << nodes[j].start
  //                 << std::endl;
  //       std::cout << it2 - last_done[i] << " " << n_steps[i] << std::endl;
  //       last_done[i] = it2;
  //     }
  //   }
  //   if (all_done) {
  //     break;
  //   }
  //   // Handle index into directions string `dirs`
  //   dirs_index2++;
  //   if (dirs_index2 > dirs.size() - 1) {
  //     // Wrap back
  //     dirs_index2 = 0;
  //   }
  // }

  // why does looping start immediately
}
