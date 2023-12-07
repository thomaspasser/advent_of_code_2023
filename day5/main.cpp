#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <numeric>
#include <ranges>
#include <sstream>
#include <vector>

struct Map {
  long int dst_range_start;
  long int src_range_start;
  long int len;
};

struct Range {
  long int start;
  long int len;
};

std::vector<Range> map_ranges(std::vector<Range> const &in_ranges,
                              std::vector<Map> const &maps) {
  std::vector<Range> in_ranges_copy = in_ranges;

  std::vector<Range> out_ranges;

  for (size_t i = 0; i < in_ranges_copy.size(); i++) {
    auto &in_range = in_ranges_copy[i];
    // auto len_rem = in_range.len;
    for (auto const &map : maps) {
      auto const in_range_end = in_range.start + in_range.len - 1;
      auto const map_src_range_end = map.src_range_start + map.len - 1;
      if ((in_range.start <= map_src_range_end) &&
          (map.src_range_start <= in_range_end)) {
        auto const start = std::max(in_range.start, map.src_range_start);
        auto const end = std::min(in_range_end, map_src_range_end);
        auto const len = end - start + 1;

        long int out_start = start - map.src_range_start + map.dst_range_start;
        out_ranges.push_back(Range(out_start, len));

        in_range.len = start - in_range.start;
        if (end < in_range_end) {
          // New range
          auto end_range = Range(end + 1, in_range_end - end);
          in_ranges_copy.push_back(end_range);
        }
      }
    }
  }

  for (auto const &in_range : in_ranges_copy) {
    if (in_range.len != 0) {
      out_ranges.push_back(in_range);
    }
  }

  return out_ranges;
}

std::pair<std::vector<Map>, size_t>
collect_maps(std::vector<std::string> const &lines, size_t start_line) {
  std::vector<Map> maps;
  size_t line_num = start_line; // start of seed-to-soil maps
  while (1) {
    if (line_num >= lines.size() || lines[line_num] == "")
      break;
    std::stringstream ss(lines[line_num]);

    long int num1, num2, num3;
    ss >> num1 >> num2 >> num3;
    maps.push_back(Map(num1, num2, num3));

    line_num++;
  }
  return {maps, line_num};
}

std::vector<Map> join_maps(std::vector<Map> const &a_to_b,
                           std::vector<Map> const &b_to_c) {
  /* Unused idea*/
  std::vector<Map> a_to_c;
  std::vector<Map> b_to_c_copy = b_to_c;

  std::vector<bool> matches_a_to_b(a_to_b.size(), false);
  // std::vector<bool> matches_b_to_c(b_to_c.size(), false);

  for (size_t i = 0; i < b_to_c_copy.size(); i++) {
    auto &map_bc = b_to_c_copy[i];
    for (size_t j = 0; j < a_to_b.size(); j++) {
      auto const &map_ab = a_to_b[j];
      // if (map_ab.dst_range_start + map_ab.len >= map_bc.src_range_start &&
      //     map_ab.dst_range_start < map_bc.src_range_start + map_bc.len) {
      if ((map_ab.dst_range_start <= map_bc.src_range_start &&
           map_bc.src_range_start < (map_ab.dst_range_start + map_ab.len)) ||
          (map_ab.dst_range_start <= (map_bc.src_range_start + map_bc.len) &&
           (map_bc.src_range_start + map_bc.len - 1) <
               (map_ab.dst_range_start + map_ab.len))) {
        matches_a_to_b[j] = true;
        // matches_b_to_c[i] = true;

        bool p1_in = (map_ab.dst_range_start >= map_bc.src_range_start);
        bool p2_in = map_ab.dst_range_start + map_ab.len <=
                     map_bc.src_range_start + map_bc.len;

        auto b_start = p1_in ? map_ab.dst_range_start : map_bc.src_range_start;
        auto a_start =
            b_start - map_ab.dst_range_start + map_ab.src_range_start;

        auto c_start =
            b_start - map_bc.src_range_start + map_bc.dst_range_start;

        auto b_end = p2_in ? map_ab.dst_range_start + map_ab.len
                           : map_bc.src_range_start + map_bc.len;
        auto len = b_end - b_start;
        if (len == 0) // could be handled before probably
          continue;

        a_to_c.push_back(Map(c_start, a_start, len));

        // Add the rest of ab
        if (!p1_in) {
          a_to_c.push_back(Map(map_ab.dst_range_start, map_ab.src_range_start,
                               b_start - map_ab.dst_range_start));
        }
        if (!p2_in) {
          a_to_c.push_back(Map(b_start + len, a_start + len,
                               map_ab.dst_range_start + map_ab.len - b_end));
        }

        // Add the rest of bc
        // if (p1_in) {
        //   if (map_bc.src_range_start + map_bc.len - b_end != 0)
        //     a_to_c.push_back(Map(c_start + len, b_end,
        //                          map_bc.src_range_start + map_bc.len -
        //                          b_end));
        // }
        // if (p2_in) {
        //   if (map_bc.src_range_start - b_start != 0)
        //     a_to_c.push_back(Map(map_bc.dst_range_start,
        //     map_bc.src_range_start,
        //                          b_start - map_bc.src_range_start));
        // }

        // No, just adjust bcs..
        // temporary to avoid invalidating map_bc..
        std::vector<Map> maps_to_add;

        // new one before?
        if (map_bc.src_range_start < b_start) {
          if (b_start - map_bc.src_range_start != 0)
            maps_to_add.push_back(Map(map_bc.dst_range_start,
                                      map_bc.src_range_start,
                                      b_start - map_bc.src_range_start));
        }
        // New one after?
        if (map_bc.src_range_start + map_bc.len > b_end) {
          maps_to_add.push_back(
              Map(c_start + len, b_start + len,
                  map_bc.src_range_start + map_bc.len - b_end));
        }
        map_bc.len = 0; // zero to indicate to ignore it later - it was
                        // replaced by the new ones
        for (auto const &adds : maps_to_add) {
          b_to_c_copy.push_back(adds);
        }
      }
    }
  }

  // Also add the remainder with no matches..
  for (auto const &map_bc : b_to_c_copy) {
    if (map_bc.len != 0) {
      a_to_c.push_back(map_bc);
    }
  }
  for (size_t j = 0; j < a_to_b.size(); j++) {
    if (!matches_a_to_b[j]) {
      a_to_c.push_back(a_to_b[j]);
    }
  }

  // Remove duplicates.. ?

  return a_to_c;
}

int main(int argc, char const *argv[]) {
  std::ifstream f("../../day5/input.txt");
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

  // Load seed numbers into vector
  std::string &seedline = lines[0];
  size_t colon_pos = seedline.find(':');
  std::string tmp;
  std::stringstream ss(seedline.substr(colon_pos + 2));
  std::vector<long int> seed_numbers;
  while (std::getline(ss, tmp, ' ')) {
    if (tmp.length() == 0)
      continue;
    seed_numbers.push_back(std::stol(tmp));
  }

  auto [seed_to_soil_maps, line_end1] = collect_maps(lines, 3);
  auto [soil_to_fertilizer_maps, line_end2] =
      collect_maps(lines, line_end1 + 2);
  auto [fertilizer_to_water_maps, line_end3] =
      collect_maps(lines, line_end2 + 2);
  auto [water_to_light_maps, line_end4] = collect_maps(lines, line_end3 + 2);
  auto [light_to_temperature_maps, line_end5] =
      collect_maps(lines, line_end4 + 2);
  auto [temperature_to_humidity_maps, line_end6] =
      collect_maps(lines, line_end5 + 2);
  auto [humidity_to_location_maps, _] = collect_maps(lines, line_end6 + 2);

  auto find_in_map = [](std::vector<Map> maps, long int in_num) {
    for (auto const &map : maps) {
      if (in_num >= map.src_range_start &&
          in_num < map.src_range_start + map.len) {
        return map.dst_range_start + (in_num - map.src_range_start);
      }
    }
    return in_num;
  };

  long int min_location{std::numeric_limits<long int>::max()};
  for (auto const &seed_num : seed_numbers) {
    long int const soil_num{find_in_map(seed_to_soil_maps, seed_num)};
    long int const fertilizer_num{
        find_in_map(soil_to_fertilizer_maps, soil_num)};
    long int const water_num{
        find_in_map(fertilizer_to_water_maps, fertilizer_num)};
    long int const light_num{find_in_map(water_to_light_maps, water_num)};
    long int const temperature_num{
        find_in_map(light_to_temperature_maps, light_num)};
    long int const humidity_num{
        find_in_map(temperature_to_humidity_maps, temperature_num)};
    long int const location_num{
        find_in_map(humidity_to_location_maps, humidity_num)};

    if (location_num < min_location) {
      min_location = location_num;
    }
  }
  std::cout << min_location << std::endl;

  // Part two
  std::vector<Range> seed_ranges;
  for (int i = 0; i < seed_numbers.size(); i += 2) {
    seed_ranges.push_back(Range(seed_numbers[i], seed_numbers[i + 1]));
  }

  auto soil_ranges = map_ranges(seed_ranges, seed_to_soil_maps);
  auto fertilizer_ranges = map_ranges(soil_ranges, soil_to_fertilizer_maps);
  auto water_ranges = map_ranges(fertilizer_ranges, fertilizer_to_water_maps);
  auto light_ranges = map_ranges(water_ranges, water_to_light_maps);
  auto temperature_ranges = map_ranges(light_ranges, light_to_temperature_maps);
  auto humidity_ranges =
      map_ranges(temperature_ranges, temperature_to_humidity_maps);
  auto location_ranges = map_ranges(humidity_ranges, humidity_to_location_maps);

  long int min_location2{std::numeric_limits<long int>::max()};
  for (auto const &loc_range : location_ranges) {
    if (loc_range.start < min_location2) {
      min_location2 = loc_range.start;
    }
  }
  std::cout << min_location2 << std::endl;

  // long int min_location2{std::numeric_limits<long int>::max()};
  // for (int i = 0; i < seed_numbers.size(); i += 2) {
  //   long int const start = seed_numbers[i];
  //   long int const length = seed_numbers[i + 1];

  //   for (long int j = start; j < start + length; j++) {
  //     long int const soil_num{find_in_map(seed_to_soil_maps, j)};
  //     long int const fertilizer_num{
  //         find_in_map(soil_to_fertilizer_maps, soil_num)};
  //     long int const water_num{
  //         find_in_map(fertilizer_to_water_maps, fertilizer_num)};
  //     long int const light_num{find_in_map(water_to_light_maps,
  //     water_num)}; long int const temperature_num{
  //         find_in_map(light_to_temperature_maps, light_num)};
  //     long int const humidity_num{
  //         find_in_map(temperature_to_humidity_maps, temperature_num)};
  //     long int const location_num{
  //         find_in_map(humidity_to_location_maps, humidity_num)};

  //     // std::cout << location_num << std::endl;
  //     if (location_num < min_location2) {
  //       min_location2 = location_num;
  //     }
  //   }
  // }

  // auto reverse_find_in_map = [](std::vector<Map> maps, long int in_num) {
  //   for (auto const &map : maps) {
  //     if (in_num >= map.dst_range_start &&
  //         in_num < map.dst_range_start + map.len) {
  //       return map.src_range_start + (in_num - map.dst_range_start);
  //     }
  //   }
  //   return in_num;
  // };

  // std::cout << min_location2 << std::endl;
  // std::sort(humidity_to_location_maps.begin(),
  // humidity_to_location_maps.end(),
  //           [](Map a, Map b) { return a.dst_range_start <
  //           b.dst_range_start;
  //           });

  // auto seed_to_fertilizer_maps =
  //     join_maps(seed_to_soil_maps, soil_to_fertilizer_maps);

  // auto seed_to_water_maps =
  //     join_maps(seed_to_fertilizer_maps, fertilizer_to_water_maps);

  // auto seed_to_light_maps = join_maps(seed_to_water_maps,
  // water_to_light_maps);

  // auto seed_to_temperature_maps =
  //     join_maps(seed_to_light_maps, light_to_temperature_maps);

  // auto seed_to_humidity_maps =
  //     join_maps(seed_to_temperature_maps, temperature_to_humidity_maps);

  // auto seed_to_location_maps =
  //     join_maps(seed_to_humidity_maps, humidity_to_location_maps);

  // Solve part 1 again

  // long int min_location2{std::numeric_limits<long int>::max()};
  // for (int seed_num = 0; seed_num < 200; seed_num++) {
  // for (auto const &seed_num : seed_numbers) {
  // long int const soil_num{find_in_map(seed_to_soil_maps, seed_num)};

  // long int const fertilizer_num{
  //     find_in_map(soil_to_fertilizer_maps, soil_num)};

  // long int const fertilizer_num2{
  //     find_in_map(seed_to_fertilizer_maps, seed_num)};

  // if (fertilizer_num != fertilizer_num2) {

  //   std::cout << "Seed: " << seed_num << " => Soil: " << soil_num
  //             << " => Fertilizer: " << fertilizer_num << " =? "
  //             << fertilizer_num2 << std::endl;
  // }

  //   long int const location_num{find_in_map(seed_to_location_maps,
  //   seed_num)};

  //   if (location_num < min_location2) {
  //     min_location2 = location_num;
  //   }
  // }
  // std::cout << min_location2 << std::endl;

  // int guess = 0;
  // while (1) {
  //   // Try to find reverse
  //   if (guess % 10000 == 0)
  //     std::cout << guess << std::endl;

  //   long int humidity_num =
  //       reverse_find_in_map(humidity_to_location_maps, guess);
  //   long int temperature_num =
  //       reverse_find_in_map(temperature_to_humidity_maps, humidity_num);
  //   long int light_num =
  //       reverse_find_in_map(light_to_temperature_maps, temperature_num);
  //   long int water_num = reverse_find_in_map(water_to_light_maps,
  //   light_num); long int fertilizer_num =
  //       reverse_find_in_map(fertilizer_to_water_maps, water_num);
  //   long int soil_num =
  //       reverse_find_in_map(soil_to_fertilizer_maps, fertilizer_num);
  //   long int seed_num = reverse_find_in_map(seed_to_soil_maps, soil_num);

  //   // std::cout << seed_num << std::endl;

  //   bool found = false;
  //   for (int i = 0; i < seed_numbers.size(); i += 2) {
  //     long int const start = seed_numbers[i];
  //     long int const length = seed_numbers[i + 1];
  //     if (seed_num >= start && seed_num < start + length) {
  //       std::cout << "Wohoo!" << std::endl;
  //       std::cout << guess << ": " << seed_num;
  //       found = true;
  //       break;
  //     }
  //   }
  //   if (found)
  //     break;
  //   guess++;
  // }
}
