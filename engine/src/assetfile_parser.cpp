#include <enjam/assetfile_parser.h>
#include <enjam/asset.h>

namespace Enjam {

Lexer::token_type Lexer::scan() {
  clear();
  skipWhitespace(input);

  if(input.eof()) { return token_type::end_of_input; }

  auto next = input.peek();
  switch (next) {
    case '{': { input.get(); return type = token_type::begin_object; }
    case '}': { input.get(); return type = token_type::end_object; }
    case '[': { input.get(); return type = token_type::begin_array; }
    case ']': { input.get(); return type = token_type::end_array; }
    case ':': { input.get(); return type = token_type::name_separator; }
    case ',': { input.get(); return type = token_type::value_separator; }
    case -1 : { input.get(); return type = token_type::end_of_input; }

    case '\"': { return type = scanStr(); }
    case '#': { return type = scanHash(); }
    default: break;
  }

  return scanNum();
}

Lexer::token_type Lexer::scanKey() {
  clear();
  skipWhitespace(input);

  while(!input.eof()) {
    auto current = input.peek();
    if(current == ' ' || current == ':') {
      return type = token_type::string_value;
    }

    if(!std::isalnum(current) && current != '_') {
      return type = token_type::parse_error;
    }

    buffer.push_back(input.get());
  }

  return type = token_type::parse_error;
}

Lexer::token_type Lexer::scanHash() {
  if(input.eof()) { return token_type::parse_error; }
  if(input.peek() != '#') { return token_type::parse_error; }

  input.get();

  while(!input.eof() && std::isalnum(input.peek())) {
    buffer.push_back(input.get());
  }

  return token_type::hashcode_value;
}

Lexer::token_type Lexer::scanStr() {
  if(input.eof()) { return token_type::parse_error; }
  if(input.peek() != '\"') { return token_type::parse_error; }

  input.get();

  while(!input.eof()) {
    auto current = input.get();
    if(current == '\"') { return token_type::string_value; }

    buffer.push_back(current);
  }

  return token_type::parse_error;
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
  while(!input.eof()) {
    auto next = input.peek();
    if(std::isdigit(next)) {
      buffer.push_back(input.get());
      continue;
    }

    if(next == '.') {
      if(floatingPoints) {
        return token_type::parse_error;
      }
      floatingPoints++;
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

  numValue = std::stol(buffer);
  return token_type::int_value;
}

void Lexer::clear() {
  buffer.clear();
  numValue = { };
  type = token_type::uninitialized;
}

bool AssetFileParser::parse(std::istream &input, Asset &asset) {
  Lexer lexer { input };
  return parseObject(lexer, asset) == token_type::end_of_input;
}

Asset AssetFileParser::parse(std::istream& input) {
  Lexer lexer { input };
  Asset asset;
  if(parseObject(lexer, asset) != token_type::end_of_input) {
    asset.clear();
  }
  return asset;
}

AssetFileParser::token_type AssetFileParser::parseObject(Lexer &lexer, Asset &object) {
  std::hash<std::string> hash;
  while(true) {
    if(lexer.scanKey() != token_type::string_value) { return token_type::parse_error; }
    auto key = lexer.getStr();

    if(lexer.scan() != token_type::name_separator) { return token_type::parse_error; }

    Property property;
    if(!parseProperty(lexer, property)) { return token_type::parse_error; }

    property.nameHash = hash(key);

    object.add(std::move(property));

    auto type = lexer.scan();
    if(type == token_type::value_separator) {
      continue;
    }

    return type;
  }
}

bool AssetFileParser::parseProperty(Lexer &lexer, Property& property) {
  while(true) {
    auto type = lexer.scan();
    switch(type) {
      case token_type::begin_object: {
        if(Asset object; parseObject(lexer, object) == token_type::end_object) {
          property.value = std::move(object);
          return true;
        }
        return false;
      }
      case token_type::float_value: {
        property.value = lexer.getFloat();
        return true;
      }
      case token_type::int_value: {
        property.value = lexer.getInt();
        return true;
      }
      case token_type::string_value: {
        property.value = lexer.getStr();
        return true;
      }
      case token_type::hashcode_value: {
        property.value = BufferHash { .value = lexer.getStr() };
        return true;
      }
      case token_type::uninitialized:
      case token_type::name_separator:
      case token_type::value_separator:
      case token_type::begin_array:
      case token_type::end_array:
      case token_type::end_object:
      case token_type::end_of_input:
      case token_type::parse_error: {
        return false;
      }
    }
  }
}
}