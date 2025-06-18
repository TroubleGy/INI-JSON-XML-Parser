#include "parsers/xml_parser.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace parser {

    // XMLNode implementation
    XMLNode* XMLNode::get_child(const std::string& child_name) {
        for (auto& child : children) {
            if (child.name == child_name) {
                return &child;
            }
        }
        return nullptr;
    }

    const XMLNode* XMLNode::get_child(const std::string& child_name) const {
        for (const auto& child : children) {
            if (child.name == child_name) {
                return &child;
            }
        }
        return nullptr;
    }

    std::string XMLNode::get_attribute(const std::string& attr_name, const std::string& default_value) const {
        auto it = attributes.find(attr_name);
        if (it != attributes.end()) {
            return it->second;
        }
        return default_value;
    }

    bool XMLNode::has_attribute(const std::string& attr_name) const {
        return attributes.find(attr_name) != attributes.end();
    }

    std::vector<XMLNode*> XMLNode::get_children(const std::string& child_name) {
        std::vector<XMLNode*> result;
        for (auto& child : children) {
            if (child.name == child_name) {
                result.push_back(&child);
            }
        }
        return result;
    }

    std::vector<const XMLNode*> XMLNode::get_children(const std::string& child_name) const {
        std::vector<const XMLNode*> result;
        for (const auto& child : children) {
            if (child.name == child_name) {
                result.push_back(&child);
            }
        }
        return result;
    }

    void XMLNode::add_child(const XMLNode& child) {
        children.push_back(child);
        children.back().parent = this;
    }

    void XMLNode::set_attribute(const std::string& name, const std::string& value) {
        attributes[name] = value;
    }

    // XMLResult implementation
    std::string XMLResult::get_value(const std::string& path, const std::string& default_value) const {
        const XMLNode* node = get_node(path);
        if (node) {
            return node->value;
        }
        return default_value;
    }

    std::string XMLResult::get_attribute(const std::string& path, const std::string& attr_name, const std::string& default_value) const {
        const XMLNode* node = get_node(path);
        if (node) {
            return node->get_attribute(attr_name, default_value);
        }
        return default_value;
    }

    const XMLNode* XMLResult::get_node(const std::string& path) const {
        if (path.empty()) {
            return &root;
        }
        std::vector<std::string> components;
        std::istringstream path_stream(path);
        std::string component;
        while (std::getline(path_stream, component, '.')) {
            if (!component.empty()) {
                components.push_back(component);
            }
        }
        const XMLNode* current = &root;
        for (const auto& comp : components) {
            bool found = false;
            for (size_t i = 0; i < current->children.size(); ++i) {
                if (current->children[i].name == comp) {
                    current = &current->children[i];
                    found = true;
                    break;
                }
            }
            if (!found) {
                return nullptr;
            }
        }
        return current;
    }

    bool XMLResult::has_path(const std::string& path) const {
        return get_node(path) != nullptr;
    }

    std::vector<std::string> XMLResult::get_children(const std::string& path) const {
        const XMLNode* node = get_node(path);
        if (!node) {
            return {};
        }
        
        std::vector<std::string> result;
        for (const auto& child : node->children) {
            result.push_back(child.name);
        }
        return result;
    }

    std::vector<std::string> XMLResult::get_attributes(const std::string& path) const {
        const XMLNode* node = get_node(path);
        if (!node) {
            return {};
        }
        
        std::vector<std::string> result;
        for (const auto& attr : node->attributes) {
            result.push_back(attr.first);
        }
        return result;
    }

    // XMLParser implementation
    XMLResult XMLParser::parse(const std::string& content) {
        XMLResult result;
        size_t pos = 0;
        
        try {
            skip_whitespace(content, pos);
            
            // Skip XML declaration if present
            if (pos < content.length() && content[pos] == '<' && pos + 1 < content.length() && content[pos + 1] == '?') {
                skip_processing_instructions(content, pos);
                skip_whitespace(content, pos);
            }
            
            // Skip comments if present
            while (pos < content.length() && content[pos] == '<' && pos + 3 < content.length() && 
                   content.substr(pos, 4) == "<!--") {
                skip_comments(content, pos);
                skip_whitespace(content, pos);
            }
            
            if (pos >= content.length()) {
                throw std::runtime_error("No root element found");
            }
            
            result.root = parse_node(content, pos, nullptr);
            result.success = true;
        } catch (const std::exception& e) {
            result.success = false;
            result.error_message = e.what();
        }
        
        return result;
    }

    XMLResult XMLParser::parse_file(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            XMLResult result;
            result.success = false;
            result.error_message = "Cannot open file: " + filename;
            return result;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        return parse(buffer.str());
    }

    std::string XMLParser::to_string(const XMLResult& result, bool pretty_print) {
        return node_to_string(result.root, 0, pretty_print);
    }

    bool XMLParser::save_to_file(const XMLResult& result, const std::string& filename, bool pretty_print) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << (pretty_print ? "\n" : "");
        file << to_string(result, pretty_print);
        return true;
    }

    // Private helper methods
    XMLNode XMLParser::parse_node(const std::string& content, size_t& pos, XMLNode* parent) {
        XMLNode node;
        node.parent = parent;
        
        skip_whitespace(content, pos);
        
        if (pos >= content.length() || content[pos] != '<') {
            throw std::runtime_error("Expected '<' at start of element");
        }
        
        pos++; // Skip '<'
        
        if (pos >= content.length()) {
            throw std::runtime_error("Unexpected end of input");
        }
        
        // Check for closing tag
        if (content[pos] == '/') {
            throw std::runtime_error("Unexpected closing tag");
        }
        
        // Parse element name and attributes
        if (!parse_element_tag(content, pos, node)) {
            throw std::runtime_error("Failed to parse element tag");
        }
        
        skip_whitespace(content, pos);
        
        // Check for self-closing tag
        if (pos < content.length() && content[pos] == '/') {
            pos++; // Skip '/'
            skip_whitespace(content, pos);
            if (pos >= content.length() || content[pos] != '>') {
                throw std::runtime_error("Expected '>' after '/' in self-closing tag");
            }
            pos++; // Skip '>'
            return node;
        }
        
        if (pos >= content.length() || content[pos] != '>') {
            throw std::runtime_error("Expected '>' after element tag");
        }
        
        pos++; // Skip '>'
        
        // Parse content and child elements
        std::string text_content = parse_text_content(content, pos);
        
        // Parse child elements
        while (pos < content.length()) {
            skip_whitespace(content, pos);
            if (pos >= content.length()) {
                throw std::runtime_error("Unexpected end of input");
            }
            if (content[pos] == '<') {
                pos++; // Skip '<'
                if (pos >= content.length()) {
                    throw std::runtime_error("Unexpected end of input");
                }
                if (content[pos] == '/') {
                    // Closing tag
                    pos++; // Skip '/'
                    skip_whitespace(content, pos);
                    // Find closing tag name
                    size_t tag_end = content.find('>', pos);
                    if (tag_end == std::string::npos) {
                        throw std::runtime_error("Unterminated closing tag");
                    }
                    std::string closing_name = trim(content.substr(pos, tag_end - pos));
                    if (closing_name != node.name) {
                        throw std::runtime_error("Mismatched closing tag: expected '" + node.name + "', got '" + closing_name + "'");
                    }
                    pos = tag_end + 1; // Skip '>'
                    break;
                } else {
                    // Child element
                    pos--; // Go back to '<'
                    XMLNode child = parse_node(content, pos, &node);
                    node.add_child(child);
                }
            } else {
                // More text content
                std::string more_text = parse_text_content(content, pos);
                if (!more_text.empty()) {
                    text_content += more_text;
                }
            }
        }
        // Assign value only if node has no children
        if (node.children.empty()) {
            node.value = text_content;
        }
        return node;
    }

    bool XMLParser::parse_element_tag(const std::string& content, size_t& pos, XMLNode& node) {
        // Parse element name
        size_t name_start = pos;
        while (pos < content.length() && !std::isspace(content[pos]) && content[pos] != '>' && content[pos] != '/') {
            pos++;
        }
        
        if (pos == name_start) {
            return false;
        }
        
        node.name = content.substr(name_start, pos - name_start);
        
        skip_whitespace(content, pos);
        
        // Parse attributes
        parse_attributes(content, pos, node);
        
        return true;
    }

    void XMLParser::parse_attributes(const std::string& content, size_t& pos, XMLNode& node) {
        while (pos < content.length() && content[pos] != '>' && content[pos] != '/') {
            skip_whitespace(content, pos);
            
            if (pos >= content.length() || content[pos] == '>' || content[pos] == '/') {
                break;
            }
            
            // Parse attribute name
            size_t name_start = pos;
            while (pos < content.length() && !std::isspace(content[pos]) && content[pos] != '=' && content[pos] != '>' && content[pos] != '/') {
                pos++;
            }
            
            if (pos == name_start) {
                throw std::runtime_error("Invalid attribute name");
            }
            
            std::string attr_name = content.substr(name_start, pos - name_start);
            
            skip_whitespace(content, pos);
            
            if (pos >= content.length() || content[pos] != '=') {
                throw std::runtime_error("Expected '=' after attribute name");
            }
            
            pos++; // Skip '='
            skip_whitespace(content, pos);
            
            if (pos >= content.length()) {
                throw std::runtime_error("Unexpected end of input in attribute");
            }
            
            std::string attr_value = parse_attribute_value(content, pos);
            node.set_attribute(attr_name, attr_value);
        }
    }

    std::string XMLParser::parse_attribute_value(const std::string& content, size_t& pos) {
        if (pos >= content.length()) {
            throw std::runtime_error("Unexpected end of input in attribute value");
        }
        
        char quote = content[pos];
        if (quote != '"' && quote != '\'') {
            throw std::runtime_error("Expected quote in attribute value");
        }
        
        pos++; // Skip opening quote
        size_t value_start = pos;
        
        while (pos < content.length() && content[pos] != quote) {
            pos++;
        }
        
        if (pos >= content.length()) {
            throw std::runtime_error("Unterminated attribute value");
        }
        
        std::string value = content.substr(value_start, pos - value_start);
        pos++; // Skip closing quote
        
        return value;
    }

    std::string XMLParser::parse_text_content(const std::string& content, size_t& pos) {
        size_t start = pos;
        
        while (pos < content.length() && content[pos] != '<') {
            pos++;
        }
        
        std::string text = content.substr(start, pos - start);
        
        // Decode XML entities
        // This is a simple implementation - in a real parser you'd want more comprehensive entity handling
        size_t amp_pos = text.find("&amp;");
        while (amp_pos != std::string::npos) {
            text.replace(amp_pos, 5, "&");
            amp_pos = text.find("&amp;", amp_pos + 1);
        }
        
        size_t lt_pos = text.find("&lt;");
        while (lt_pos != std::string::npos) {
            text.replace(lt_pos, 4, "<");
            lt_pos = text.find("&lt;", lt_pos + 1);
        }
        
        size_t gt_pos = text.find("&gt;");
        while (gt_pos != std::string::npos) {
            text.replace(gt_pos, 4, ">");
            gt_pos = text.find("&gt;", gt_pos + 1);
        }
        
        size_t quot_pos = text.find("&quot;");
        while (quot_pos != std::string::npos) {
            text.replace(quot_pos, 6, "\"");
            quot_pos = text.find("&quot;", quot_pos + 1);
        }
        
        size_t apos_pos = text.find("&apos;");
        while (apos_pos != std::string::npos) {
            text.replace(apos_pos, 6, "'");
            apos_pos = text.find("&apos;", apos_pos + 1);
        }
        
        return text;
    }

    void XMLParser::skip_whitespace(const std::string& content, size_t& pos) {
        while (pos < content.length() && std::isspace(content[pos])) {
            pos++;
        }
    }

    void XMLParser::skip_comments(const std::string& content, size_t& pos) {
        if (content.substr(pos, 4) != "<!--") {
            return;
        }
        
        pos += 4; // Skip "<!--"
        
        size_t end_pos = content.find("-->", pos);
        if (end_pos == std::string::npos) {
            throw std::runtime_error("Unterminated comment");
        }
        
        pos = end_pos + 3; // Skip "-->"
    }

    void XMLParser::skip_processing_instructions(const std::string& content, size_t& pos) {
        if (content[pos] != '<' || pos + 1 >= content.length() || content[pos + 1] != '?') {
            return;
        }
        
        pos += 2; // Skip "<?"
        
        size_t end_pos = content.find("?>", pos);
        if (end_pos == std::string::npos) {
            throw std::runtime_error("Unterminated processing instruction");
        }
        
        pos = end_pos + 2; // Skip "?>"
    }

    std::string XMLParser::node_to_string(const XMLNode& node, int indent, bool pretty_print) {
        std::string indent_str = pretty_print ? std::string(indent * 2, ' ') : "";
        std::string newline = pretty_print ? "\n" : "";
        
        std::string result = indent_str + "<" + node.name;
        
        // Add attributes
        for (const auto& attr : node.attributes) {
            result += " " + attr.first + "=\"" + attr.second + "\"";
        }
        
        if (node.children.empty() && node.value.empty()) {
            result += " />";
            return result;
        }
        
        result += ">";
        
        if (!node.value.empty()) {
            // Encode XML entities in text content
            std::string encoded_value = node.value;
            size_t amp_pos = encoded_value.find("&");
            while (amp_pos != std::string::npos) {
                encoded_value.replace(amp_pos, 1, "&amp;");
                amp_pos = encoded_value.find("&", amp_pos + 5);
            }
            
            size_t lt_pos = encoded_value.find("<");
            while (lt_pos != std::string::npos) {
                encoded_value.replace(lt_pos, 1, "&lt;");
                lt_pos = encoded_value.find("<", lt_pos + 4);
            }
            
            size_t gt_pos = encoded_value.find(">");
            while (gt_pos != std::string::npos) {
                encoded_value.replace(gt_pos, 1, "&gt;");
                gt_pos = encoded_value.find(">", gt_pos + 4);
            }
            
            result += encoded_value;
        }
        
        // Add child elements
        for (const auto& child : node.children) {
            result += newline + node_to_string(child, indent + 1, pretty_print);
        }
        
        result += "</" + node.name + ">";
        
        return result;
    }

    const XMLNode* XMLParser::get_node_by_path(const XMLNode& root, const std::string& path) const {
        if (path.empty()) {
            return &root;
        }
        
        std::vector<std::string> components = split_path(path);
        const XMLNode* current = &root;
        
        for (const auto& component : components) {
            current = current->get_child(component);
            if (!current) {
                return nullptr;
            }
        }
        
        return current;
    }

    std::vector<std::string> XMLParser::split_path(const std::string& path) const {
        std::vector<std::string> components;
        std::istringstream path_stream(path);
        std::string component;
        
        while (std::getline(path_stream, component, '.')) {
            if (!component.empty()) {
                components.push_back(component);
            }
        }
        
        return components;
    }

    std::string XMLParser::trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) {
            return "";
        }
        
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }

} // namespace parser 