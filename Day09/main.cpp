#include <vector>
#include <string_view>
#include <fstream>
#include <iterator>
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <cassert>

std::ostream &log() {
	return std::cout << "[ LOG ] | ";
}

template<typename T>
std::ostream &log(T && msg) {
	return log() << msg;
}

template<int lookback>
struct xmas_encoding final {
	using Iterator = std::vector<uint64_t>::iterator;
	auto operator()(Iterator begin, Iterator end) {
		auto sequenceStart{begin + lookback};
		for(; sequenceStart < end && findSum(sequenceStart - lookback, sequenceStart, *sequenceStart); sequenceStart++) {}

		return sequenceStart;
	}
private:
	static bool findSum(Iterator unsortedBegin, Iterator unsortedEnd, Iterator::value_type sum) {
		std::vector<uint64_t> sorted{unsortedBegin, unsortedEnd};
		assert(sorted.size() == lookback);
		std::sort(sorted.begin(), sorted.end());
		auto begin{sorted.begin()};
		auto end{sorted.begin() + (lookback - 1)};
		for(; begin < end; begin++) {
			uint64_t const x = *begin;
			// we look for a y, such that: x + y = sum
			uint64_t const y = sum - x;
			// all y' (y != y') such that y < y' can be ignored.
			while(*end > y) {
				end--;
			}

			if (*begin + *end == sum && begin != end) return true;
		}
		return false;
	}
};

template<typename T>
auto readFromFile(std::string_view filename) {
	std::ifstream istream{filename.data()};
	if (!istream) {
		log("istream is bad.\n\t") << std::filesystem::current_path() << "\n";
	}

	std::istream_iterator<T> start{istream};
	std::istream_iterator<T> end{};
	std::vector<T> data{start, end};
	istream.close();
	return data;
}

int main(int argc, char** argv) {
	if (argc != 2) {
		std::cout << "Expected path to input file." << std::endl;
		return -2;
	}

	auto input{readFromFile<uint64_t>(argv[1])};

	log("input size = ") << input.size() << "\n";

	xmas_encoding<25> encoding{};

	auto dodgy = encoding(input.begin(), input.end());

	log("Result 1: ") << *dodgy << '\n';

	return 0;
}
