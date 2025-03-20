#ifndef NJCTR_TESTS_TEST_H_
#define NJCTR_TESTS_TEST_H_

struct Test {
  template <class TFunc>
  Test(const TFunc& func) {
    func();
  }
};


#endif //NJCTR_TESTS_TEST_H_
