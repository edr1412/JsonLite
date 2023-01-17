#include <iostream>
#include "myjson.hpp"
using namespace js01;
int main() {
    std::string str("{\"string\": \"abc\", \"integer\": 123, \"double\": 3.14, \"array\": [\"item1\", \"item2\"], \"object\": {\"name1\": \"value1\", \"name2\": \"value2\"}}");
    std::cout << str << std::endl;
    std::stringstream ins(str);
    auto res = parse_json(ins);
    //res->get_object()["array"]->get_array().at(0)->get_string() = "item3";
    //auto res = parse_json(std::cin);
    res->write(std::cout, 0);
	return 0;
}
