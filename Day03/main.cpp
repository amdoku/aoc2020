#include <vector>
#include <string_view>
#include <fstream>
#include <iterator>
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <numeric>
#include <cassert>

std::ostream & log() {
	return std::cout << "[ LOG ] | ";
}

std::ostream & log(std::string_view msg) {
	return log() << msg;
}

// Fwd declare
struct Trie;

using MovementFn = std::function<bool(Trie&, int)>;

struct Trie final {
	int coloumn{};
	int row{};
	int treesHit{};

	MovementFn move;
};

template<int coloumn, int rows>
struct traverse final {
	bool operator()(Trie& t, int row) {
		if (row == t.row) {
			t.coloumn += coloumn;
			t.row += rows;
			return true;
		}
		return false;
	}
};

std::vector<std::string> readFromFile(std::string_view filename) {
	std::ifstream istream{filename.data()};
	if (!istream) {
		log("istream is bad.\n\t") << std::filesystem::current_path() << "\n";
	}
	std::istream_iterator<std::string> start{istream};
	std::istream_iterator<std::string> end;
	std::vector<std::string> data{start, end};
	istream.close();
	return data;
}

void hitTree(Trie & pos, std::string const & line, int row) {
	if (pos.row == row && line.at(pos.coloumn % line.length()) == '#') {
		pos.treesHit++;
	}
}


int main(int argc, char** argv) {
	if (argc != 2) {
		std::cout << "Expected path to input file." << std::endl;
		return -2;
	}
	std::vector<std::string> input{readFromFile(argv[1])};

	log("input size = ") << input.size() << "\n";

	Trie t{0, 0, 0, traverse<3, 1>{}};

	std::array<Trie, 5> position{{
		{.move = traverse<1, 1>{}},
		{.move = traverse<3, 1>{}},
		{.move = traverse<5, 1>{}},
		{.move = traverse<7, 1>{}},
		{.move = traverse<1, 2>{}}
	}};
	std::for_each(std::cbegin(input), std::cend(input), [&position, row = 0](std::string const & line) mutable -> void {
		std::for_each(std::begin(position), std::end(position), [&line, row](Trie & pos) -> void {
			hitTree(pos, line, row);
			pos.move(pos, row);
		});
		row++;
	});

	std::for_each(std::begin(position), std::end(position), [](Trie const & t) {
		log("Trees hit = ") << t.treesHit << "\n";
	});

	log("Result = ") << std::accumulate(std::begin(position), std::end(position), 1, [](int64_t acc, Trie const & t) -> int64_t {
		assert(t.treesHit > 0);
		return acc * t.treesHit;
	}) << "\n";

	return 0;
}
