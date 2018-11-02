
#include "gtest/gtest.h"
#include "a3c/cc/serialization/serializer.h"
#include "a3c/cc/serialization/test_generated.h"

TEST(SerializerTest, TestInit) {

    // Test the creation of a serializer.
    Serializer<serialization_test::StructureBuilder> serializer;

}

TEST(SerializerTest, TestExample) {

    // Test an example serialization
    Serializer<serialization_test::StructureBuilder> serializer;
    serializer.get_builder()->add_x(1);
    serializer.get_builder()->add_y(2.0);
    std::vector<float> vec;
    vec.push_back(1.0);
    vec.push_back(2.0);
    vec.push_back(3.0);
    auto s_vec = serializer.get_buffer()->CreateVector(vec);
    serializer.get_builder()->add_z(s_vec);

    uint8_t * buf = serializer.serialize();
    
    // Make sure the buffer has been cleaned after serialization.
    ASSERT_EQ(serializer.get_buffer()->GetSize(), 0);
    // sending over network..
    auto structure = serialization_test::GetStructure(buf);
    
    // check the values 
    ASSERT_EQ(structure->x(), 1);
    ASSERT_EQ(structure->y(), 2.0);
    std::vector<float> d_vec(structure->z()->begin(), structure->z()->end());
    ASSERT_EQ(d_vec[0], 1.0);
    ASSERT_EQ(d_vec[1], 2.0);
    ASSERT_EQ(d_vec[2], 3.0);

}