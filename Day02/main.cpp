#include <vector>
#include <string_view>
#include <fstream>
#include <iterator>
#include <iostream>
#include <filesystem>
#include <functional>
#include <cassert>

std::ostream & log() {
	return std::cout << "[ LOG ] | ";
}

std::ostream & log(std::string_view msg) {
	return log() << msg;
}

class Password final {
public:
	std::string password{};
	std::function<bool(std::string const &)> testFn{};
private:
	friend std::istream& operator>>(std::istream& in, Password &t);
};

std::istream& operator>>(std::istream& in, Password & t) {
	int lowerBound = -1;
	if (!(in >> lowerBound)) {
		return in;
	}
	assert(in.peek() == '-');
	in.ignore();

	int upperBound = -1;
	in >> upperBound;
	assert(in.peek() == ' ');
	in.ignore();

	char letter = 0;
	in >> letter;
	assert(in.peek() == ':');
	in.ignore();

	assert(in.peek() == ' ');
	in.ignore();

	std::string password{};
	in >> password;
	assert(in.peek() == '\n');
	in.ignore();

	t.password = std::move(password);
	t.testFn = [lowerBound, upperBound, letter](std::string const & view) -> bool {
		return (view[lowerBound - 1] == letter) xor (view[upperBound - 1] == letter);
	};

	return in;
}

std::vector<Password> readFromFile(std::string_view filename) {
	std::ifstream istream{filename.data()};
	if (!istream) {
		istream.exceptions();
		std::filesystem::current_path();
		log("istream is bad.\n\t") << std::filesystem::current_path() << "\n";
	}
	std::istream_iterator<Password> start{istream};
	std::istream_iterator<Password> end;
	std::vector<Password> data{start, end};
	istream.close();
	return data;
}

int main(int argc, char** argv) {
	if (argc != 2) {
		std::cout << "Expected path to input file." << std::endl;
		return -2;
	}
	std::vector<Password> input{readFromFile(argv[1])};
	log("input size = ") << input.size() << "\n";

	auto result = std::count_if(input.cbegin(), input.cend(), [](Password const & pw) {
		return pw.testFn(pw.password);
	});

	log("Found ") << result << " valid passwords.\n";
	return 0;
}
