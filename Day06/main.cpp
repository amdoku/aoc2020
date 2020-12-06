#include <vector>
#include <string_view>
#include <fstream>
#include <iterator>
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <bitset>
#include <cassert>
#include <numeric>

struct Answers final {
	std::bitset<26> yesAnswers;
};

std::ostream &log() {
	return std::cout << "[ LOG ] | ";
}

template<typename T>
std::ostream &log(T && msg) {
	return log() << msg;
}

constexpr auto toMask = [](char const c) -> size_t {
	return static_cast<size_t>(c - 'a');
};

std::istream& operator>>(std::istream &in, Answers & out) {
	std::string line{};
	std::getline(in, line);

	Answers group{};
	group.yesAnswers.flip();
	do {
		Answers person{};
		std::for_each(line.cbegin(), line.cend(), [&person](char const c) {
			if ('a' <= c && c <= 'z') {
				assert(toMask(c) < 26);
				person.yesAnswers.set(toMask(c));
			} else {
				log("Encountered character '") << c << "'\n";
			}
		});

		group.yesAnswers &= person.yesAnswers;

		std::getline(in, line);
	} while (line.length() > 0);

	out = group;

	return in;
}

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

	auto const input{readFromFile<Answers>(argv[1])};

	log("input size = ") << input.size() << "\n";

	auto const part1 = std::accumulate(input.begin(), input.end(), 0ul, [](auto & acc, auto const & entry) {
		return acc + entry.yesAnswers.count();
	});

	log("Part 2: ") << part1 << '\n';

	return 0;
}
