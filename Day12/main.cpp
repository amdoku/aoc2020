#include <string_view>
#include <fstream>
#include <iterator>
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <cassert>
#include <complex>

using Coordinates = std::complex<ssize_t>;
using Direction = std::complex<ssize_t>;

enum class Facing : int {
	NORTH = 0,
	EAST = 90,
	SOUTH = 180,
	WEST = 270,
};

std::ostream& operator<<(std::ostream & out, Facing const & facing) {
	switch(facing) {
		case Facing::NORTH: return out << "NORTH";
		case Facing::EAST: return out << "EAST";
		case Facing::SOUTH: return out << "SOUTH";
		case Facing::WEST: return out << "WEST";
	}
	throw std::runtime_error("wut.");
}

Direction degreeToVector(Facing f) {
	static constexpr Direction EAST{+1, 0};
	static constexpr Direction NORTH{0,+1};
	static constexpr Direction WEST{-1, 0};
	static constexpr Direction SOUTH{0, -1};

	switch(f) {
		case Facing::NORTH: return NORTH;
		case Facing::EAST: return EAST;
		case Facing::WEST: return WEST;
		case Facing::SOUTH: return SOUTH;
	}
	throw std::runtime_error("Bad maths.");
}

enum class Turn : int {
	LEFT = -1, RIGHT = 1
};

struct Ship final {
	Coordinates coord {0,0};
	Facing facing {Facing::EAST};

	auto turn(Turn const turn, ssize_t amount) {
		auto degrees = (std::underlying_type_t<Facing>(facing) + (std::underlying_type_t<Turn>(turn) * amount));
		if (degrees < 0) degrees+=360;
		degrees %= 360;
		assert(0 <= degrees && degrees <= 360);

		switch(degrees) {
			case 0:
				facing = Facing::NORTH;
				break;
			case 90:
				facing = Facing::EAST;
				break;
			case 180:
				facing = Facing::SOUTH;
				break;
			case 270:
				facing = Facing::WEST;
				break;
			default:
				assert(false);
		}

	}
};

std::ostream& operator<<(std::ostream& out, Ship const & ship) {
	return out << "Ship[" << ship.coord.real() << ", " << ship.coord.imag() << " | " << ship.facing << ']';
}

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

int main(int argc, char** argv) {
	if (argc != 2) {
		std::cout << "Expected path to input file." << std::endl;
		return -2;
	}

	auto const input{readFromFile<std::string>(argv[1])};

	Ship ship{};
	for(auto const & line : input) {
		log(ship) << " -> " << line << '\n';
		auto value{std::stol(line.data() + 1)};
		switch(line.at(0)) {
			case 'E':
				ship.coord += Coordinates{value, 0};
				break;
			case 'W':
				ship.coord += Coordinates{-value, 0};
				break;
			case 'N':
				ship.coord += Coordinates{0, value};
				break;
			case 'S':
				ship.coord += Coordinates{0, -value};
				break;
			case 'L':
				ship.turn(Turn::LEFT, value);
				break;
			case 'R':
				ship.turn(Turn::RIGHT, value);
				break;
			case 'F':
				ship.coord += degreeToVector(ship.facing) * value;
				break;
			default:
				assert(false);
		}
	}

	log("input size=") << input.size() << '\n';

	log("Part 1: ") << manhattanDistance(ship.coord) << '\n';

	return 0;
}
