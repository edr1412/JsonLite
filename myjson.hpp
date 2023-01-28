#ifndef MY_JSON
#define MY_JSON

#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <exception>
#include <sstream>
#include <iomanip>

#define double long double

namespace js01 {

enum class JsonType : uint8_t {
    NIL,
    STRING,
    NUMBER,
    BOOL,
    ARRAY,
    OBJECT
};

// The parent interface provides default implementations of all access methods that only throw exceptions.
// The interface itself can be used to represent the null type, because nothing can be obtained from this type.
struct JsonNode {
    inline virtual JsonType get_type() {
        return JsonType::NIL;
    }
    inline virtual std::string& get_string() {
        throw(std::runtime_error("not a string"));
    }
    inline virtual double& get_double() {
        throw(std::runtime_error("not a double"));
    }
    inline virtual bool& get_bool() {
        throw(std::runtime_error("not a bool"));
    }
    inline virtual std::vector<std::shared_ptr<JsonNode>>& get_array() {
        throw(std::runtime_error("not an array"));
    }
    inline virtual std::vector<std::string>& get_insert_order() {
        throw(std::runtime_error("not an object"));
    }
    inline virtual std::unordered_map<std::string, std::shared_ptr<JsonNode>>& get_object() {
        throw(std::runtime_error("not an object"));
    }
    //  A method implemented in each class that writes its value into a string given as argument with an indentation given as another argument.
    inline virtual void write(std::ostream& out, int = 0) const {
        out << "null";
    }
    inline void write_to_file(const std::string& filename) const {
        std::ofstream out(filename);
        if (!out.good()) 
            throw(std::runtime_error("Could not write to file " + filename));
        this->write(out, 0);
    }

    virtual ~JsonNode() = default;
protected:
    inline void write_string(std::ostream& out, const std::string& written) const {
        out.put('"');
        for (unsigned int i = 0; i < written.size(); i++) {
            out.put(written[i]);
        }
        out.put('"');
    }
    inline void indent(std::ostream& out, int depth) const {
        for (int i = 0; i < depth; i++){
            out.put(' ');
            out.put(' ');
        }
    }
};

// All types can be represented by similar C++ structures from the C++11 standard:
// null is not represented
// string is represented by std::string
// number is represented by double
// boolean is represented by bool
// object is represented by std::unordered_map<std::string, std::shared_ptr<JsonNode>>
// array is represented by std::vector<std::shared_ptr<JsonNode>>

struct JsonString : public JsonNode {
private:
    std::string contents_;
public:
    JsonString(const std::string& from = "") : contents_(from) {}

    inline virtual JsonType get_type() {
        return JsonType::STRING;
    }
    inline virtual std::string& get_string() {
        return contents_;
    }
    inline virtual void write(std::ostream& out, int = 0) const {
        write_string(out, contents_);
    }
};
struct JsonDouble : public JsonNode {
private:
    double value_;
public:
    JsonDouble(double from = 0) : value_(from) {}

    inline virtual JsonType get_type() {
        return JsonType::NUMBER;
    }
    inline virtual double& get_double() {
        return value_;
    }
    inline virtual void write(std::ostream& out, int = 0) const {
        out << std::fixed << value_;
    }
};
struct JsonBool : public JsonNode {
private:
    bool value_;
public:
    JsonBool(bool from = false) : value_(from) {}

    inline virtual JsonType get_type() {
        return JsonType::BOOL;
    }
    inline virtual bool& get_bool() {
        return value_;
    }
    inline virtual void write(std::ostream& out, int = 0) const {
        out << (value_ ? "true" : "false");
    }
};
struct JsonObject : public JsonNode {
private:
    std::unordered_map<std::string, std::shared_ptr<JsonNode>> contents_;
    std::vector<std::string> insert_order_; // record insert order
public:
    JsonObject() {}

    inline virtual JsonType get_type() {
        return JsonType::OBJECT;
    }
    inline virtual std::unordered_map<std::string, std::shared_ptr<JsonNode>>& get_object() {
        return contents_;
    }
    inline virtual std::vector<std::string>& get_insert_order() {
        return insert_order_;
    }
    inline virtual void write(std::ostream& out, int depth = 0) const {
        out.put('{');
        if (contents_.empty()) {
            out.put('}');
            return;
        }

        out.put('\n');
        bool first_ele = true;
        for (auto& it : insert_order_) {
            if (first_ele)
                first_ele = false;
            else {
                out.put(',');
                out.put('\n');
            }
            indent(out, depth + 1);
            write_string(out, it);
            out.put(':');
            out.put(' ');
            contents_.at(it)->write(out, depth + 1);
        }
        out.put('\n');
        indent(out, depth);
        out.put('}');
    }
};
struct JsonArray : public JsonNode {
private:
    std::vector<std::shared_ptr<JsonNode>> contents_;
public:
    JsonArray() {}

    inline virtual JsonType get_type() {
        return JsonType::ARRAY;
    }
    inline virtual std::vector<std::shared_ptr<JsonNode>>& get_array() {
        return contents_;
    }
    inline virtual void write(std::ostream& out, int depth = 0) const {
        out.put('[');
        if (contents_.empty()) {
            out.put(']');
            return;
        }
        bool first_ele = true;
        for (auto& it : contents_) {
            if(first_ele){
                first_ele = false;
            }
            else{
                out.put(',');
            }
            out.put('\n');
            indent(out, depth+1);
            it->write(out, depth + 1);
        }
        out.put('\n');
        indent(out, depth);
        out.put(']');
        
    }
};

// A recursive function that parses the input character by character.
std::shared_ptr<JsonNode> parse_json(std::istream& in) {
    // read until "
    auto read_string = [&in] () -> std::string {
        char letter = in.get();
        std::string collected;
        bool backslash = false;
        while (letter != '"' || backslash) //the end of string
        {
            if (letter == '\\') {
                backslash=!backslash;
            } 
            else{
                backslash=false;
            }
            collected.push_back(letter);
            letter = in.get();
        }
        return collected;
    };
    // ignore whitespace and get first char after whitespace
    auto read_whitespace = [&in] () -> char {
        char letter;
        do {
            letter = in.get();
        } while (letter == ' ' || letter == '\t' || letter == '\n' || letter == '\r');
        return letter;
    };

    char letter = read_whitespace();
    if (letter == 0 || letter == EOF) return std::make_shared<JsonNode>();
    else if (letter == '"') {
        return std::make_shared<JsonString>(read_string());
    }
    else if (letter == 't') {
        if (in.get() == 'r' && in.get() == 'u' && in.get() == 'e')
            return std::make_shared<JsonBool>(true);
        else
            throw(std::runtime_error("Misspelled bool 'true'"));
    }
    else if (letter == 'f') {
        if (in.get() == 'a' && in.get() == 'l' && in.get() == 's' && in.get() == 'e')
            return std::make_shared<JsonBool>(false);
        else
            throw(std::runtime_error("Misspelled bool 'false'"));
    }
    else if (letter == 'n') {
        if (in.get() == 'u' && in.get() == 'l' && in.get() == 'l')
            return std::make_shared<JsonNode>();
        else
            throw(std::runtime_error("Misspelled bool 'null'"));
    }
    else if (letter == '-' || (letter >= '0' && letter <= '9')) {
        std::string asString;
        asString.push_back(letter);
        do {
            letter = in.get();
            asString.push_back(letter);
        } while (letter == '-' || letter == 'E' || letter == 'e' || letter == '.' || (letter >= '0' && letter <= '9'));
        in.unget();
        std::stringstream parsing(asString);
        double number;
        parsing >> number;
        return std::make_shared<JsonDouble>(number);
    }
    else if (letter == '{') {
        auto retval = std::make_shared<JsonObject>();
        letter = read_whitespace();
        while (letter != '}') {
            if (letter == ',') {
                letter = read_whitespace();
            }
            if (letter == '"') {
                const std::string& name = read_string();
                letter = read_whitespace();
                if (letter != ':') throw(std::runtime_error("Missed an ':' somewhere"));
                retval->get_insert_order().push_back(name);
                retval->get_object()[name] = parse_json(in);
                letter = read_whitespace();
            } 
        }
        return retval;
    }
    else if (letter == '[') {
        auto retval = std::make_shared<JsonArray>();
        letter = read_whitespace();
        while (letter != ']'){
            if (letter == ',') {
                letter = read_whitespace(); 
            }
            in.unget();
            retval->get_array().push_back(parse_json(in));
            letter = read_whitespace();
        }
        return retval;
    } 
    else {
        throw(std::runtime_error(std::string("Unexpected character ") + letter));
    }
    return std::make_shared<JsonNode>();
}
// read from file and parse
std::shared_ptr<JsonNode> parse_json(const std::string& filename) {
    std::ifstream in(filename);
    if (!in.good()) return std::make_shared<JsonNode>();
    return parse_json(in);
}

}
#endif //MY_JSON