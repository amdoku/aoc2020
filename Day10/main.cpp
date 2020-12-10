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

	auto input{readFromFile<size_t>(argv[1])};
	input.push_back((*std::max_element(input.begin(), input.end())) + 3);
	input.push_front(0);

	log("input size = ") << input.size() << "\n";

	std::sort(input.begin(), input.end());
	std::vector<int> deltaJolts(input.size());

	std::adjacent_difference(input.begin(), input.end(), deltaJolts.begin());

	auto threeCount = std::count(deltaJolts.begin(), deltaJolts.end(), 3);
	auto oneCount = std::count(deltaJolts.begin(), deltaJolts.end(), 1);

	log("Part 1: ") << threeCount * oneCount << '\n';

	// Part 2
	// go from "goal" to "start",
	// filling in the number of paths which lead from this jolt amount to the "goal"
	std::vector<size_t> paths(input.back() + 1, 0);
	paths.at(input.back()) = 1; // only one path leads to the "goal" by definition

	// we've fixed the paths to the goal, so start loop with second to last element
	std::for_each(++input.crbegin(), input.crend(), [&paths](auto const jolt) {
		paths.at(jolt) = paths.at(jolt + 1) + paths.at(jolt + 2) + paths.at(jolt + 3);
	});

	log("Part 2: ") << paths.at(0) << '\n';

	return 0;
}
