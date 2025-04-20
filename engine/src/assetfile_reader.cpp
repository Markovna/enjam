#include <enjam/assetfile_reader.h>
#include <enjam/asset.h>

namespace Enjam {

Lexer::token_type Lexer::scan() {
  clear();
  skipWhitespace(input);

  if(input.eof()) { return type = token_type::end_of_input; }

  auto next = input.peek();
  switch (next) {
    case '{': { input.get(); return type = token_type::begin_object; }
    case '}': { input.get(); return type = token_type::end_object; }
    case '[': { input.get(); return type = token_type::begin_array; }
    case ']': { input.get(); return type = token_type::end_array; }
    case ':': { input.get(); return type = token_type::name_separator; }
    case ',': { input.get(); return type = token_type::value_separator; }
    case -1 : { input.get(); return type = token_type::end_of_input; }
    case '\"': { input.get(); return type = scanStr(true); }
    default: break;
  }

  if(std::isalpha(next) || next == '_') {
    return type = scanStr(false);
  }

  return type = scanNum();
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

  if(isHex) {
    numValue = (int64_t) std::stoul(buffer, nullptr, 16);
    return token_type::hex_int_value;
  }

  numValue = std::stol(buffer, nullptr, 10);
  return token_type::int_value;
}

void Lexer::clear() {
  type = token_type::parse_error;
  buffer.clear();
  numValue = { };
}

}