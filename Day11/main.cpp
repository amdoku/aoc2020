#include <string_view>
#include <fstream>
#include <iterator>
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <cassert>
#include <numeric>
#include <complex>

using Layout = std::vector<std::string>;
using Change = std::complex<size_t>;
using Direction = std::complex<size_t>;
using Coordinates = std::complex<size_t>;
using SeatTestFunction = std::function<bool(Layout const&, size_t const, size_t const)>;

constexpr auto FLOOR = '.';
constexpr auto SEAT = 'L';
constexpr auto PERSON = '#';

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

template<char SEAT, int AMOUNT, bool COUNT_FLOOR>
constexpr auto countSurrounding = [](Layout const & layout, size_t const row, size_t const coloumn) -> bool {
	std::array<std::pair<size_t, size_t>, 8> const surroundingSeats{{
			{row-1, coloumn-1}, {row-1, coloumn}, {row-1, coloumn+1},
			{row  , coloumn-1},   /* current seat */    {row  , coloumn+1},
			{row+1, coloumn-1}, {row+1, coloumn}, {row+1, coloumn+1}
	}};

	auto const counted = std::count_if(surroundingSeats.begin(), surroundingSeats.end(), [&layout](auto const & pair) {
		bool count{COUNT_FLOOR};
		if (	   0 <= pair.first && pair.first < layout.size()
				&& 0 <= pair.second && pair.second < layout.at(pair.first).length()) {
			count = layout.at(pair.first).at(pair.second) == SEAT;
			if(COUNT_FLOOR && !count) {
				count = layout.at(pair.first).at(pair.second) == FLOOR;
			}
		}
		return count;
	});

	return counted >= AMOUNT;
};

template<char SEAT, int AMOUNT, bool COUNT_FLOOR>
constexpr auto countVisible = [](Layout const & layout, size_t const row, size_t const coloumn) {
	std::array<Direction, 8> constexpr directionVectors{{
			{-1ul, -1ul}, {-1ul, 0}, {-1ul, +1},
			{ 0, -1ul}, /*seat*/ { 0, +1},
			{+1, -1ul}, {+1, 0}, {+1, +1}
	}};
	Coordinates origin{row, coloumn};

	auto seen{0ul};

	for(auto const & entry : directionVectors) {
		Coordinates current{origin + entry};

		while (   current.real() < layout.size()
				&& current.imag() < layout.at(current.real()).length()) {
			if (layout.at(current.real()).at(current.imag()) != FLOOR) {
				if (layout.at(current.real()).at(current.imag()) == SEAT) {
					seen++;
				}
				break;
			}
			current+=entry;
		}
		if (COUNT_FLOOR && !(current.real() < layout.size()
				&& current.imag() < layout.at(current.real()).length())) {
			seen++;
		}
	}

	return seen >= AMOUNT;
};



bool step(Layout & layout, SeatTestFunction const & willChangeToPerson, SeatTestFunction const & willChangeToSeat) {
	std::vector<Change> markedChanges;

	auto rows{0ul};
	auto coloumn{0ul};

	for(auto const & row : layout) {
		coloumn = 0;
		for(auto const & c : row) {
			bool const seatAndWillChange{c == SEAT && willChangeToPerson(layout, rows, coloumn)};
			bool const personAndWillChange{c == PERSON && willChangeToSeat(layout, rows, coloumn)};
			if (seatAndWillChange || personAndWillChange) {
				markedChanges.emplace_back(rows, coloumn);
			}
			coloumn++;
		}
		rows++;
	}

	std::for_each(markedChanges.cbegin(), markedChanges.cend(), [&layout](auto const & pair) {
		auto & seat = layout[pair.real()][pair.imag()];
		if (seat == SEAT) {
			seat = PERSON;
		} else if (seat == PERSON) {
			seat = SEAT;
		} else {
			assert(false);
		}
	});

	return !markedChanges.empty();
}

std::ostream& operator<<(std::ostream & out, Layout const & layout) {
	std::for_each(layout.begin(), layout.end(), [&out](auto && entry) {
		out << entry << '\n';
	});

	return out << '\n';
}

int main(int argc, char** argv) {
	if (argc != 2) {
		std::cout << "Expected path to input file." << std::endl;
		return -2;
	}

	auto input{readFromFile<std::string>(argv[1])};

	log("input size=") << input.size() << '\n';
	{
		bool progress{true};
		int iteration{0u};
		Layout part1{input};

		constexpr auto willChangeToPerson = countSurrounding<SEAT, 8, true>;
		constexpr auto willChangeToSeat = countSurrounding<PERSON, 4, false>;
		for (; progress && iteration < 1000; ++iteration) {
			progress = step(part1, willChangeToPerson, willChangeToSeat);
		}

		log("Iteration=") << iteration << '\n';

		log("Part 1: ")
				<< std::accumulate(part1.cbegin(), part1.cend(), 0ul, [](size_t const &acc, std::string const &line) {
					return acc + static_cast<unsigned long>(std::count(line.begin(), line.end(), PERSON));
				}) << '\n';
	}

	{
		Layout part2{input};
		bool progress{true};
		auto iteration{0ul};
		constexpr auto willChangeToPerson = countVisible<SEAT, 8, true>;
		constexpr auto willChangeToSeat = countVisible<PERSON, 5, false>;
		for (; progress && iteration < 1000; ++iteration) {
			progress = step(part2, willChangeToPerson, willChangeToSeat);
		}

		log("Part 2: ")
				<< std::accumulate(part2.cbegin(), part2.cend(), 0ul, [](size_t const &acc, std::string const &line) {
					return acc + static_cast<std::size_t>(std::count(line.begin(), line.end(), PERSON));
				}) << '\n';
	}

	return 0;
}
