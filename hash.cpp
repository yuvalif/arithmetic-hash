/*
The MIT License (MIT)

Copyright (c) 2014 Mark Thomas Nelson

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE. 

 This code was written to illustrate the article:
 Data Compression With Arithmetic Coding
 by Mark Nelson
 published at: http://marknelson.us/2014/10/19/data-compression-with-arithmetic-coding

*/
#include <stdexcept>
#include <format>
#include <string>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <type_traits>

template<typename T>
struct Model {
  static_assert(std::is_same<float, T>::value, "type must be similar to 'float'");
  const T epsilon;

  Model(T _epsilon = 0.0) : epsilon(_epsilon)  {
    // initially the model assume uniform distribution of chars
    for (unsigned char c = 0; c < size; ++c) {
      ++total;
      cumulative_frequency[c] = c;
    }
  }

  void update(const std::string& s) {
    for (const auto c : s) {
      update(c);
    }
  }

  void update(unsigned char c){
    ++total;
    for (unsigned i = char_to_index(c); i < size; ++i)
      ++cumulative_frequency[i];
  }
  
  void dump() const {
    for (unsigned char i = 0; i < size; ++i) {
      const auto c = index_to_char(i);
      const auto p = get_probability(c);
      std::cout << "prob(" << c << ") = (" << 
        p.first << ", " << p.second << ")" << std::endl;
    }
  }

  std::pair<T,T> get_probability(unsigned char c) const {
    const auto i = char_to_index(c);
    const auto count = cumulative_frequency[i];
    const auto next_count = cumulative_frequency[i+1];
    const auto p1 = static_cast<T>(count)/static_cast<T>(total);
    const auto p2 = static_cast<T>(next_count)/static_cast<T>(total);
    return std::make_pair(p1, p2); 
  }
  
private:
  static constexpr unsigned size = ('Z'-'A') + ('z'-'a') + 1;
  static constexpr unsigned first_range_size = 'Z' - 'A';
  unsigned cumulative_frequency[size];
  unsigned total = 0;

  static unsigned char char_to_index(unsigned char c) {
    if (c >= 'A' && c <= 'Z')
      return (c - 'A');
    if (c >= 'a' && c <= 'z')
       return (c - 'a' + first_range_size);
    throw std::range_error(std::format("'char_to_index: {}' is out of range", c));
  }

  static unsigned char index_to_char(unsigned char i) {
    if (i <= first_range_size)
      return (i + 'A');
    if (i < size)
       return (i - first_range_size + 'a');
    throw std::range_error(std::format("index_to_char: '{}' is out of range", i));
  }
};

template <typename T>
T compress(const std::string& s, const Model<T>& model) {
  T high = 1.0;
  T low = 0.0;
  for (const auto c : s) {
    const auto p = model.get_probability(c);
    const T range = high - low;
    high = low + range * p.second;
    low = low + range * p.first; 
    if (high - low <= model.epsilon) break;
  }
  return low + (high-low)/2.0;
}

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cerr << "usage: " << argv[0] << " <input file> " << std::endl;
    return 1;
  }
  std::string line;
  using hash_type = float;
  std::map<hash_type, std::vector<std::string>> sorted_names;
  Model<hash_type> model(0.00001);
  /*{
    // seed the model with the real distribution
    std::ifstream infile(argv[1]);
    while (std::getline(infile, line)) {
      model.update(line);
    }
  }*/
  model.dump();
  {
    std::ifstream infile(argv[1]);
    while (std::getline(infile, line)) {
      // update the model on the fly
      //model.update(line);
      hash_type hash = compress<hash_type>(line, model);
      sorted_names[hash].push_back(line);
    }
  }
  std::cout << "total shards: " << sorted_names.size() << std::endl;
  size_t max_shard = 0ULL;
  for (const auto& index : sorted_names) {
    max_shard = std::max(index.second.size(), max_shard);
    //std::cout << index.second.size() << " names in: " << std::setprecision(19) << index.first << std::endl;
    //for (const auto& name : index.second) {
    //  std::cout << name << std::endl;
    //}
  }
  std::cout << "max shard: " << max_shard << std::endl;
  return 0;
}

