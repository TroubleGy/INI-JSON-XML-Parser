#pragma once

#include <string>
#include <map>
#include <vector>
#include <variant>

namespace parser {

    // Forward declaration for JSON value types
    class JSONValue;

    /**
     * @brief JSON value types supported by the parser
     */
    using JSONValueType = std::variant<std::string, int, double, bool, std::nullptr_t>;

    /**
     * @brief JSON value class that can hold different types
     */
    class JSONValue {
    public:
        enum class Type {
            String,
            Number,
            Integer,
            Boolean,
            Null,
            Object,
            Array
        };

        JSONValue() : type_(Type::Null) {}
        JSONValue(const std::string& value) : type_(Type::String), string_value_(value) {}
        JSONValue(int value) : type_(Type::Integer), int_value_(value) {}
        JSONValue(double value) : type_(Type::Number), double_value_(value) {}
        JSONValue(bool value) : type_(Type::Boolean), bool_value_(value) {}

        Type get_type() const { return type_; }
        
        std::string as_string() const;
        int as_int() const;
        double as_double() const;
        bool as_bool() const;
        
        // Object methods
        void set(const std::string& key, const JSONValue& value);
        JSONValue get(const std::string& key) const;
        bool has_key(const std::string& key) const;
        std::vector<std::string> get_keys() const;
        
        // Array methods
        void push_back(const JSONValue& value);
        JSONValue at(size_t index) const;
        size_t size() const;
        bool is_array() const { return type_ == Type::Array; }
        bool is_object() const { return type_ == Type::Object; }

    private:
        Type type_;
        std::string string_value_;
        int int_value_ = 0;
        double double_value_ = 0.0;
        bool bool_value_ = false;
        std::map<std::string, JSONValue> object_values_;
        std::vector<JSONValue> array_values_;
    };

    /**
     * @brief Result structure for JSON parsing operations
     */
    struct JSONResult {
        bool success = false;
        std::string error_message;
        JSONValue root;
        
        /**
         * @brief Get a string value by path (e.g., "address.city")
         * @param path The path to the value
         * @param default_value Default value if not found
         * @return The string value
         */
        std::string get_string(const std::string& path, const std::string& default_value = "") const;
        
        /**
         * @brief Get an integer value by path
         * @param path The path to the value
         * @param default_value Default value if not found
         * @return The integer value
         */
        int get_int(const std::string& path, int default_value = 0) const;
        
        /**
         * @brief Get a double value by path
         * @param path The path to the value
         * @param default_value Default value if not found
         * @return The double value
         */
        double get_double(const std::string& path, double default_value = 0.0) const;
        
        /**
         * @brief Get a boolean value by path
         * @param path The path to the value
         * @param default_value Default value if not found
         * @return The boolean value
         */
        bool get_bool(const std::string& path, bool default_value = false) const;
        
        /**
         * @brief Get a JSON value by path
         * @param path The path to the value
         * @return The JSON value
         */
        JSONValue get_value(const std::string& path) const;
        
        /**
         * @brief Check if a path exists
         * @param path The path to check
         * @return True if path exists
         */
        bool has_path(const std::string& path) const;
        
        /**
         * @brief Get all keys at a specific path
         * @param path The path to get keys from
         * @return Vector of key names
         */
        std::vector<std::string> get_keys(const std::string& path = "") const;
    };

    /**
     * @brief JSON file parser class
     * 
     * A lightweight and efficient JSON parser that supports:
     * - Objects and arrays
     * - String, number, boolean, and null values
     * - Nested structures
     * - Path-based access (e.g., "address.city")
     * - Type conversion
     */
    class JSONParser {
    public:
        /**
         * @brief Parse JSON content from string
         * @param content The JSON content as string
         * @return JSONResult with parsed data or error information
         */
        JSONResult parse(const std::string& content);
        
        /**
         * @brief Parse JSON content from file
         * @param filename The path to the JSON file
         * @return JSONResult with parsed data or error information
         */
        JSONResult parse_file(const std::string& filename);
        
        /**
         * @brief Convert parsed data back to JSON format
         * @param result The parsed JSON result
         * @param pretty_print Whether to format with indentation
         * @return JSON content as string
         */
        std::string to_string(const JSONResult& result, bool pretty_print = false);
        
        /**
         * @brief Save parsed data to file
         * @param result The parsed JSON result
         * @param filename The output file path
         * @param pretty_print Whether to format with indentation
         * @return True if successful
         */
        bool save_to_file(const JSONResult& result, const std::string& filename, bool pretty_print = false);

    private:
        /**
         * @brief Parse JSON value from string
         * @param content The JSON content
         * @param pos Current position in the content
         * @return Parsed JSON value
         */
        JSONValue parse_value(const std::string& content, size_t& pos);
        
        /**
         * @brief Parse JSON object from string
         * @param content The JSON content
         * @param pos Current position in the content
         * @return Parsed JSON object
         */
        JSONValue parse_object(const std::string& content, size_t& pos);
        
        /**
         * @brief Parse JSON array from string
         * @param content The JSON content
         * @param pos Current position in the content
         * @return Parsed JSON array
         */
        JSONValue parse_array(const std::string& content, size_t& pos);
        
        /**
         * @brief Parse JSON string from string
         * @param content The JSON content
         * @param pos Current position in the content
         * @return Parsed string value
         */
        std::string parse_string(const std::string& content, size_t& pos);
        
        /**
         * @brief Parse JSON number from string
         * @param content The JSON content
         * @param pos Current position in the content
         * @return Parsed number value
         */
        JSONValue parse_number(const std::string& content, size_t& pos);
        
        /**
         * @brief Skip whitespace characters
         * @param content The JSON content
         * @param pos Current position in the content
         */
        void skip_whitespace(const std::string& content, size_t& pos);
        
        /**
         * @brief Convert JSON value to string representation
         * @param value The JSON value to convert
         * @param indent Current indentation level
         * @param pretty_print Whether to format with indentation
         * @return String representation
         */
        std::string value_to_string(const JSONValue& value, int indent = 0, bool pretty_print = false);
    };

} // namespace parser 