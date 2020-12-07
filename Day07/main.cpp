#include <vector>
#include <string_view>
#include <fstream>
#include <iterator>
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <bitset>
#include <cassert>
#include <set>
#include <map>

struct BagContains final {
	std::string name;
	int amount;

	friend bool operator<(BagContains a, BagContains const &b) {
		return std::tie(a.name, a.amount)
		        < std::tie(b.name, b.amount);
	}
};

struct Bag final {
	std::string name;
	std::set<BagContains> contains;
};

std::ostream &log() {
	return std::cout << "[ LOG ] | ";
}

template<typename T>
std::ostream &log(T && msg) {
	return log() << msg;
}

std::ostream& operator<<(std::ostream &out, BagContains const & e) {
	return out << '[' << e.amount << '|' << e.name << ']';
}

std::ostream& operator<<(std::ostream &out, std::set<BagContains> const & set) {
	if (set.empty()) {
		out << "<empty set>";
	} else {
		auto iter{set.cbegin()};
		out << *iter;
		iter++;

		while (iter != set.cend()) {
			out << *iter;
			iter++;
		}
	}
	return out;
}

std::istream& operator>>(std::istream &in, Bag& out) {
	std::string word{};
	Bag bag{};

	if (!(in >> bag.name)) {
		return in;
	}

	in >> word;
	bag.name += word;

	in >> word; // skips bags
	assert(word == "bags");
	in >> word; // skips contain
	assert(word == "contain");

	int amount{};
	do {
		if (in.peek() == '\n' || in.peek() == -1) {
			in.ignore(1);
			break;
		}
		if (!(in >> amount)) {
			in.clear();
			in.ignore(std::numeric_limits<int>::max(), '\n');
			break;
		}
		std::string colour;
		in >> word;
		in >> colour;
		bag.contains.insert({word + colour, amount});
		in >> word; // skips bag[s]
		assert(word == "bag," || word == "bags," || word == "bag." || word == "bags.");
	} while(true);

	out = bag;
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

	auto const input{readFromFile<Bag>(argv[1])};

	log("input size = ") << input.size() << "\n";

	// build backwards relations.
	std::map<std::string, std::set<std::string>> relations{};
	for (Bag const & entry : input) {
		relations[entry.name]; // force a map entry for bags not containing any more bags
		std::for_each(entry.contains.begin(), entry.contains.end(), [&relations, &name = entry.name](BagContains const & bagContains) {
			auto & value = relations[bagContains.name];
			value.insert(name);
		});
	}

	constexpr auto needle{"shinygold"};
	std::set<std::string> searchSpace{needle};
	std::set<std::string> visited{};

	while(!searchSpace.empty()) {
		std::set<std::string> nextSearch{};
		for (auto && search : searchSpace) {
			auto const candidates = relations.at(search);
			if (std::find(visited.cbegin(), visited.cend(), search) == visited.cend()) {
				visited.insert(search);
				std::transform(candidates.begin(), candidates.end(), std::inserter(nextSearch, nextSearch.begin()), [](auto && name) {
					return name;
				});
			}
		}
		searchSpace = std::move(nextSearch);
	}

	// subtract 1 to account for visited containing shiny gold
	log("Result 1: ") << visited.size() - 1 << '\n';
	return 0;
}
