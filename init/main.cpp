#include <cstdio>
#include "value.hpp"

int main(int, char**) {
	std::puts(test::value);
	std::puts(test::Constants::value);
	return 0;
}
