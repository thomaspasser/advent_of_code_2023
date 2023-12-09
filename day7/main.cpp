#include <fstream>
#include <iostream>
#include <numeric>
#include <ranges>
#include <set>
#include <sstream>
#include <string>
#include <vector>

std::string card_order{"AKQJT98765432"};
std::string card_order_two{"AKQT98765432J"};

int const n_cards{5};

enum HandType {
  HIGH_CARD,
  ONE_PAIR,
  TWO_PAIR,
  THREE_OF_A_KIND,
  FULL_HOUSE,
  FOUR_OF_A_KIND,
  FIVE_OF_A_KIND
};

HandType determine_type(std::string cards) {
  std::set<char> set(cards.begin(), cards.end());
  size_t const n_unique{set.size()};

  // Check if five of a kind
  if (n_unique == 1) {
    return HandType::FIVE_OF_A_KIND;
  }

  // If four of a kind
  if (n_unique == 2) {
    // Can be four of a kind or full house
    for (auto const &sete : set) {
      int count{0};
      for (auto e : cards) {
        if (e == sete) {
          count++;
        }
      }
      if (count == 4) {
        return HandType::FOUR_OF_A_KIND;
      }
    }
    // Otherwise full house
    return HandType::FULL_HOUSE;
  }

  if (n_unique == 3) {
    // Three of a kind or two pairs
    // Check 3 of a kind
    // Check each if there are 3 of any of them
    for (auto const &sete : set) {
      int count{0};
      for (auto e : cards) {
        if (e == sete) {
          count++;
        }
      }
      if (count == 3) {
        return HandType::THREE_OF_A_KIND;
      }
    }
    // Check two pair
    return HandType::TWO_PAIR;
  }
  if (n_unique == 4) {
    return HandType::ONE_PAIR;
  }

  return HandType::HIGH_CARD;
}

HandType determine_type_part2(std::string cards) {
  // count jokers
  int n_jokers = std::count(cards.begin(), cards.end(), 'J');
  if (n_jokers == 0) {
    return determine_type(cards);
  }
  std::set<char> set(cards.begin(), cards.end());
  size_t const n_unique{set.size()};

  if (n_unique == 1) {
    // It's all jokers
    return HandType::FIVE_OF_A_KIND;
  }

  if (n_unique == 2) {
    // All jokers can be the other kind
    return HandType::FIVE_OF_A_KIND;
  }

  if (n_unique == 3) {
    // Can be four of a kind or full house
    for (auto const &sete : set) {
      int count{0};
      for (auto e : cards) {
        if (e == sete) {
          count++;
        }
      }
      if (count == 4 - n_jokers) {
        return HandType::FOUR_OF_A_KIND;
      }
    }
    // Otherwise full house
    return HandType::FULL_HOUSE;
  }

  if (n_unique == 4) {
    // It cannot be two pairs because the joker for one pair could
    // just be on the other pair instead
    return HandType::THREE_OF_A_KIND;
  }

  // at least a pair
  return HandType::ONE_PAIR;
}

struct Hand {
  std::string cards;
  int bid;
  HandType type;

  bool operator<(Hand const &a) const {
    if (type != a.type) {
      return type < a.type;
    }
    for (int i = 0; i < n_cards; i++) {
      if (cards[i] == a.cards[i]) {
        continue;
      }
      auto const thisv{card_order.find(cards[i])};
      auto const thatv{card_order.find(a.cards[i])};
      return thisv > thatv;
    }
    // This should never happen..
    std::cout << "WUT" << std::endl;
    return true;
  }
};

struct HandTwo {
  std::string cards;
  int bid;
  HandType type;

  bool operator<(HandTwo const &a) const {
    if (type != a.type) {
      return type < a.type;
    }
    for (int i = 0; i < n_cards; i++) {
      if (cards[i] == a.cards[i]) {
        continue;
      }
      auto const thisv{card_order_two.find(cards[i])};
      auto const thatv{card_order_two.find(a.cards[i])};
      return thisv > thatv;
    }
    // This should never happen..
    std::cout << "WUT" << std::endl;
    return true;
  }
};

int main(int argc, char const *argv[]) {
  std::ifstream f("../../day7/input.txt");

  std::vector<Hand> hands;
  for (std::string line; std::getline(f, line);) {
    std::istringstream stream(line);
    std::string cards;
    int bid;
    stream >> cards >> bid;

    HandType const type{determine_type(cards)};
    hands.push_back({cards, bid, type});
  }

  // Sort according to operator defined in Hands struct
  std::sort(hands.begin(), hands.end());

  int total_winnings{0};
  int rank{1};
  for (auto const &hand : hands) {
    total_winnings += rank * hand.bid;
    rank++;
  }

  std::cout << total_winnings << std::endl;

  // Part two
  std::vector<HandTwo> part_two_hands;
  for (auto const &hand : hands) {
    auto type = determine_type_part2(hand.cards);
    part_two_hands.push_back({hand.cards, hand.bid, type});
  }
  std::sort(part_two_hands.begin(), part_two_hands.end());
  int total_winnings_two{0};
  int rank_two{1};
  for (auto const &hand : part_two_hands) {
    total_winnings_two += rank_two * hand.bid;
    rank_two++;
  }
  std::cout << total_winnings_two << std::endl;
}
