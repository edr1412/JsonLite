#include <iostream>
#include "myjson.hpp"
using namespace js01;
int main() {
    // std::string str("{\"string\": \"abc\", \"integer\": 123, \"double\": 3.14, \"array\": [\"item1\", \"item2\"], \"object\": {\"name1\": \"value1\", \"name2\": \"value2\"}}");
    // std::cout << str << std::endl;
    // std::stringstream ins(str);
    // auto res = parse_json(ins);
    auto res = parse_json("test.json");
    res->write_to_file("out.json");
	return 0;
}
