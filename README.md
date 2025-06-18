# INI JSON XML Parser Library

A lightweight, header-only C++ library for parsing INI, JSON, and XML files. This library provides a simple and efficient way to read, write, and manipulate configuration files in various formats.

## Features

### INI Parser
- ✅ Section-based configuration parsing
- ✅ Key-value pair support
- ✅ Comments (lines starting with # or ;)
- ✅ Type conversion (string, int, bool, double)
- ✅ File I/O operations
- ✅ Error handling with detailed messages

### JSON Parser
- ✅ Full JSON specification support
- ✅ Objects and arrays
- ✅ String, number, boolean, and null values
- ✅ Nested structures
- ✅ Path-based access (e.g., "address.city")
- ✅ Pretty printing
- ✅ Type conversion

### XML Parser
- ✅ Element parsing with attributes
- ✅ Nested structures
- ✅ Path-based access (e.g., "config.database.host")
- ✅ Attribute access
- ✅ Text content extraction
- ✅ XML entity handling
- ✅ Comments and processing instructions

## Requirements

- C++17 or later
- Visual Studio 2019/2022 (Windows)
- GCC 7+ or Clang 5+ (Linux/macOS)

## Installation

### Option 1: Include as Header-Only Library

Simply copy the `include/parsers/` directory to your project and include the headers:

```cpp
#include "parsers/ini_parser.h"
#include "parsers/json_parser.h"
#include "parsers/xml_parser.h"
```

### Option 2: Build from Source

1. Clone the repository:
```bash
git clone https://github.com/TroubleGy/INI-JSON-XML-Parser.git
cd INI_JSON_XML_Parser
```

2. Build the project:
```bash
# Windows (Visual Studio)
msbuild INI_JSON_XML_Parser.sln

# Linux/macOS
mkdir build && cd build
cmake ..
make
```

## Quick Start

### INI File Parsing

```cpp
#include "parsers/ini_parser.h"

using namespace parser;

// Parse INI content
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
auto result = ini_parser.parse(ini_content);

if (result.success) {
    // Get values
    std::string host = result.get("Database", "host");
    int port = result.get_int("Database", "port");
    bool debug = result.get_bool("Settings", "debug");
    
    std::cout << "Host: " << host << std::endl;
    std::cout << "Port: " << port << std::endl;
    std::cout << "Debug: " << (debug ? "true" : "false") << std::endl;
} else {
    std::cout << "Error: " << result.error_message << std::endl;
}
```

### JSON File Parsing

```cpp
#include "parsers/json_parser.h"

using namespace parser;

// Parse JSON content
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
auto result = json_parser.parse(json_content);

if (result.success) {
    // Get values using path notation
    std::string name = result.get_string("name");
    int age = result.get_int("age");
    std::string city = result.get_string("address.city");
    
    std::cout << "Name: " << name << std::endl;
    std::cout << "Age: " << age << std::endl;
    std::cout << "City: " << city << std::endl;
} else {
    std::cout << "Error: " << result.error_message << std::endl;
}
```

### XML File Parsing

```cpp
#include "parsers/xml_parser.h"

using namespace parser;

// Parse XML content
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
auto result = xml_parser.parse(xml_content);

if (result.success) {
    // Get values using path notation
    std::string host = result.get_value("config.database.host");
    std::string username = result.get_value("config.database.credentials.username");
    bool debug = result.get_value("config.settings.debug") == "true";
    
    std::cout << "Host: " << host << std::endl;
    std::cout << "Username: " << username << std::endl;
    std::cout << "Debug: " << (debug ? "true" : "false") << std::endl;
} else {
    std::cout << "Error: " << result.error_message << std::endl;
}
```

## API Reference

### INI Parser

#### INIResult Methods
- `get(section, key)` - Get string value
- `get_int(section, key, default_value)` - Get integer value
- `get_bool(section, key, default_value)` - Get boolean value
- `get_double(section, key, default_value)` - Get double value
- `has_section(section)` - Check if section exists
- `has_key(section, key)` - Check if key exists
- `get_sections()` - Get all section names
- `get_keys(section)` - Get all keys in section

#### INIParser Methods
- `parse(content)` - Parse INI string
- `parse_file(filename)` - Parse INI file
- `to_string(result)` - Convert to INI string
- `save_to_file(result, filename)` - Save to file

### JSON Parser

#### JSONResult Methods
- `get_string(path, default_value)` - Get string value by path
- `get_int(path, default_value)` - Get integer value by path
- `get_double(path, default_value)` - Get double value by path
- `get_bool(path, default_value)` - Get boolean value by path
- `get_value(path)` - Get JSON value by path
- `has_path(path)` - Check if path exists
- `get_keys(path)` - Get all keys at path

#### JSONParser Methods
- `parse(content)` - Parse JSON string
- `parse_file(filename)` - Parse JSON file
- `to_string(result, pretty_print)` - Convert to JSON string
- `save_to_file(result, filename, pretty_print)` - Save to file

### XML Parser

#### XMLResult Methods
- `get_value(path, default_value)` - Get value by path
- `get_attribute(path, attr_name, default_value)` - Get attribute value
- `get_node(path)` - Get node by path
- `has_path(path)` - Check if path exists
- `get_children(path)` - Get all child names
- `get_attributes(path)` - Get all attribute names

#### XMLParser Methods
- `parse(content)` - Parse XML string
- `parse_file(filename)` - Parse XML file
- `to_string(result, pretty_print)` - Convert to XML string
- `save_to_file(result, filename, pretty_print)` - Save to file

## Error Handling

All parsers return a result structure with:
- `success` - Boolean indicating if parsing was successful
- `error_message` - Detailed error description if parsing failed

```cpp
auto result = parser.parse(content);
if (!result.success) {
    std::cerr << "Parsing failed: " << result.error_message << std::endl;
    return;
}
```

## Performance

This library is designed for lightweight applications and configuration files. For high-performance requirements or very large files, consider using specialized libraries like:
- RapidJSON for JSON
- pugixml for XML
- inih for INI

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request. For major changes, please open an issue first to discuss what you would like to change.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Roadmap

- [ ] YAML parser support
- [ ] TOML parser support
- [ ] Binary format support
- [ ] Schema validation
- [ ] Streaming parser for large files
- [ ] More comprehensive XML entity handling
- [ ] Performance optimizations

## Acknowledgments

- Inspired by the need for a simple, unified configuration parsing library
- Built with modern C++17 features for better performance and safety
- Designed to be easy to use and integrate into existing projects 
