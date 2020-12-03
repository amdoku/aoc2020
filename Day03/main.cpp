#include <vector>
#include <string_view>
#include <fstream>
#include <iterator>
#include <iostream>
#include <filesystem>
#include <algorithm>

std::ostream & log() {
	return std::cout << "[ LOG ] | ";
}

std::ostream & log(std::string_view msg) {
	return log() << msg;
}

struct Trie final {
	int coloumn{};

	int treesHit{};
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

void hitTree(Trie & pos, std::string const & line) {
	if (line.at(pos.coloumn) == '#') {
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

	Trie position{};
	std::for_each(std::cbegin(input), std::cend(input), [&position](auto && line) {
		hitTree(position, line);
		position.coloumn += 3;
		if (position.coloumn >= static_cast<int>(line.length())) {
			position.coloumn -= line.length();
		}
	});

	log("Result = ") << position.treesHit << "\n";

	return 0;
}
