#ifndef ENGINE_INCLUDE_ENJAM_BYTE_ARRAY_H_
#define ENGINE_INCLUDE_ENJAM_BYTE_ARRAY_H_

#include <vector>
#include <string_view>

namespace Enjam {

using ByteArray = std::vector<uint8_t>;

static inline ByteArray makeByteArray(const void* buffer, size_t size) {
  return { static_cast<ByteArray::const_pointer>(buffer),
           static_cast<ByteArray::const_pointer>(buffer) + size };
}

template<class InputIterator,
    std::enable_if_t<std::is_trivially_copyable<typename std::iterator_traits<InputIterator>::value_type>::value, nullptr_t> = nullptr>
ByteArray makeByteArray(InputIterator first, InputIterator last, std::random_access_iterator_tag) {
  using value_type = typename std::iterator_traits<InputIterator>::value_type;

  auto size = last - first;
  ByteArray ret(size * sizeof(value_type));

  uint8_t* dst = ret.data();
  for(auto it = first; it != last; it++) {
    std::memcpy(dst, reinterpret_cast<const uint8_t*>(std::addressof(*it)), sizeof(value_type));
    dst += sizeof(value_type);
  }
  return ret;
}

template<class InputIterator,
    std::enable_if_t<std::is_trivially_copyable<typename std::iterator_traits<InputIterator>::value_type>::value, nullptr_t> = nullptr>
ByteArray makeByteArray(InputIterator first, InputIterator last) {
  using category = typename std::iterator_traits<InputIterator>::iterator_category;
  return makeByteArray(first, last, category{});
}

}

namespace std {

template<>
struct hash<Enjam::ByteArray> {
  size_t operator()(const Enjam::ByteArray& buf) const {
    return hash<string_view> {}({ reinterpret_cast<const char*>(buf.data()), buf.size() });
  }
};

}

#endif //ENGINE_INCLUDE_ENJAM_BYTE_ARRAY_H_
