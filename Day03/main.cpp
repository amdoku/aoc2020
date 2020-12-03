#include <vector>
#include <string_view>
#include <fstream>
#include <iterator>
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <complex>
#include <numeric>

std::ostream & log() {
	return std::cout << "[ LOG ] | ";
}

std::ostream & log(std::string_view msg) {
	return log() << msg;
}

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

int main(int argc, char** argv) {
	if (argc != 2) {
		std::cout << "Expected path to input file." << std::endl;
		return -2;
	}
	std::vector<std::string> const input{readFromFile(argv[1])};

	log("input size = ") << input.size() << "\n";
	size_t const lineLength{input.at(0).length()};

	std::array<std::complex<int>, 5> position{{
		{1, 1},
		{3, 1},
		{5, 1},
		{7, 1},
		{1, 2}
	}};
	// line is real
	// coloumn is imaginary
	std::array<int, position.max_size()> result{};
	std::transform(std::begin(position), std::end(position), result.begin(), [&input, lineLength](auto const & direction) -> int {
		int treesHit{};
		for(std::complex<int> start{}; static_cast<size_t>(start.imag()) < input.size(); start += direction) {
			if (input.at(start.imag())
					.at(start.real() % lineLength) == '#') {
				treesHit++;
			}
		}
		return treesHit;
	});

	for(int ii = 0; ii < 5; ii++) {
		log() << position[ii] << ": " << result[ii] << "\n";
	}

	log("Product = ") << std::transform_reduce(std::cbegin(result), std::cend(result), 1ll, std::multiplies<>{}, [](int a) { return a; });

	return 0;
}
