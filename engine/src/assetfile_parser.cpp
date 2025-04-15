#include <enjam/assetfile_parser.h>
#include <enjam/asset.h>

namespace Enjam {

Lexer::token_type Lexer::scan() {
  clear();
  skipWhitespace(input);

  if(input.eof()) { return token_type::end_of_input; }

  auto next = input.peek();
  switch (next) {
    case '{': { input.get(); return token_type::begin_object; }
    case '}': { input.get(); return token_type::end_object; }
    case '[': { input.get(); return token_type::begin_array; }
    case ']': { input.get(); return token_type::end_array; }
    case ':': { input.get(); return token_type::name_separator; }
    case ',': { input.get(); return token_type::value_separator; }
    case -1 : { input.get(); return token_type::end_of_input; }
    case '\"': { input.get(); return scanStr(true); }
    default: break;
  }

  if(std::isalpha(next) || next == '_') {
    return scanStr(false);
  }

  return scanNum();
}

Lexer::token_type Lexer::scanStr(bool quoted) {
  if(input.eof()) { return token_type::parse_error; }

  while(!input.eof()) {
    auto next = input.peek();
    if(next == '\"') {
      if(quoted) {
        input.get();
      }

      return token_type::string_value;
    }

    if(!quoted) {
      if(!std::isalnum(next) && next != '_') {
        return token_type::string_value;
      }
    }

    buffer.push_back(next);
    input.get();
  }

  return token_type::string_value;
}

void Lexer::skipWhitespace(std::istream &input) {
  while(!input.eof()) {
    auto next = input.peek();
    if(next != ' ' && next != '\t' && next != '\n' && next != '\r') {
      return;
    }
    input.get();
  }
}

Lexer::token_type Lexer::scanNum() {
  if(input.eof()) { return token_type::parse_error; }
  if(input.peek() == '-') {
    buffer.push_back(input.get());
  }

  int floatingPoints = 0;
  bool isHex = false;
  while(!input.eof()) {
    auto next = input.peek();

    if(next == '.') {
      if(floatingPoints) {
        return token_type::parse_error;
      }
      floatingPoints++;
      buffer.push_back(input.get());
      continue;
    }

    if(std::isdigit(next)) {
      buffer.push_back(input.get());
      continue;
    }

    if(next == 'x' || next == 'X' ||
        next == 'A' || next == 'a' ||
        next == 'B' || next == 'b' ||
        next == 'C' || next == 'c' ||
        next == 'D' || next == 'd' ||
        next == 'E' || next == 'e' ||
        next == 'F' || next == 'f') {
      isHex |= true;
      buffer.push_back(input.get());
      continue;
    }

    break;
  }

  if(buffer.empty()) {
    return token_type::parse_error;
  }

  if(floatingPoints) {
    numValue = std::stof(buffer);
    return token_type::float_value;
  }

  numValue = std::stol(buffer, nullptr, isHex ? 16 : 10);
  return token_type::int_value;
}

void Lexer::clear() {
  buffer.clear();
  numValue = { };
}

AssetFileParser::token_type AssetFileParser::parseObject(Asset& object) {
  std::hash<std::string> hash;
  while(true) {
    if(lexer.scan() != token_type::string_value) { return token_type::parse_error; }
    auto& property = object[lexer.getStr()];

    if(lexer.scan() != token_type::name_separator) { return token_type::parse_error; }

    if(!parseProperty(property)) { return token_type::parse_error; }

    auto type = lexer.scan();
    if(type == token_type::value_separator) {
      continue;
    }

    return type;
  }
}

bool AssetFileParser::parseProperty(Asset& value) {
  while(true) {
    auto type = lexer.scan();
    switch(type) {
      case token_type::begin_object: {
        if(Asset object; parseObject(object) == token_type::end_object) {
          value = std::move(object);
          return true;
        }
        return false;
      }
      case token_type::begin_array: {
        if(Asset object; parseArray(object) == token_type::end_array) {
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
}

AssetFileParser::token_type AssetFileParser::parseArray(Asset& asset) {
  while(true) {
    Asset property;
    if(!parseProperty(property)) { return token_type::parse_error; }
    asset.pushBack(std::move(property));

    auto type = lexer.scan();
    if(type == token_type::value_separator) {
      continue;
    }

    return type;
  }
}

AssetFileParser::AssetFileParser(std::istream& input) : lexer(input) {

}

Asset AssetFileParser::parse() {
  Asset asset;
  if(parseObject(asset) != token_type::end_of_input) {
    asset.clear();
  }
  return asset;
}

bool AssetFileParser::parse(Asset& asset) {
  if(parseObject(asset) != token_type::end_of_input) {
    asset.clear();
    return false;
  }
  return true;
}

Asset AssetFileParser::operator()() {
  return parse();
}

bool AssetFileParser::operator()(Asset& asset) {
  return parse(asset);
}

}