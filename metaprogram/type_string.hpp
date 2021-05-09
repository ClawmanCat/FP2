#pragma once

#include <metaprogram/common.hpp>
#include <metaprogram/functional.hpp>


namespace meta {
    namespace detail {
        inline std::string parse_string_arg(std::string_view source) {
            std::vector<std::string> number_strings;
            std::string current_number_string;
            
            for (char ch : source) {
                if (std::isdigit(ch)) {
                    current_number_string += ch;
                } else {
                    if (!current_number_string.empty()) {
                        number_strings.push_back(std::move(current_number_string));
                        current_number_string.clear();
                    }
                }
            }
            
            return number_strings
                | views::transform(fn_object(std::stoull))
                | views::transform(fn_object(cast<char>))
                | views::drop_last(1) // Last char will be null terminator.
                | ranges::to<std::string>;
        }
    }
    
    
    template <typename T> inline std::string type_string(void) {
        // __PRETTY_FUNCTION__ returns the name of the function, including its template arguments.
        // We can use this to get a string of that argument by removing the rest of the function name.
        // This macro is a compiler intrinsic, and this function currently only works with Clang.
        // - For GCC, you need only adjust the prefix and suffix length.
        // - For MSVC, you can use __FUNCSIG__ instead.
        std::string_view sv = __PRETTY_FUNCTION__;
        sv.remove_prefix(37);
        sv.remove_suffix(1);
        
        return std::string { sv };
    }
    
    
    // Equivalent to above, but splits the string across multiple lines, adding indentation,
    // and replaces string arguments with their ASCII representation.
    template <typename T> inline std::string pretty_type_string(std::size_t max_indent = 32, std::size_t indent_size = 3) {
        std::string unformatted = type_string<T>();
        
        // Replace string arguments of the form {{X, X, X, X, X, ...}} where 'X' is some number
        // representing an ASCII character, with the ASCII characters encoded within.
        constexpr static ctll::fixed_string rgx = R"RGX(\{\{[\d\s,]+\}\})RGX";
        
        while (auto m = ctre::search<rgx>(unformatted)) {
            auto substring   = m.template get<0>();
            auto replacement = detail::parse_string_arg(substring.to_view());
            
            unformatted.replace(
                substring.begin(),
                substring.end(),
                replacement.begin(),
                replacement.end()
            );
        }
        
        // Split the string into lines and add indentation.
        // - When an opening angle bracket is encountered, indentation is increased and the line is ended.
        // - When a closing angle bracket is encountered, indentation is decreased and the line is ended.
        // - When a comma is encountered, the line is ended.
        // - Any content having an indentation level greater than max_indent is replaced with ellipses (...).
        std::size_t indent_level = 0;
        std::size_t nested_braces = 0;
        std::string result;
        
        auto print = [&](auto&&... chars) {
            if (indent_level < max_indent) {
                ([&](auto&& ch) { result += ch; }(fwd(chars)), ...);
            }
        };
        
        
        for (std::size_t i = 0; i < unformatted.length(); ++i) {
            char ch = unformatted[i];
            
            if (ch == '{') ++nested_braces;
            if (ch == '}') --nested_braces;
            
            if (ch == '<') {
                if (++indent_level == max_indent) {
                    result += "< ... >";
                } else {
                    print(ch, '\n');
                    print(std::string(indent_level * indent_size, ' '));
                }
            }
            
            else if (ch == '>') {
                if (indent_level-- < max_indent) {
                    print('\n');
                    print(std::string(indent_level * indent_size, ' '), ch);
                }
            }
            
            else if (ch == ',') {
                print(ch);
    
                if (!nested_braces) {
                    print('\n', std::string(indent_level * indent_size, ' '));
                }
                
                // Next character will be a space that will ruin indentation, skip it.
                ++i;
            }
            
            else print(ch);
        }
        
        
        return result;
    }
}