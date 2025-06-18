#pragma once

#include <string>
#include <map>
#include <vector>

namespace parser {

    /**
     * @brief Result structure for INI parsing operations
     */
    struct INIResult {
        bool success = false;
        std::string error_message;
        std::map<std::string, std::map<std::string, std::string>> sections;
        
        /**
         * @brief Get a value from a specific section and key
         * @param section_name The section name
         * @param key The key name
         * @return The value as string, or empty string if not found
         */
        std::string get(const std::string& section_name, const std::string& key) const;
        
        /**
         * @brief Get a value as integer
         * @param section_name The section name
         * @param key The key name
         * @param default_value Default value if conversion fails
         * @return The value as integer
         */
        int get_int(const std::string& section_name, const std::string& key, int default_value = 0) const;
        
        /**
         * @brief Get a value as boolean
         * @param section_name The section name
         * @param key The key name
         * @param default_value Default value if conversion fails
         * @return The value as boolean
         */
        bool get_bool(const std::string& section_name, const std::string& key, bool default_value = false) const;
        
        /**
         * @brief Get a value as double
         * @param section_name The section name
         * @param key The key name
         * @param default_value Default value if conversion fails
         * @return The value as double
         */
        double get_double(const std::string& section_name, const std::string& key, double default_value = 0.0) const;
        
        /**
         * @brief Check if a section exists
         * @param section_name The section name
         * @return True if section exists
         */
        bool has_section(const std::string& section_name) const;
        
        /**
         * @brief Check if a key exists in a section
         * @param section_name The section name
         * @param key The key name
         * @return True if key exists
         */
        bool has_key(const std::string& section_name, const std::string& key) const;
        
        /**
         * @brief Get all section names
         * @return Vector of section names
         */
        std::vector<std::string> get_sections() const;
        
        /**
         * @brief Get all keys in a section
         * @param section_name The section name
         * @return Vector of key names
         */
        std::vector<std::string> get_keys(const std::string& section_name) const;
    };

    /**
     * @brief INI file parser class
     * 
     * A lightweight and efficient INI file parser that supports:
     * - Section-based configuration
     * - Key-value pairs
     * - Comments (lines starting with # or ;)
     * - Empty lines
     * - Type conversion (string, int, bool, double)
     */
    class INIParser {
    public:
        /**
         * @brief Parse INI content from string
         * @param content The INI content as string
         * @return INIResult with parsed data or error information
         */
        INIResult parse(const std::string& content);
        
        /**
         * @brief Parse INI content from file
         * @param filename The path to the INI file
         * @return INIResult with parsed data or error information
         */
        INIResult parse_file(const std::string& filename);
        
        /**
         * @brief Convert parsed data back to INI format
         * @param result The parsed INI result
         * @return INI content as string
         */
        std::string to_string(const INIResult& result);
        
        /**
         * @brief Save parsed data to file
         * @param result The parsed INI result
         * @param filename The output file path
         * @return True if successful
         */
        bool save_to_file(const INIResult& result, const std::string& filename);

    private:
        /**
         * @brief Trim whitespace from string
         * @param str The string to trim
         * @return Trimmed string
         */
        std::string trim(const std::string& str);
        
        /**
         * @brief Check if line is a comment
         * @param line The line to check
         * @return True if line is a comment
         */
        bool is_comment(const std::string& line);
        
        /**
         * @brief Check if line is empty
         * @param line The line to check
         * @return True if line is empty
         */
        bool is_empty(const std::string& line);
        
        /**
         * @brief Check if line is a section header
         * @param line The line to check
         * @return True if line is a section header
         */
        bool is_section(const std::string& line);
        
        /**
         * @brief Extract section name from line
         * @param line The line containing section
         * @return Section name
         */
        std::string extract_section(const std::string& line);
        
        /**
         * @brief Parse key-value pair from line
         * @param line The line to parse
         * @return Pair of key and value
         */
        std::pair<std::string, std::string> parse_key_value(const std::string& line);
    };

} // namespace parser 