#include <vector>
#include <string_view>
#include <fstream>
#include <iterator>
#include <iostream>
#include <filesystem>
#include <algorithm>

#include "utilities.hpp"

std::ostream & log() {
	return std::cout << "[ LOG ] | ";
}

std::ostream & log(std::string_view msg) {
	return log() << msg;
}

std::vector<int> readFromFile(std::string_view filename) {
	std::ifstream istream{filename.data()};
	if (!istream) {
		istream.exceptions();
		std::filesystem::current_path();
		log("istream is bad.\n\t") << std::filesystem::current_path() << "\n";
	}
	std::istream_iterator<util::read_sep<int, '\n'>> start{istream};
	std::istream_iterator<util::read_sep<int, '\n'>> end;
	std::vector<int> data{start, end};
	istream.close();
	return data;
}

std::pair<int, int> searchForAdded(int const sum, std::vector<int>::const_iterator begin, std::vector<int>::const_iterator end) {
	for(; begin < end; begin++) {
		int const x = *begin;
		// we look for a y, such that: x + y = sum
		int const y = sum - x;
		// all y' (y != y') such that y < y' can be ignored.
		while(*end > y) { end--; }

		if (*begin + *end == sum) return {*begin, *end};
	}

	return {-1, -1};
}

int main(int argc, char** argv) {
	if (argc != 2) {
		std::cout << "Expected path to input file." << std::endl;
		return -2;
	}
	std::vector<int> input{readFromFile(argv[1])};
	log("input size = ") << input.size() << "\n";

	std::sort(std::begin(input), std::end(input));

	auto start{std::cbegin(input)};
	auto end {std::cbegin(input) + (input.size() - 1)};

	std::pair<int, int> result{searchForAdded(2020, start, end)};

	log("Result [") << result.first << " | " << result.second << "] = " << result.first * result.second << std::endl;
	return 0;
}
