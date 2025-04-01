#ifndef INCLUDE_ENJAM_ASSETFILE_PARSER_H_
#define INCLUDE_ENJAM_ASSETFILE_PARSER_H_

#include <string>
#include <istream>
#include <cmath>

namespace Enjam {

class Asset;
class Property;

class Lexer {
 public:
  enum class token_type {
    uninitialized,
    begin_object,
    end_object,
    begin_array,
    end_array,
    float_value,
    int_value,
    string_value,
    name_separator,
    value_separator,
    end_of_input,
    parse_error
  };

 private:
  using char_traits = std::char_traits<char>;

 public:
  explicit Lexer(std::istream& input) : input(input), numValue(), type(token_type::uninitialized), buffer() { }

  token_type scan();

  token_type scanKey();

  token_type getType() const { return type; }

  std::string getStr() const { return buffer; }
  float getFloat() const { return std::visit([](auto val) -> float_t { return val; }, numValue); }
  int64_t getInt() const { return std::visit([](auto val) -> int64_t { return val; }, numValue); }

 private:
  token_type scanStr();
  token_type scanNum();
  void clear();

  static void skipWhitespace(std::istream& input);

 private:
  using numeric_value_t = std::variant<float_t, int64_t>;

  std::istream& input;
  token_type type;
  numeric_value_t numValue;
  std::string buffer;
};

class AssetFileParser {
 private:
  using token_type = Lexer::token_type;

 public:
  static bool parse(std::istream& input, Asset& asset);
  static Asset parse(std::istream& input);

 private:
  static token_type parseObject(Lexer&, Asset&);
  static token_type parseArray(Lexer&, Asset&);
  static bool parseProperty(Lexer&, Asset&);
};

}

#endif //INCLUDE_ENJAM_ASSETFILE_PARSER_H_
