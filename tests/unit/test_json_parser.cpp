#include <iostream>
#include <cassert>
#include "../src/json_parser.hpp"

void test_json_parser_extract_string() {
    std::string json = R"({"key":"value","nested":{"inner":"data"}})";
    
    std::string val = json_parser::extract_string(json, "key");
    assert(val == "value");
    
    std::string nested = json_parser::extract_string(json, "nested");
    assert(nested.find("inner") != std::string::npos);
    
    std::string missing = json_parser::extract_string(json, "missing");
    assert(missing.empty());
}

void test_json_parser_extract_bool() {
    std::string json = R"({"enabled":true,"disabled":false,"missing":null})";
    
    assert(json_parser::extract_bool(json, "enabled", false) == true);
    assert(json_parser::extract_bool(json, "disabled", true) == false);
    assert(json_parser::extract_bool(json, "missing", true) == true);
    assert(json_parser::extract_bool(json, "nonexistent", false) == false);
}

void test_json_parser_extract_int() {
    std::string json = R"({"count":42,"negative":-10,"zero":0})";
    
    assert(json_parser::extract_int(json, "count", 0) == 42);
    assert(json_parser::extract_int(json, "negative", 0) == -10);
    assert(json_parser::extract_int(json, "zero", -1) == 0);
    assert(json_parser::extract_int(json, "missing", 100) == 100);
}

void test_json_parser_extract_string_array() {
    std::string json = R"({"items":["one","two","three"],"empty":[],"missing":null})";
    
    auto items = json_parser::extract_string_array(json, "items");
    assert(items.size() == 3);
    assert(items[0] == "one");
    assert(items[1] == "two");
    assert(items[2] == "three");
    
    auto empty = json_parser::extract_string_array(json, "empty");
    assert(empty.empty());
    
    auto missing = json_parser::extract_string_array(json, "missing");
    assert(missing.empty());
}

void test_json_parser_complex_nested() {
    std::string json = R"({"notifications":{"enabled":true,"sound":false},"connection":{"timeout":30}})";
    
    std::string notifications = json_parser::extract_string(json, "notifications");
    assert(!notifications.empty());
    assert(json_parser::extract_bool(notifications, "enabled", false) == true);
    assert(json_parser::extract_bool(notifications, "sound", true) == false);
    
    std::string connection = json_parser::extract_string(json, "connection");
    assert(!connection.empty());
    assert(json_parser::extract_int(connection, "timeout", 0) == 30);
}

void test_json_parser_validation() {
    // Type mismatch handling
    std::string json = R"({"str_as_int": "123", "int_as_str": 456, "bool_as_str": "true"})";
    
    // extract_int from string "123" should fail (return default) because it expects raw digits
    assert(json_parser::extract_int(json, "str_as_int", 999) == 999);
    
    // extract_string from int 456 should fail (return empty) because it expects quotes/braces/brackets
    assert(json_parser::extract_string(json, "int_as_str") == "");
    
    // extract_bool from string "true" should succeed (it handles quoted strings)
    assert(json_parser::extract_bool(json, "bool_as_str", false) == true);
    
    std::cout << "  ✓ JSON parser type validation test passed" << std::endl;
}

void test_json_parser_malformed() {
    // Unclosed string
    std::string json1 = R"({"key": "value)";
    assert(json_parser::extract_string(json1, "key") == "");
    
    // Unclosed brace
    std::string json2 = R"({"key": { "nested": "val" )";
    assert(json_parser::extract_string(json2, "key") == ""); 
    
    // Missing colon
    std::string json3 = R"({"key" "value"})";
    assert(json_parser::extract_string(json3, "key") == "");
    
    std::cout << "  ✓ JSON parser malformed input test passed" << std::endl;
}

void test_json_parser_whitespace() {
    std::string json = R"({
        "key" :  "value" , 
        "int" : 123 ,
        "bool" : true
    })";
    
    assert(json_parser::extract_string(json, "key") == "value");
    assert(json_parser::extract_int(json, "int", 0) == 123);
    assert(json_parser::extract_bool(json, "bool", false) == true);
    
    std::cout << "  ✓ JSON parser whitespace test passed" << std::endl;
}

void test_json_parser() {
    test_json_parser_extract_string();
    test_json_parser_extract_bool();
    test_json_parser_extract_int();
    test_json_parser_extract_string_array();
    test_json_parser_complex_nested();
    test_json_parser_validation();
    test_json_parser_malformed();
    test_json_parser_whitespace();
    std::cout << "All JSON parser tests passed!" << std::endl;
}
