#include "parsers/ini_parser.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace parser {

    // INIResult implementation
    std::string INIResult::get(const std::string& section_name, const std::string& key) const {
        auto section_it = sections.find(section_name);
        if (section_it == sections.end()) {
            return "";
        }
        
        auto key_it = section_it->second.find(key);
        if (key_it == section_it->second.end()) {
            return "";
        }
        
        return key_it->second;
    }

    int INIResult::get_int(const std::string& section_name, const std::string& key, int default_value) const {
        std::string value = get(section_name, key);
        if (value.empty()) {
            return default_value;
        }
        
        try {
            return std::stoi(value);
        } catch (const std::exception&) {
            return default_value;
        }
    }

    bool INIResult::get_bool(const std::string& section_name, const std::string& key, bool default_value) const {
        std::string value = get(section_name, key);
        if (value.empty()) {
            return default_value;
        }
        
        // Convert to lowercase for comparison
        std::transform(value.begin(), value.end(), value.begin(), ::tolower);
        
        return (value == "true" || value == "1" || value == "yes" || value == "on");
    }

    double INIResult::get_double(const std::string& section_name, const std::string& key, double default_value) const {
        std::string value = get(section_name, key);
        if (value.empty()) {
            return default_value;
        }
        
        try {
            return std::stod(value);
        } catch (const std::exception&) {
            return default_value;
        }
    }

    bool INIResult::has_section(const std::string& section_name) const {
        return sections.find(section_name) != sections.end();
    }

    bool INIResult::has_key(const std::string& section_name, const std::string& key) const {
        auto section_it = sections.find(section_name);
        if (section_it == sections.end()) {
            return false;
        }
        
        return section_it->second.find(key) != section_it->second.end();
    }

    std::vector<std::string> INIResult::get_sections() const {
        std::vector<std::string> result;
        for (const auto& section : sections) {
            result.push_back(section.first);
        }
        return result;
    }

    std::vector<std::string> INIResult::get_keys(const std::string& section_name) const {
        std::vector<std::string> result;
        auto section_it = sections.find(section_name);
        if (section_it != sections.end()) {
            for (const auto& key : section_it->second) {
                result.push_back(key.first);
            }
        }
        return result;
    }

    // INIParser implementation
    INIResult INIParser::parse(const std::string& content) {
        INIResult result;
        std::istringstream stream(content);
        std::string line;
        std::string current_section = "";
        
        while (std::getline(stream, line)) {
            line = trim(line);
            
            if (is_empty(line) || is_comment(line)) {
                continue;
            }
            
            if (is_section(line)) {
                current_section = extract_section(line);
                if (current_section.empty()) {
                    result.success = false;
                    result.error_message = "Invalid section format: " + line;
                    return result;
                }
            } else {
                if (current_section.empty()) {
                    result.success = false;
                    result.error_message = "Key-value pair found outside of section: " + line;
                    return result;
                }
                
                auto key_value = parse_key_value(line);
                if (key_value.first.empty()) {
                    result.success = false;
                    result.error_message = "Invalid key-value format: " + line;
                    return result;
                }
                
                result.sections[current_section][key_value.first] = key_value.second;
            }
        }
        
        result.success = true;
        return result;
    }

    INIResult INIParser::parse_file(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            INIResult result;
            result.success = false;
            result.error_message = "Cannot open file: " + filename;
            return result;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        return parse(buffer.str());
    }

    std::string INIParser::to_string(const INIResult& result) {
        std::ostringstream output;
        
        for (const auto& section : result.sections) {
            output << "[" << section.first << "]" << std::endl;
            
            for (const auto& key_value : section.second) {
                output << key_value.first << "=" << key_value.second << std::endl;
            }
            
            output << std::endl;
        }
        
        return output.str();
    }

    bool INIParser::save_to_file(const INIResult& result, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        file << to_string(result);
        return true;
    }

    // Private helper methods
    std::string INIParser::trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) {
            return "";
        }
        
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }

    bool INIParser::is_comment(const std::string& line) {
        std::string trimmed = trim(line);
        return !trimmed.empty() && (trimmed[0] == '#' || trimmed[0] == ';');
    }

    bool INIParser::is_empty(const std::string& line) {
        return trim(line).empty();
    }

    bool INIParser::is_section(const std::string& line) {
        std::string trimmed = trim(line);
        return trimmed.length() >= 2 && trimmed[0] == '[' && trimmed[trimmed.length() - 1] == ']';
    }

    std::string INIParser::extract_section(const std::string& line) {
        std::string trimmed = trim(line);
        if (trimmed.length() < 2) {
            return "";
        }
        
        return trimmed.substr(1, trimmed.length() - 2);
    }

    std::pair<std::string, std::string> INIParser::parse_key_value(const std::string& line) {
        size_t equal_pos = line.find('=');
        if (equal_pos == std::string::npos) {
            return {"", ""};
        }
        
        std::string key = trim(line.substr(0, equal_pos));
        std::string value = trim(line.substr(equal_pos + 1));
        
        // Remove quotes if present
        if (value.length() >= 2 && 
            ((value[0] == '"' && value[value.length() - 1] == '"') ||
             (value[0] == '\'' && value[value.length() - 1] == '\''))) {
            value = value.substr(1, value.length() - 2);
        }
        
        return {key, value};
    }

} // namespace parser 