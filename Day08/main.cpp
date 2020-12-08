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

	InstructionType type{};
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
		case Instruction::InstructionType::NOOP:
			instruction.instructionDelta = delta;
			break;
	}

	out = instruction;
	return in;
}

using Memory = std::vector<Instruction>;

class CPU final {
	ssize_t accumulator;
	ssize_t instructionRegister;

	Memory memory;

public:

	explicit CPU(Memory & mem) : accumulator(0), instructionRegister(0),
		memory(mem) {}

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
		if (in.type != Instruction::InstructionType::NOOP) {
			instructionRegister += in.instructionDelta;
		} else {
			instructionRegister++;
		}

		return instructionRegister;
	}

	[[nodiscard]]
	bool terminated() const {
		return static_cast<size_t>(instructionRegister) >= memory.size();
	}

	void reset() {
		instructionRegister = 0;
		accumulator = 0;
	}

	friend bool run(CPU & cpu, std::pair<ssize_t, Instruction> const & instructionToMod);
};

auto constexpr recordInstruction = [](Memory const &memory, ssize_t instructionCounter) -> std::optional<std::pair<ssize_t, Instruction>> {
	using InstructionType = Instruction::InstructionType;
	auto instruction{memory.at(static_cast<size_t>(instructionCounter))};
	switch(instruction.type) {
		case InstructionType::JMP:
		case InstructionType::NOOP:
			return std::make_pair(instructionCounter, instruction);
		case InstructionType::ACC:
			break;
	}
	return {};
};

auto constexpr switchInstruction(Instruction & instruction) {
	if (instruction.type == Instruction::InstructionType::JMP) {
		instruction.type = Instruction::InstructionType::NOOP;
	} else if (instruction.type == Instruction::InstructionType::NOOP) {
		instruction.type = Instruction::InstructionType::JMP;
	}
}

bool run(CPU & cpu, std::pair<ssize_t, Instruction> const & instructionToMod) {
	std::set<ssize_t> seenInstructions{};
	auto instruction{0l};

	switchInstruction(cpu.memory.at(static_cast<std::size_t>(instructionToMod.first)));

	while(!cpu.terminated() && seenInstructions.find(instruction) == seenInstructions.end()) {
		seenInstructions.insert(instruction);
		instruction = cpu.execute();
	}

	switchInstruction(cpu.memory.at(static_cast<std::size_t>(instructionToMod.first)));

	return cpu.terminated();
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

	auto input{readFromFile<Instruction>(argv[1])};

	log("input size = ") << input.size() << "\n";

	CPU cpu{input};

	// execute until encountered already hit instruction register
	auto instruction{0l};
	std::set<ssize_t> seenInstructions{};
	std::vector<std::pair<ssize_t, Instruction>> modifyableInstructions{};

	while(!cpu.terminated() && seenInstructions.find(instruction) == seenInstructions.end()) {
		seenInstructions.insert(instruction);
		auto maybeRecordable = recordInstruction(input, instruction);
		if (maybeRecordable) {
			modifyableInstructions.push_back(std::move(maybeRecordable.value()));
		}
		instruction = cpu.execute();
	}

	log("Result 1: ") << cpu.acc() << '\n';

	// replay with recorded jump and noop instructions
	for(auto const & modifyableInstruction : modifyableInstructions) {
		cpu.reset();
		if (run(cpu, modifyableInstruction)) {
			log("Result 2: ") << cpu.acc() << '\n';
			break;
		}
	}

	return 0;
}
