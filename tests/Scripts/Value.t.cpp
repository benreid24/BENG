#include <BLIB/Scripts/Function.hpp>
#include <BLIB/Scripts/Script.hpp>
#include <BLIB/Scripts/Value.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace script
{
namespace unittest
{
TEST(Value, Void) {
    Value val;
    EXPECT_EQ(val.getType(), Value::TVoid);
}

TEST(Value, Numeric) {
    Value val;
    EXPECT_EQ(val.getType(), Value::TVoid);
    val = 15;
    EXPECT_EQ(val.getType(), Value::TNumeric);
    EXPECT_EQ(val.getAsNum(), 15);
}

TEST(Value, String) {
    Value val;
    EXPECT_EQ(val.getType(), Value::TVoid);
    val = "hello";
    EXPECT_EQ(val.getType(), Value::TString);
    EXPECT_EQ(val.getAsString(), "hello");
}

TEST(Value, Array) {
    Value val;
    Value::Array arr(3);
    arr[0] = Value::Ptr(new Value(12));
    arr[1] = Value::Ptr(new Value("world"));
    arr[2] = Value::Ptr(new Value(1));

    EXPECT_EQ(val.getType(), Value::TVoid);
    val = arr;
    EXPECT_EQ(val.getType(), Value::TArray);
    ASSERT_EQ(val.getAsArray().size(), 3);
    EXPECT_EQ(val.getAsArray()[0]->getType(), Value::TNumeric);
    EXPECT_EQ(val.getAsArray()[0]->getAsNum(), 12);
    EXPECT_EQ(val.getAsArray()[1]->getType(), Value::TString);
    EXPECT_EQ(val.getAsArray()[1]->getAsString(), "world");
    EXPECT_EQ(val.getAsArray()[2]->getType(), Value::TNumeric);
    EXPECT_EQ(val.getAsArray()[2]->getAsNum(), 1);

    EXPECT_EQ(val.getProperty("length")->getAsNum(), 3);
    EXPECT_FALSE(val.setProperty("length", 4));
    EXPECT_FALSE(val.setProperty("append", 4));
    EXPECT_FALSE(val.setProperty("resize", 4));
    EXPECT_FALSE(val.setProperty("clear", 4));
    EXPECT_FALSE(val.setProperty("insert", 4));
}

TEST(Value, Ref) {
    Value val;
    Value::Ptr ref(new Value());
    EXPECT_EQ(val.getType(), Value::TVoid);

    val = ref;
    ASSERT_EQ(val.getType(), Value::TRef);

    Value::Ptr deref = val.getAsRef().lock();
    EXPECT_EQ(deref->getType(), Value::TVoid);

    *deref = "hello";

    EXPECT_EQ(val.getAsRef().lock()->getType(), Value::TString);
    EXPECT_EQ(val.getAsRef().lock()->getAsString(), "hello");
}

TEST(Value, Properties) {
    Value val;

    EXPECT_EQ(val.getProperty("fake").get(), nullptr);

    EXPECT_TRUE(val.setProperty("set", std::string("woah")));
    EXPECT_EQ(val.getProperty("set")->getAsString(), "woah");

    Value val2 = val;
    EXPECT_EQ(val2.getProperty("set")->getAsString(), "woah");
    val2 = 5;
    EXPECT_EQ(val2.getProperty("set").get(), nullptr);
}

} // namespace unittest
} // namespace script
} // namespace bl