#include "parsers/json_parser.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace parser {

    // JSONValue implementation
    std::string JSONValue::as_string() const {
        switch (type_) {
            case Type::String:
                return string_value_;
            case Type::Integer:
                return std::to_string(int_value_);
            case Type::Number:
                return std::to_string(double_value_);
            case Type::Boolean:
                return bool_value_ ? "true" : "false";
            case Type::Null:
                return "null";
            default:
                return "";
        }
    }

    int JSONValue::as_int() const {
        switch (type_) {
            case Type::String:
                try { return std::stoi(string_value_); } catch (...) { return 0; }
            case Type::Integer:
                return int_value_;
            case Type::Number:
                return static_cast<int>(double_value_);
            case Type::Boolean:
                return bool_value_ ? 1 : 0;
            default:
                return 0;
        }
    }

    double JSONValue::as_double() const {
        switch (type_) {
            case Type::String:
                try { return std::stod(string_value_); } catch (...) { return 0.0; }
            case Type::Integer:
                return static_cast<double>(int_value_);
            case Type::Number:
                return double_value_;
            case Type::Boolean:
                return bool_value_ ? 1.0 : 0.0;
            default:
                return 0.0;
        }
    }

    bool JSONValue::as_bool() const {
        switch (type_) {
            case Type::String:
                return !string_value_.empty() && string_value_ != "false" && string_value_ != "0";
            case Type::Integer:
                return int_value_ != 0;
            case Type::Number:
                return double_value_ != 0.0;
            case Type::Boolean:
                return bool_value_;
            default:
                return false;
        }
    }

    void JSONValue::set(const std::string& key, const JSONValue& value) {
        if (type_ != Type::Object) {
            type_ = Type::Object;
            string_value_.clear();
            int_value_ = 0;
            double_value_ = 0.0;
            bool_value_ = false;
            array_values_.clear();
        }
        object_values_[key] = value;
    }

    JSONValue JSONValue::get(const std::string& key) const {
        if (type_ != Type::Object) {
            return JSONValue();
        }
        
        auto it = object_values_.find(key);
        if (it != object_values_.end()) {
            return it->second;
        }
        return JSONValue();
    }

    bool JSONValue::has_key(const std::string& key) const {
        if (type_ != Type::Object) {
            return false;
        }
        return object_values_.find(key) != object_values_.end();
    }

    std::vector<std::string> JSONValue::get_keys() const {
        std::vector<std::string> keys;
        if (type_ == Type::Object) {
            for (const auto& pair : object_values_) {
                keys.push_back(pair.first);
            }
        }
        return keys;
    }

    void JSONValue::push_back(const JSONValue& value) {
        if (type_ != Type::Array) {
            type_ = Type::Array;
            string_value_.clear();
            int_value_ = 0;
            double_value_ = 0.0;
            bool_value_ = false;
            object_values_.clear();
        }
        array_values_.push_back(value);
    }

    JSONValue JSONValue::at(size_t index) const {
        if (type_ != Type::Array || index >= array_values_.size()) {
            return JSONValue();
        }
        return array_values_[index];
    }

    size_t JSONValue::size() const {
        if (type_ == Type::Array) {
            return array_values_.size();
        } else if (type_ == Type::Object) {
            return object_values_.size();
        }
        return 0;
    }

    // JSONResult implementation
    std::string JSONResult::get_string(const std::string& path, const std::string& default_value) const {
        JSONValue value = get_value(path);
        if (value.get_type() == JSONValue::Type::Null) {
            return default_value;
        }
        return value.as_string();
    }

    int JSONResult::get_int(const std::string& path, int default_value) const {
        JSONValue value = get_value(path);
        if (value.get_type() == JSONValue::Type::Null) {
            return default_value;
        }
        return value.as_int();
    }

    double JSONResult::get_double(const std::string& path, double default_value) const {
        JSONValue value = get_value(path);
        if (value.get_type() == JSONValue::Type::Null) {
            return default_value;
        }
        return value.as_double();
    }

    bool JSONResult::get_bool(const std::string& path, bool default_value) const {
        JSONValue value = get_value(path);
        if (value.get_type() == JSONValue::Type::Null) {
            return default_value;
        }
        return value.as_bool();
    }

    JSONValue JSONResult::get_value(const std::string& path) const {
        if (path.empty()) {
            return root;
        }
        
        std::istringstream path_stream(path);
        std::string component;
        JSONValue current = root;
        
        while (std::getline(path_stream, component, '.')) {
            if (current.get_type() != JSONValue::Type::Object) {
                return JSONValue();
            }
            
            current = current.get(component);
            if (current.get_type() == JSONValue::Type::Null) {
                return JSONValue();
            }
        }
        
        return current;
    }

    bool JSONResult::has_path(const std::string& path) const {
        JSONValue value = get_value(path);
        return value.get_type() != JSONValue::Type::Null;
    }

    std::vector<std::string> JSONResult::get_keys(const std::string& path) const {
        JSONValue value = get_value(path);
        if (value.get_type() == JSONValue::Type::Object) {
            return value.get_keys();
        }
        return {};
    }

    // JSONParser implementation
    JSONResult JSONParser::parse(const std::string& content) {
        JSONResult result;
        size_t pos = 0;
        
        try {
            skip_whitespace(content, pos);
            result.root = parse_value(content, pos);
            result.success = true;
        } catch (const std::exception& e) {
            result.success = false;
            result.error_message = e.what();
        }
        
        return result;
    }

    JSONResult JSONParser::parse_file(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            JSONResult result;
            result.success = false;
            result.error_message = "Cannot open file: " + filename;
            return result;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        return parse(buffer.str());
    }

    std::string JSONParser::to_string(const JSONResult& result, bool pretty_print) {
        return value_to_string(result.root, 0, pretty_print);
    }

    bool JSONParser::save_to_file(const JSONResult& result, const std::string& filename, bool pretty_print) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        file << to_string(result, pretty_print);
        return true;
    }

    // Private helper methods
    JSONValue JSONParser::parse_value(const std::string& content, size_t& pos) {
        skip_whitespace(content, pos);
        
        if (pos >= content.length()) {
            throw std::runtime_error("Unexpected end of input");
        }
        
        char c = content[pos];
        
        if (c == '{') {
            return parse_object(content, pos);
        } else if (c == '[') {
            return parse_array(content, pos);
        } else if (c == '"') {
            return JSONValue(parse_string(content, pos));
        } else if (c == 't' || c == 'f') {
            // Boolean
            if (content.substr(pos, 4) == "true") {
                pos += 4;
                return JSONValue(true);
            } else if (content.substr(pos, 5) == "false") {
                pos += 5;
                return JSONValue(false);
            } else {
                throw std::runtime_error("Invalid boolean value");
            }
        } else if (c == 'n') {
            // Null
            if (content.substr(pos, 4) == "null") {
                pos += 4;
                return JSONValue();
            } else {
                throw std::runtime_error("Invalid null value");
            }
        } else if (std::isdigit(c) || c == '-') {
            return parse_number(content, pos);
        } else {
            throw std::runtime_error("Unexpected character: " + std::string(1, c));
        }
    }

    JSONValue JSONParser::parse_object(const std::string& content, size_t& pos) {
        JSONValue obj;
        obj.set("", JSONValue()); // Initialize as object
        
        pos++; // Skip '{'
        skip_whitespace(content, pos);
        
        if (pos < content.length() && content[pos] == '}') {
            pos++; // Skip '}'
            return obj;
        }
        
        while (pos < content.length()) {
            skip_whitespace(content, pos);
            
            if (content[pos] != '"') {
                throw std::runtime_error("Expected string key in object");
            }
            
            std::string key = parse_string(content, pos);
            skip_whitespace(content, pos);
            
            if (pos >= content.length() || content[pos] != ':') {
                throw std::runtime_error("Expected ':' after key");
            }
            
            pos++; // Skip ':'
            skip_whitespace(content, pos);
            
            JSONValue value = parse_value(content, pos);
            obj.set(key, value);
            
            skip_whitespace(content, pos);
            
            if (pos >= content.length()) {
                throw std::runtime_error("Unexpected end of input in object");
            }
            
            if (content[pos] == '}') {
                pos++; // Skip '}'
                break;
            } else if (content[pos] == ',') {
                pos++; // Skip ','
            } else {
                throw std::runtime_error("Expected ',' or '}' in object");
            }
        }
        
        return obj;
    }

    JSONValue JSONParser::parse_array(const std::string& content, size_t& pos) {
        JSONValue arr;
        
        pos++; // Skip '['
        skip_whitespace(content, pos);
        
        if (pos < content.length() && content[pos] == ']') {
            pos++; // Skip ']'
            return arr;
        }
        
        while (pos < content.length()) {
            skip_whitespace(content, pos);
            
            JSONValue value = parse_value(content, pos);
            arr.push_back(value);
            
            skip_whitespace(content, pos);
            
            if (pos >= content.length()) {
                throw std::runtime_error("Unexpected end of input in array");
            }
            
            if (content[pos] == ']') {
                pos++; // Skip ']'
                break;
            } else if (content[pos] == ',') {
                pos++; // Skip ','
            } else {
                throw std::runtime_error("Expected ',' or ']' in array");
            }
        }
        
        return arr;
    }

    std::string JSONParser::parse_string(const std::string& content, size_t& pos) {
        if (content[pos] != '"') {
            throw std::runtime_error("Expected '\"' at start of string");
        }
        
        pos++; // Skip opening quote
        std::string result;
        
        while (pos < content.length()) {
            char c = content[pos++];
            
            if (c == '"') {
                return result;
            } else if (c == '\\') {
                if (pos >= content.length()) {
                    throw std::runtime_error("Unexpected end of input in string");
                }
                
                char escape = content[pos++];
                switch (escape) {
                    case '"': result += '"'; break;
                    case '\\': result += '\\'; break;
                    case '/': result += '/'; break;
                    case 'b': result += '\b'; break;
                    case 'f': result += '\f'; break;
                    case 'n': result += '\n'; break;
                    case 'r': result += '\r'; break;
                    case 't': result += '\t'; break;
                    default:
                        throw std::runtime_error("Invalid escape sequence: \\" + std::string(1, escape));
                }
            } else {
                result += c;
            }
        }
        
        throw std::runtime_error("Unterminated string");
    }

    JSONValue JSONParser::parse_number(const std::string& content, size_t& pos) {
        size_t start = pos;
        
        if (content[pos] == '-') {
            pos++;
        }
        
        while (pos < content.length() && std::isdigit(content[pos])) {
            pos++;
        }
        
        if (pos < content.length() && content[pos] == '.') {
            pos++;
            while (pos < content.length() && std::isdigit(content[pos])) {
                pos++;
            }
        }
        
        if (pos < content.length() && (content[pos] == 'e' || content[pos] == 'E')) {
            pos++;
            if (pos < content.length() && (content[pos] == '+' || content[pos] == '-')) {
                pos++;
            }
            while (pos < content.length() && std::isdigit(content[pos])) {
                pos++;
            }
        }
        
        std::string num_str = content.substr(start, pos - start);
        
        try {
            if (num_str.find('.') != std::string::npos || 
                num_str.find('e') != std::string::npos || 
                num_str.find('E') != std::string::npos) {
                return JSONValue(std::stod(num_str));
            } else {
                return JSONValue(std::stoi(num_str));
            }
        } catch (const std::exception&) {
            throw std::runtime_error("Invalid number: " + num_str);
        }
    }

    void JSONParser::skip_whitespace(const std::string& content, size_t& pos) {
        while (pos < content.length() && std::isspace(content[pos])) {
            pos++;
        }
    }

    std::string JSONParser::value_to_string(const JSONValue& value, int indent, bool pretty_print) {
        std::string indent_str = pretty_print ? std::string(indent * 2, ' ') : "";
        std::string newline = pretty_print ? "\n" : "";
        
        switch (value.get_type()) {
            case JSONValue::Type::String:
                return "\"" + value.as_string() + "\"";
            case JSONValue::Type::Integer:
            case JSONValue::Type::Number:
                return value.as_string();
            case JSONValue::Type::Boolean:
                return value.as_string();
            case JSONValue::Type::Null:
                return "null";
            case JSONValue::Type::Object: {
                std::string result = "{" + newline;
                bool first = true;
                for (const auto& key : value.get_keys()) {
                    if (!first) {
                        result += "," + newline;
                    }
                    result += indent_str + (pretty_print ? "  " : "") + "\"" + key + "\": " + 
                             value_to_string(value.get(key), indent + 1, pretty_print);
                    first = false;
                }
                result += newline + indent_str + "}";
                return result;
            }
            case JSONValue::Type::Array: {
                std::string result = "[" + newline;
                bool first = true;
                for (size_t i = 0; i < value.size(); ++i) {
                    if (!first) {
                        result += "," + newline;
                    }
                    result += indent_str + (pretty_print ? "  " : "") + 
                             value_to_string(value.at(i), indent + 1, pretty_print);
                    first = false;
                }
                result += newline + indent_str + "]";
                return result;
            }
            default:
                return "";
        }
    }

} // namespace parser 