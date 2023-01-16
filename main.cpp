#include <iostream>
#include "myjson.hpp"

int main() {
    auto res = parse_json(std::cin);
    res->write(std::cout, 0);
	return 0;
}
