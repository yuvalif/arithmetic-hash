#include <iostream>
#include <random>
#include <string>
#include <algorithm>

std::string generate_random_string(unsigned len) {
  static std::string chars(
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz");
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<unsigned> dist(0, chars.length() - 1);
  auto result = std::string(len, '\0');
  std::generate_n(begin(result), len, [&]() {return chars[dist(gen)];});
  return result;
}

int main(int argc, char** argv) {

  if (argc < 3) {
    std::cerr << "usage: " << argv[0] << " <# prefixes> <# names per prefix>" << std::endl;
    return 1;
  }
  const auto number_of_prefix = std::stoul(argv[1]);
  const auto number_of_names = std::stoul(argv[2]);
  constexpr unsigned min_prefix = 2;
  constexpr unsigned max_prefix = 10;
  constexpr double mean_name = 3; 
  constexpr double stdev_name = 1;
  std::vector<std::string> longest_prefixes;
  std::random_device rd;
  std::mt19937 gen(rd());

  for (auto i = 0UL; i < number_of_prefix; ++i ){
    longest_prefixes.push_back(generate_random_string(max_prefix));
  }

  std::uniform_int_distribution<unsigned> prefix_dist(0, number_of_prefix-1);
  std::uniform_int_distribution<unsigned> prefix_len_dist(min_prefix, max_prefix);
  std::lognormal_distribution<> name_dist(mean_name, stdev_name);

  for (auto j = 0UL; j < number_of_names*number_of_prefix; ++j) {
    const auto& longest_prefix = longest_prefixes[prefix_dist(gen)];
    const auto prefix_len = prefix_len_dist(gen);
    const auto prefix = longest_prefix.substr(0, prefix_len);
    const auto name_len = static_cast<unsigned>(name_dist(gen));
    const auto name = generate_random_string(name_len);
    std::cout << prefix << name << std::endl;
  }
  return 0;
}

