#ifndef INCLUDE_ENJAM_ASSETFILE_PARSER_H_
#define INCLUDE_ENJAM_ASSETFILE_PARSER_H_

#include <string>
#include <istream>
#include <cmath>
#include <enjam/asset.h>

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
    hex_int_value,
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
  explicit Lexer(std::istream& input) : input(input), numValue(), buffer() {}

  token_type scan();
  inline token_type token() { return type; }

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

  token_type type;
  std::istream& input;
  numeric_value_t numValue;
  std::string buffer;
};

template<class TInput>
class AssetFileReader {
 private:
  using token_type = Lexer::token_type;

 public:
  AssetFileReader(TInput& input)
    : lexer(input.fileStream()), input(input) {

  }

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
  TInput& input;
};

template<class TInput>
Lexer::token_type AssetFileReader<TInput>::parseObject(Asset& object) {
  std::hash<std::string> hash;
  while(true) {
    if(lexer.scan() != token_type::string_value) {
      return token_type::parse_error;
    }

    auto& property = object[lexer.getStr()];

    if(lexer.scan() != token_type::name_separator) { return token_type::parse_error; }

    if(!parseProperty(property)) { return lexer.token(); }

    auto type = lexer.scan();
    if(type == token_type::value_separator) {
      continue;
    }

    return type;
  }
}

template<class TInput>
bool AssetFileReader<TInput>::parseProperty(Asset& value) {
  auto type = lexer.scan();
  switch(type) {
    case token_type::begin_object: {
      if(Asset object = Asset::object(); parseObject(object) == token_type::end_object) {
        value = std::move(object);
        return true;
      }
      return false;
    }
    case token_type::begin_array: {
      if(Asset object = Asset::array(); parseArray(object) == token_type::end_array) {
        value = std::move(object);
        return true;
      }
      return false;
    }
    case token_type::float_value: {
      value = lexer.getFloat();
      return true;
    }
    case token_type::int_value: {
      value = lexer.getInt();
      return true;
    }
    case token_type::hex_int_value: {
      auto hash = lexer.getInt();
      value = input.getBufferLoader(hash);
      return true;
    }
    case token_type::string_value: {
      value = std::string { lexer.getStr() };
      return true;
    }
    case token_type::name_separator:
    case token_type::value_separator:
    case token_type::end_array:
    case token_type::end_object:
    case token_type::end_of_input:
    case token_type::parse_error: {
      return false;
    }
  }
}

template<class TInput>
Lexer::token_type AssetFileReader<TInput>::parseArray(Asset& asset) {
  while(true) {
    Asset property;
    if(!parseProperty(property)) {
      return lexer.token();
    }
    asset.pushBack(std::move(property));

    auto type = lexer.scan();
    if(type == token_type::value_separator) {
      continue;
    }

    return type;
  }
}

template<class TInput>
Asset AssetFileReader<TInput>::parse() {
  Asset asset;
  if(parseObject(asset) != token_type::end_of_input) {
    asset.clear();
  }
  return asset;
}

template<class TInput>
bool AssetFileReader<TInput>::parse(Asset& asset) {
  if(parseObject(asset) != token_type::end_of_input) {
    asset.clear();
    return false;
  }
  return true;
}

template<class TInput>
Asset AssetFileReader<TInput>::operator()() {
  return parse();
}

template<class TInput>
bool AssetFileReader<TInput>::operator()(Asset& asset) {
  return parse(asset);
}

}

#endif //INCLUDE_ENJAM_ASSETFILE_PARSER_H_
