#include <vector>
#include <string_view>
#include <fstream>
#include <iterator>
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <bitset>
#include <map>
#include <charconv>

using FieldMask = std::bitset<7>;
using Fields = std::map<std::string, std::string>;

struct Passport final {
	FieldMask fieldMask{};
	Fields fields{};
};

FieldMask constexpr byr{1 << 0};
FieldMask constexpr iyr{1 << 1};
FieldMask constexpr eyr{1 << 2};
FieldMask constexpr hgt{1 << 3};
FieldMask constexpr hcl{1 << 4};
FieldMask constexpr ecl{1 << 5};
FieldMask constexpr pid{1 << 6};

std::array<std::pair<FieldMask, std::string_view>, 7> constexpr featureVectorsArr{
	std::make_pair(byr, "byr:"),
	std::make_pair(iyr, "iyr:"),
	std::make_pair(eyr, "eyr:"),
	std::make_pair(hgt, "hgt:"),
	std::make_pair(hcl, "hcl:"),
	std::make_pair(ecl, "ecl:"),
	std::make_pair(pid, "pid:")
};

std::array<std::string_view, 7> constexpr validEyeColours{
	"amb", "blu", "brn", "gry", "grn", "hzl", "oth"
};

std::ostream &log() {
	return std::cout << "[ LOG ] | ";
}

template<typename T>
std::ostream &log(T && msg) {
	return log() << msg;
}

template<int lowerBound, int upperBound>
struct between final {
	bool operator() (std::string_view in) {
		int value{};
		std::from_chars(in.begin(), in.end(), value);
		return (lowerBound <= value && value <= upperBound);
	}
};

struct checkEyecolour final {
	bool operator() (std::string_view in) {
		return std::find(validEyeColours.begin(), validEyeColours.end(), in) != validEyeColours.end();
	}
};

template<int digitCount>
struct number final {
	bool operator() (std::string_view in) {
		return in.length() == digitCount && std::all_of(in.begin(), in.end(), isdigit);
	}
};

struct checkHairColour final {
	bool operator() (std::string_view in) {
		return in.front() == '#'
			&& std::all_of(in.begin() + 1, in.end(), [](auto c) {
				return isdigit(c) || islower(c);
			});
	}
};

struct checkHeight final {
	bool operator() (std::string_view in) {
		int value{};
		auto res = std::from_chars(in.begin(), in.end(), value);
		if (std::string_view unit{res.ptr}; unit == "cm") {
			return 150 <= value && value <= 193;
		} else if (unit == "in") {
			return 59 <= value && value <= 76;
		} else {
			return false;
		}
	}
};

std::istream& operator>>(std::istream &in, Passport & out) {
	std::string line{};
	std::getline(in, line);
	Passport v{};
	do {
		std::for_each(featureVectorsArr.cbegin(), featureVectorsArr.cend(), [&v, &line](std::pair<FieldMask, std::string_view> const & feature) {
			if (auto linePtr = std::search(line.cbegin(), line.cend(), feature.second.cbegin(), feature.second.cend()); linePtr != line.cend()) {
				v.fieldMask |= feature.first;

				auto const split{std::find(linePtr, line.cend(), ':')};
				auto const end{std::find(linePtr, line.cend(), ' ')};
				v.fields.emplace(std::make_pair(std::string{linePtr, split}, std::string{split + 1, end}));
			}
		});
		std::getline(in, line);
	} while (line.length() > 0);

	out = v;

	return in;
}

std::vector<Passport> readFromFile(std::string_view filename) {
	std::ifstream istream{filename.data()};
	if (!istream) {
		log("istream is bad.\n\t") << std::filesystem::current_path() << "\n";
	}

	std::istream_iterator<Passport> start{istream}, end;
	std::vector<Passport> data{start, end};
	istream.close();
	return data;
}

int main(int argc, char** argv) {
	if (argc != 2) {
		std::cout << "Expected path to input file." << std::endl;
		return -2;
	}

	std::vector<Passport> const input{readFromFile(argv[1])};

	log("input size = ") << input.size() << "\n";

	auto const result = std::count_if(input.cbegin() ,input.cend(), [](Passport const & v) {
		return v.fieldMask == (byr | iyr | eyr | hgt | hcl | ecl | pid);
	});

	auto const result2 = std::count_if(input.cbegin(), input.end(), [](Passport const & v) {
		if (v.fieldMask == (byr | iyr | eyr | hgt | hcl | ecl | pid)) {
			auto const & fields{v.fields};
			return number<4>{}(fields.at("byr")) && between<1920, 2002>{}(fields.at("byr"))
				&& number<4>{}(fields.at("iyr")) && between<2010, 2020>{}(fields.at("iyr"))
				&& number<4>{}(fields.at("eyr")) && between<2020, 2030>{}(fields.at("eyr"))
				&& number<9>{}(fields.at("pid"))
				&& checkEyecolour{}(fields.at("ecl"))
				&& checkHairColour{}(fields.at("hcl"))
				&& checkHeight{}(fields.at("hgt"));
		}
		return false;
	});

	log("Part 1: ") << result << '\n';
	log("Part 2: ") << result2 << '\n';

	return 0;
}
