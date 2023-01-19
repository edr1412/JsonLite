# JsonParser

## Usage

```shell
make test
```

Read from file `test.json`, and write to `out.json`.

## Other Examples

```c++
std::string str("{\"string\": \"abc\", \"integer\": 123, \"double\": 3.14, \"array\": [\"item1\", \"item2\"], \"object\": {\"name1\": \"value1\", \"name2\": \"value2\"}}");
std::cout << str << std::endl;
std::stringstream ins(str);
auto res1 = parse_json(ins);
res1->get_object()["array"]->get_array().at(0)->get_string() = "item3";
res1->write(std::cout, 0);
```

```c++
JsonObject testJson;
testJson.get_object()["string"] = std::make_shared<JsonString>("abc");
testJson.get_insert_order().push_back("string");
testJson.get_object()["double"] = std::make_shared<JsonDouble>(3.14);
testJson.get_insert_order().push_back("double");
testJson.get_object()["boolean"] = std::make_shared<JsonBool>(true);
testJson.get_insert_order().push_back("boolean");
std::shared_ptr<JsonArray> array = std::make_shared<JsonArray>();
for (int i = 0; i < 3; i++) {
    std::shared_ptr<JsonObject> obj = std::make_shared<JsonObject>();
    obj->get_object()["index"] = std::make_shared<JsonDouble>(i);
    obj->get_insert_order().push_back("index");
    std::shared_ptr<JsonObject> obj2 = std::make_shared<JsonObject>();
    obj->get_object()["contents"] = obj2;
    obj->get_insert_order().push_back("contents");
    obj2->get_object()["empty"] = std::make_shared<JsonObject>();
    obj2->get_insert_order().push_back("empty");
    array->get_array().push_back(obj);
}
testJson.get_object()["data"] = array;
testJson.get_insert_order().push_back("data");
testJson.write(std::cout,0);
```
