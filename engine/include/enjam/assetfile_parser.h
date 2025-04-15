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
  explicit Lexer(std::istream& input) : input(input), numValue(), buffer() { }

  token_type scan();

  std::string_view getStr() const { return buffer; }
  float getFloat() const { return std::visit([](auto val) -> float_t { return val; }, numValue); }
  int64_t getInt() const { return std::visit([](auto val) -> int64_t { return val; }, numValue); }

 private:
  static void skipWhitespace(std::istream& input);

  token_type scanStr(bool quoted);
  token_type scanNum();
  void clear();

 private:
  using numeric_value_t = std::variant<float_t, int64_t>;

  std::istream& input;
  numeric_value_t numValue;
  std::string buffer;
};

class AssetFileParser {
 private:
  using token_type = Lexer::token_type;

 public:
  explicit AssetFileParser(std::istream& input);

  Asset parse();
  bool parse(Asset&);

  Asset operator()();
  bool operator()(Asset&);

 private:
  token_type parseObject(Asset&);
  token_type parseArray(Asset&);
  bool parseProperty(Asset&);

 private:
  Lexer lexer;
};

}

#endif //INCLUDE_ENJAM_ASSETFILE_PARSER_H_
