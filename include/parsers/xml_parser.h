#pragma once

#include <string>
#include <map>
#include <vector>

namespace parser {

    /**
     * @brief XML node structure
     */
    struct XMLNode {
        std::string name;
        std::string value;
        std::map<std::string, std::string> attributes;
        std::vector<XMLNode> children;
        XMLNode* parent = nullptr;
        
        /**
         * @brief Get child node by name
         * @param child_name The name of the child node
         * @return Pointer to child node or nullptr if not found
         */
        XMLNode* get_child(const std::string& child_name);
        
        /**
         * @brief Get child node by name (const version)
         * @param child_name The name of the child node
         * @return Pointer to child node or nullptr if not found
         */
        const XMLNode* get_child(const std::string& child_name) const;
        
        /**
         * @brief Get attribute value
         * @param attr_name The attribute name
         * @param default_value Default value if attribute not found
         * @return The attribute value
         */
        std::string get_attribute(const std::string& attr_name, const std::string& default_value = "") const;
        
        /**
         * @brief Check if node has attribute
         * @param attr_name The attribute name
         * @return True if attribute exists
         */
        bool has_attribute(const std::string& attr_name) const;
        
        /**
         * @brief Get all child nodes with specific name
         * @param child_name The name of child nodes to find
         * @return Vector of child nodes
         */
        std::vector<XMLNode*> get_children(const std::string& child_name);
        
        /**
         * @brief Get all child nodes with specific name (const version)
         * @param child_name The name of child nodes to find
         * @return Vector of child nodes
         */
        std::vector<const XMLNode*> get_children(const std::string& child_name) const;
        
        /**
         * @brief Add child node
         * @param child The child node to add
         */
        void add_child(const XMLNode& child);
        
        /**
         * @brief Set attribute
         * @param name The attribute name
         * @param value The attribute value
         */
        void set_attribute(const std::string& name, const std::string& value);
    };

    /**
     * @brief Result structure for XML parsing operations
     */
    struct XMLResult {
        bool success = false;
        std::string error_message;
        XMLNode root;
        
        /**
         * @brief Get value by path (e.g., "config.database.host")
         * @param path The path to the value
         * @param default_value Default value if not found
         * @return The value as string
         */
        std::string get_value(const std::string& path, const std::string& default_value = "") const;
        
        /**
         * @brief Get attribute value by path
         * @param path The path to the node
         * @param attr_name The attribute name
         * @param default_value Default value if not found
         * @return The attribute value
         */
        std::string get_attribute(const std::string& path, const std::string& attr_name, const std::string& default_value = "") const;
        
        /**
         * @brief Get node by path
         * @param path The path to the node
         * @return Pointer to the node or nullptr if not found
         */
        const XMLNode* get_node(const std::string& path) const;
        
        /**
         * @brief Check if path exists
         * @param path The path to check
         * @return True if path exists
         */
        bool has_path(const std::string& path) const;
        
        /**
         * @brief Get all child names at a specific path
         * @param path The path to get children from
         * @return Vector of child names
         */
        std::vector<std::string> get_children(const std::string& path = "") const;
        
        /**
         * @brief Get all attribute names at a specific path
         * @param path The path to get attributes from
         * @return Vector of attribute names
         */
        std::vector<std::string> get_attributes(const std::string& path) const;
    };

    /**
     * @brief XML file parser class
     * 
     * A lightweight and efficient XML parser that supports:
     * - Element parsing with attributes
     * - Nested structures
     * - Path-based access (e.g., "config.database.host")
     * - Attribute access
     * - Text content extraction
     */
    class XMLParser {
    public:
        /**
         * @brief Parse XML content from string
         * @param content The XML content as string
         * @return XMLResult with parsed data or error information
         */
        XMLResult parse(const std::string& content);
        
        /**
         * @brief Parse XML content from file
         * @param filename The path to the XML file
         * @return XMLResult with parsed data or error information
         */
        XMLResult parse_file(const std::string& filename);
        
        /**
         * @brief Convert parsed data back to XML format
         * @param result The parsed XML result
         * @param pretty_print Whether to format with indentation
         * @return XML content as string
         */
        std::string to_string(const XMLResult& result, bool pretty_print = false);
        
        /**
         * @brief Save parsed data to file
         * @param result The parsed XML result
         * @param filename The output file path
         * @param pretty_print Whether to format with indentation
         * @return True if successful
         */
        bool save_to_file(const XMLResult& result, const std::string& filename, bool pretty_print = false);

        /**
         * @brief Get XML node by path
         * @param root The root XML node
         * @param path The path to traverse
         * @return The XML node at the path
         */
        const XMLNode* get_node_by_path(const XMLNode& root, const std::string& path) const;

    private:
        /**
         * @brief Parse XML node from string
         * @param content The XML content
         * @param pos Current position in the content
         * @param parent Parent node
         * @return Parsed XML node
         */
        XMLNode parse_node(const std::string& content, size_t& pos, XMLNode* parent = nullptr);
        
        /**
         * @brief Parse XML element tag
         * @param content The XML content
         * @param pos Current position in the content
         * @param node The node to populate
         * @return True if parsing was successful
         */
        bool parse_element_tag(const std::string& content, size_t& pos, XMLNode& node);
        
        /**
         * @brief Parse XML attributes
         * @param content The XML content
         * @param pos Current position in the content
         * @param node The node to populate
         */
        void parse_attributes(const std::string& content, size_t& pos, XMLNode& node);
        
        /**
         * @brief Parse XML attribute value
         * @param content The XML content
         * @param pos Current position in the content
         * @return The attribute value
         */
        std::string parse_attribute_value(const std::string& content, size_t& pos);
        
        /**
         * @brief Parse XML text content
         * @param content The XML content
         * @param pos Current position in the content
         * @return The text content
         */
        std::string parse_text_content(const std::string& content, size_t& pos);
        
        /**
         * @brief Skip whitespace characters
         * @param content The XML content
         * @param pos Current position in the content
         */
        void skip_whitespace(const std::string& content, size_t& pos);
        
        /**
         * @brief Skip XML comments
         * @param content The XML content
         * @param pos Current position in the content
         */
        void skip_comments(const std::string& content, size_t& pos);
        
        /**
         * @brief Skip XML processing instructions
         * @param content The XML content
         * @param pos Current position in the content
         */
        void skip_processing_instructions(const std::string& content, size_t& pos);
        
        /**
         * @brief Convert XML node to string representation
         * @param node The XML node to convert
         * @param indent Current indentation level
         * @param pretty_print Whether to format with indentation
         * @return String representation
         */
        std::string node_to_string(const XMLNode& node, int indent = 0, bool pretty_print = false);
        
        /**
         * @brief Split path into components
         * @param path The path to split
         * @return Vector of path components
         */
        std::vector<std::string> split_path(const std::string& path) const;
        
        /**
         * @brief Trim whitespace from string
         * @param str The string to trim
         * @return Trimmed string
         */
        std::string trim(const std::string& str);
    };

} // namespace parser 