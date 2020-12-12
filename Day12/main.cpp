#include <string_view>
#include <fstream>
#include <iterator>
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <cassert>
#include <complex>

using Coordinates = std::complex<ssize_t>;

constexpr auto manhattanDistance = [](Coordinates const & c) {
	return std::abs(c.imag()) + std::abs(c.real());
};

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
	std::vector<T> data{start, end};
	istream.close();
	return data;
}

Coordinates part1(std::vector<std::string> const & input) {
	Coordinates ship{};
	Coordinates direction{1, 0};

	for(auto const & line : input) {
		auto value{std::stol(line.data() + 1)};
		switch(line.at(0)) {
			case 'E':
				ship += Coordinates{value, 0};
				break;
			case 'W':
				ship += Coordinates{-value, 0};
				break;
			case 'N':
				ship += Coordinates{0, value};
				break;
			case 'S':
				ship += Coordinates{0, -value};
				break;
			case 'L':
				value = 360 - value;
				[[fallthrough]];
			case 'R':
				for (auto doTurns{value/90}; doTurns > 0; doTurns--) {
					auto temp = direction.imag();
					direction.imag(-direction.real());
					direction.real(temp);
				}
				break;
			case 'F':
				ship += (direction * value);
				break;
			default:
				assert(false);
		}
	}
	return ship;
}

Coordinates part2(std::vector<std::string> const & input) {
	Coordinates waypoint{10, 1};
	Coordinates coords{};

	for(auto const & line : input) {
		auto value{std::stol(line.data() + 1)};
		switch(line.at(0)) {
			case 'E':
				waypoint += Coordinates{value, 0};
				break;
			case 'W':
				waypoint += Coordinates{-value, 0};
				break;
			case 'N':
				waypoint += Coordinates{0, value};
				break;
			case 'S':
				waypoint += Coordinates{0, -value};
				break;
			case 'L':
				value = 360 - value;
				[[fallthrough]];
			case 'R': {
				for (auto doTurns{value / 90}; doTurns > 0; doTurns--) {
					auto temp = waypoint.imag();
					waypoint.imag(-waypoint.real());
					waypoint.real(temp);
				}
				break;
			}
			case 'F': {
				coords += (waypoint * value);
				break;
			}
			default:
				assert(false);
		}
	}

	return coords;
}

int main(int argc, char** argv) {
	if (argc != 2) {
		std::cout << "Expected path to input file." << std::endl;
		return -2;
	}

	auto const input{readFromFile<std::string>(argv[1])};

	Coordinates ship1{part1(input)};

	Coordinates ship2{part2(input)};

	log("input size=") << input.size() << '\n';

	log("Part 1: ") << manhattanDistance(ship1) << '\n';
	log("part 2: ") << manhattanDistance(ship2) << '\n';

	return 0;
}
