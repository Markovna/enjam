#pragma once

#include <functional>
#include <future>
#include <mutex>
#include <list>

namespace Enjam {

template<typename... Args>
class EventHandler final {
 public:
  typedef std::function<void(Args...)> HandlerFunc;
  typedef uint32_t HandlerId;

  explicit EventHandler(const HandlerFunc& handlerFunc)
    : handlerFunc(handlerFunc) {
    handlerId = ++handlerIdCounter;
  }

  EventHandler(const EventHandler& src)
    : handlerFunc(src.handlerFunc)
    , handlerId(src.handlerId) {
  }

  EventHandler(EventHandler&& src) noexcept
    : handlerFunc(std::move(src.handlerFunc))
    , handlerId(src.handlerId) {
  }

  EventHandler& operator=(const EventHandler& src) {
    handlerFunc = src.handlerFunc;
    handlerId = src.handlerId;

    return *this;
  }

  EventHandler& operator=(EventHandler&& src) noexcept {
    std::swap(handlerFunc, src.handlerFunc);
    handlerId = src.handlerId;

    return *this;
  }

  void operator()(Args... params) const {
    if(handlerFunc) {
      handlerFunc(params...);
    }
  }

  bool operator==(const EventHandler& other) const {
    return handlerId == other.handlerId;
  }

  operator bool() const {
    return handlerFunc;
  }

  [[nodiscard]] HandlerId id() const
  {
    return handlerId;
  }

 private:
  HandlerFunc handlerFunc;
  HandlerId handlerId;
  static std::atomic_uint handlerIdCounter;
};

template <typename... Args>
std::atomic_uint EventHandler<Args...>::handlerIdCounter(0);

template<typename... Args>
class Event final {
 public:
  typedef EventHandler<Args...> EventHandler;

  Event() = default;

  Event(const Event& src) {
    std::lock_guard<std::mutex> lock(src.handlersLocker);

    handlers = src.handlers;
  }

  Event(Event&& src) noexcept {
    std::lock_guard<std::mutex> lock(src.handlersLocker);

    handlers = std::move(src.handlers);
  }

  Event& operator=(const Event& src) {
    std::lock_guard<std::mutex> lock(handlersLocker);
    std::lock_guard<std::mutex> lock2(src.handlersLocker);

    handlers = src.handlers;

    return *this;
  }

  Event& operator=(Event&& src) noexcept {
    std::lock_guard<std::mutex> lock(handlersLocker);
    std::lock_guard<std::mutex> lock2(src.handlersLocker);

    std::swap(handlers, src.handlers);

    return *this;
  }

  typename EventHandler::HandlerId add(const EventHandler& handler) {
    std::lock_guard<std::mutex> lock(handlersLocker);

    handlers.push_back(handler);

    return handler.id();
  }

  inline typename EventHandler::HandlerId add(const typename EventHandler::HandlerFunc& handler) {
    return add(EventHandler(handler));
  }

  bool remove(const EventHandler& handler) {
    std::lock_guard<std::mutex> lock(handlersLocker);

    auto it = std::find(handlers.begin(), handlers.end(), handler);
    if(it != handlers.end()) {
      handlers.erase(it);
      return true;
    }

    return false;
  }

  bool removeById(const typename EventHandler::HandlerId& handlerId) {
    std::lock_guard<std::mutex> lock(handlersLocker);

    auto it = std::find_if(handlers.begin(), handlers.end(),
                           [handlerId](const EventHandler& handler) { return handler.id() == handlerId; });
    if(it != handlers.end()) {
      handlers.erase(it);
      return true;
    }

    return false;
  }

  void call(Args... params) const {
    HandlerCollection handlersCopy = getHandlersCopy();
    call(handlersCopy, params...);
  }

  std::future<void> callAsync(Args... params) const {
    return std::async(std::launch::async, [this](Args... asyncParams) { call(asyncParams...); }, params...);
  }

  inline void operator()(Args... params) const {
    call(params...);
  }

  inline typename EventHandler::HandlerId operator+=(const EventHandler& handler){
    return add(handler);
  }

  inline typename EventHandler::HandlerId operator+=(const typename EventHandler::HandlerFunc& handler){
    return add(handler);
  }

  inline bool operator-=(const EventHandler& handler) {
    return remove(handler);
  }

 private:
  typedef std::list<EventHandler> HandlerCollection;

  static void call(const HandlerCollection& handlers, Args... params) {
    for(const auto& handler : handlers) {
      handler(params...);
    }
  }

  HandlerCollection getHandlersCopy() const {
    std::lock_guard<std::mutex> lock(handlersLocker);
    return handlers;
  }

 private:
  HandlerCollection handlers;
  mutable std::mutex handlersLocker;
};

}