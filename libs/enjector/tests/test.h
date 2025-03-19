#ifndef ENJECTOR_TESTS_TEST_H_
#define ENJECTOR_TESTS_TEST_H_

struct Test {
  template <class TFunc>
  Test(const TFunc& func) {
    func();
  }
};


#endif //ENJECTOR_TESTS_TEST_H_
