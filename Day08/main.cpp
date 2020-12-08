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

std::ostream &log() {
	return std::cout << "[ LOG ] | ";
}

template<typename T>
std::ostream &log(T && msg) {
	return log() << msg;
}

struct Instruction final {
	enum class InstructionType {
		NOOP,
		ACC,
		JMP
	};
	ssize_t instructionDelta{1};
	ssize_t accumulatorDelta{};

	InstructionType type{}; // here for debug reasons
};

Instruction::InstructionType from_string(std::string_view view) {
	using InstructionType = Instruction::InstructionType;

	if (view == "nop") return InstructionType::NOOP;
	if (view == "acc") return InstructionType::ACC;
	if (view == "jmp") return InstructionType::JMP;

	throw std::out_of_range(view.data());
}

std::istream& operator>>(std::istream &in, Instruction& out) {
	std::string word{};

	if (!(in >> word)) {
		return in;
	}

	Instruction instruction{};
	instruction.type = from_string(word);

	assert(in.peek() == ' ');
	in.ignore(1, ' ');

	int delta;
	in >> delta;

	switch(instruction.type) {
		case Instruction::InstructionType::ACC:
			instruction.accumulatorDelta = delta;
			break;
		case Instruction::InstructionType::JMP:
			instruction.instructionDelta = delta;
			break;
		case Instruction::InstructionType::NOOP:
			break;
	}

	out = instruction;
	return in;
}

class CPU final {
	ssize_t accumulator;
	ssize_t instructionRegister;

	std::vector<Instruction> memory;

public:

	explicit CPU(std::vector<Instruction> && mem) : accumulator(0), instructionRegister(0),
		memory(std::move(mem)) {}

	CPU(CPU const &) = delete;
	CPU& operator=(CPU const &) = delete;
	CPU(CPU&&) noexcept = delete;
	CPU& operator=(CPU&&) noexcept = delete;

	[[nodiscard]]
	auto acc() const {
		return accumulator;
	}

	auto execute() {
		assert(instructionRegister >= 0);
		Instruction const & in{memory.at(static_cast<size_t>(instructionRegister))};
		accumulator += in.accumulatorDelta;
		instructionRegister += in.instructionDelta;

		return instructionRegister;
	}
};

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

	auto input{readFromFile<Instruction>(argv[1])};

	log("input size = ") << input.size() << "\n";

	CPU cpu{std::move(input)};

	// execute until encountered already hit instruction register
	auto instruction{0l};
	std::set<ssize_t> seenInstructions{};
	while(seenInstructions.find(instruction) == seenInstructions.end()) {
		seenInstructions.insert(instruction);
		instruction = cpu.execute();
	}

	log("Result 1: ") << cpu.acc() << '\n';

	return 0;
}
