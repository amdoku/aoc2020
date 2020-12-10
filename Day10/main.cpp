#include <deque>
#include <string_view>
#include <fstream>
#include <iterator>
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <numeric>

std::ostream &log() {
	return std::cout << "[ LOG ] | ";
}

template<typename T>
std::ostream &log(T && msg) {
	return log() << msg;
}

template<typename T>
auto readFromFile(std::string_view filename) {
	std::ifstream istream{filename.data()};
	if (!istream) {
		log("istream is bad.\n\t") << std::filesystem::current_path() << "\n";
	}

	std::istream_iterator<T> start{istream};
	std::istream_iterator<T> end{};
	std::deque<T> data{start, end};
	istream.close();
	return data;
}

int main(int argc, char** argv) {
	if (argc != 2) {
		std::cout << "Expected path to input file." << std::endl;
		return -2;
	}

	auto input{readFromFile<int>(argv[1])};
	input.push_back((*std::max_element(input.begin(), input.end())) + 3);
	input.push_front(0);

	log("input size = ") << input.size() << "\n";

	std::sort(input.begin(), input.end());
	std::vector<int> deltaJolts(input.size());

	std::adjacent_difference(input.begin(), input.end(), deltaJolts.begin());

	auto threeCount = std::count(deltaJolts.begin(), deltaJolts.end(), 3);
	auto oneCount = std::count(deltaJolts.begin(), deltaJolts.end(), 1);

	log("Part 1: ") << threeCount * oneCount << '\n';

	return 0;
}
