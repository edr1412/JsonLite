#include "myjson.hpp"
using namespace js01;
int main() {
    auto res = parse_json("test.json");
    res->write_to_file("out.json");
	return 0;
}
