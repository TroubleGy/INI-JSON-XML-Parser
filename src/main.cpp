#include <iostream>
#include <string>
#include "parsers/ini_parser.h"
#include "parsers/json_parser.h"
#include "parsers/xml_parser.h"

using namespace parser;

int main() {
    std::cout << "=== INI JSON XML Parser Library Demo ===" << std::endl;
    std::cout << "A lightweight C++ library for parsing INI, JSON, and XML files" << std::endl;
    std::cout << std::endl;

    // INI Parser Example
    std::cout << "--- INI Parser Example ---" << std::endl;
    std::string ini_content = R"(
[Database]
host=localhost
port=5432
username=admin
password=secret123

[Settings]
debug=true
log_level=INFO
max_connections=100
)";

    INIParser ini_parser;
    auto ini_result = ini_parser.parse(ini_content);
    
    if (ini_result.success) {
        std::cout << "INI parsed successfully!" << std::endl;
        std::cout << "Database host: " << ini_result.get("Database", "host") << std::endl;
        std::cout << "Debug mode: " << ini_result.get("Settings", "debug") << std::endl;
    } else {
        std::cout << "INI parsing failed: " << ini_result.error_message << std::endl;
    }

    std::cout << std::endl;

    // JSON Parser Example
    std::cout << "--- JSON Parser Example ---" << std::endl;
    std::string json_content = R"({
    "name": "John Doe",
    "age": 30,
    "email": "john@example.com",
    "address": {
        "street": "123 Main St",
        "city": "New York",
        "zip": "10001"
    },
    "hobbies": ["reading", "gaming", "coding"]
})";

    JSONParser json_parser;
    auto json_result = json_parser.parse(json_content);
    
    if (json_result.success) {
        std::cout << "JSON parsed successfully!" << std::endl;
        std::cout << "Name: " << json_result.get_string("name") << std::endl;
        std::cout << "Age: " << json_result.get_int("age") << std::endl;
        std::cout << "City: " << json_result.get_string("address.city") << std::endl;
    } else {
        std::cout << "JSON parsing failed: " << json_result.error_message << std::endl;
    }

    std::cout << std::endl;

    // XML Parser Example
    std::cout << "--- XML Parser Example ---" << std::endl;
    std::string xml_content = R"(
<?xml version="1.0" encoding="UTF-8"?>
<config>
    <database>
        <host>localhost</host>
        <port>5432</port>
        <credentials>
            <username>admin</username>
            <password>secret123</password>
        </credentials>
    </database>
    <settings>
        <debug>true</debug>
        <log_level>INFO</log_level>
    </settings>
</config>
)";

    XMLParser xml_parser;
    auto xml_result = xml_parser.parse(xml_content);
    
    if (xml_result.success) {
        std::cout << "XML parsed successfully!" << std::endl;
        
        // Debug: print the structure
        std::cout << "Root name: " << xml_result.root.name << std::endl;
        std::cout << "Root children count: " << xml_result.root.children.size() << std::endl;
        
        for (const auto& child : xml_result.root.children) {
            std::cout << "Child: " << child.name << " (children: " << child.children.size() << ")" << std::endl;
            for (const auto& grandchild : child.children) {
                std::cout << "  Grandchild: " << grandchild.name << " = '" << grandchild.value << "' (children: " << grandchild.children.size() << ")" << std::endl;
                for (const auto& greatgrandchild : grandchild.children) {
                    std::cout << "    Great-grandchild: " << greatgrandchild.name << " = '" << greatgrandchild.value << "'" << std::endl;
                }
            }
        }
        
        std::cout << "Database host: " << xml_result.get_value("config.database.host") << std::endl;
        std::cout << "Username: " << xml_result.get_value("config.database.credentials.username") << std::endl;
        std::cout << "Debug mode: " << xml_result.get_value("config.settings.debug") << std::endl;

        std::cout << "Direct access test:" << std::endl;
        std::cout << "Database host: " << xml_result.root.children[0].children[0].value << std::endl;
        std::cout << "Username: " << xml_result.root.children[0].children[2].children[0].value << std::endl;
        std::cout << "Debug mode: " << xml_result.root.children[1].children[0].value << std::endl;
    } else {
        std::cout << "XML parsing failed: " << xml_result.error_message << std::endl;
    }

    std::cout << std::endl;
    std::cout << "Demo completed successfully!" << std::endl;
    
    return 0;
} 