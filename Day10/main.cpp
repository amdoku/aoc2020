#include <deque>
#include <string_view>
#include <fstream>
#include <iterator>
#include <iostream>
#include <filesystem>
#include <algorithm>

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

	int threeCount = 0;
	int oneCount = 0;

	std::for_each(input.begin(), input.end(), [last = 0, &threeCount, &oneCount](int jolts) mutable {
		auto retVal = jolts - last;
		last = jolts;
		if (retVal == 3) threeCount++;
		if (retVal == 1) oneCount++;
		return retVal;
	});

	log("Part 1: ") << threeCount * oneCount << '\n';

	return 0;
}
