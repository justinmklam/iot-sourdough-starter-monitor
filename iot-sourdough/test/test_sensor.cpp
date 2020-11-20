#include <unity.h>

void test_nothing(void) {
    TEST_ASSERT_EQUAL(42, 42);
}

void process(){
    UNITY_BEGIN();
    RUN_TEST(test_nothing);
    UNITY_END();
}

#ifdef ARDUINO
#include <Arduino.h>

void setup(void) {
// set stuff up here
    process();
}

void loop() {
}
#else

int main(int argc, char **argv) {
    process();
    return 0;
}

#endif
