#include <vector>
#include <string_view>
#include <fstream>
#include <iterator>
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <bitset>

struct FeatureVector final : public std::bitset<7>{};

FeatureVector constexpr byr{1 << 0};
FeatureVector constexpr iyr{1 << 1};
FeatureVector constexpr eyr{1 << 2};
FeatureVector constexpr hgt{1 << 3};
FeatureVector constexpr hcl{1 << 4};
FeatureVector constexpr ecl{1 << 5};
FeatureVector constexpr pid{1 << 6};
// FeatureVector constexpr cid{1 << 8};

std::array<std::pair<FeatureVector, std::string_view>, 7> constexpr featureVectorsArr{
	std::make_pair(byr, "byr:"),
	std::make_pair(iyr, "iyr:"),
	std::make_pair(eyr, "eyr:"),
	std::make_pair(hgt, "hgt:"),
	std::make_pair(hcl, "hcl:"),
	std::make_pair(ecl, "ecl:"),
	std::make_pair(pid, "pid:")
};

std::ostream &log() {
	return std::cout << "[ LOG ] | ";
}

template<typename T>
std::ostream &log(T && msg) {
	return log() << msg;
}

std::istream& operator>>(std::istream &in, FeatureVector & v) {
	std::string line{};
	std::getline(in, line);
	v.reset();
	do {
		std::for_each(featureVectorsArr.cbegin(), featureVectorsArr.cend(), [&v, &line](std::pair<FeatureVector, std::string_view> const & feature) {
			if (std::search(line.cbegin(), line.cend(), feature.second.cbegin(), feature.second.cend()) != line.cend()) {
				v |= feature.first;
			}
		});
		std::getline(in, line);
	} while (line.length() > 0);

	return in;
}

std::vector<FeatureVector> readFromFile(std::string_view filename) {
	std::ifstream istream{filename.data()};
	if (!istream) {
		log("istream is bad.\n\t") << std::filesystem::current_path() << "\n";
	}

	std::istream_iterator<FeatureVector> start{istream}, end;
	std::vector<FeatureVector> data{start, end};
	istream.close();
	return data;
}

int main(int argc, char** argv) {
	if (argc != 2) {
		std::cout << "Expected path to input file." << std::endl;
		return -2;
	}

	std::vector<FeatureVector> const input{readFromFile(argv[1])};

	log("input size = ") << input.size() << "\n";

	auto result = std::count_if(input.cbegin() ,input.cend(), [](FeatureVector const & v) {
		return v == (byr | iyr | eyr | hgt | hcl | ecl | pid);
	});

	log ("Part 1: ") << result << '\n';

	return 0;
}
