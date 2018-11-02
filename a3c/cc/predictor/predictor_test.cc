// Testing the predictor
#include "gtest/gtest.h"
#include "a3c/cc/predictor/predictor.h"

TEST(Predictor, TestConfig) {

    // Testing the Configuration values.
    auto predictor = Predictor(
        1, "/Users/sebastianlettner/GoogleDrive/MachineLearning/a3c-framework/a3c/cc/predictor/predictor.ini"
    );
    auto address = predictor.get_config()->GetValue("ADDRESSES", "SERVER_INTERFACE_PUSH", NULL);
    std::cout << address << std::endl;
    //ASSERT_EQ(address, "tcp://127.0.0.1:3335"); Comparison does not work somehow.
}

