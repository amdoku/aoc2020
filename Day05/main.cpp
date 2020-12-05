#include <vector>
#include <string_view>
#include <fstream>
#include <iterator>
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <bitset>
#include <cassert>

using Row = std::bitset<7>;
using Coloumn = std::bitset<3>;

struct Seat final {
	Row row{};
	Coloumn seat{};
};

std::ostream &log() {
	return std::cout << "[ LOG ] | ";
}

template<typename T>
std::ostream &log(T && msg) {
	return log() << msg;
}
std::istream& operator>>(std::istream &in, Seat & out) {
	std::string line{};

	if(!(in >> line)) {
		return in;
	}
	Seat v{};

	auto seat = std::for_each_n(line.cbegin(), 7, [&v, shift = 6u](auto c) mutable {
		if (c == 'B') {
			v.row.set(shift);
		}
		shift--;
	});

	assert(std::distance(seat, line.cend()) > 0);

	std::for_each(seat, line.cend(), [&v, shift = 2u](auto c) mutable {
		if (c == 'R') {
			v.seat.set(shift);
		}
		shift--;
	});

	out = v;

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

	auto const input{readFromFile<Seat>(argv[1])};

	log("input size = ") << input.size() << "\n";

	// sort by seat id
	// get highest
	auto part1{0ul};
	std::for_each(input.begin(), input.end(), [&part1](Seat const & entry) {
		part1 = std::max(part1, entry.row.to_ulong() * 8 + entry.seat.to_ulong());
	});

	log("Part 1: ") << part1 << '\n';

	return 0;
}
