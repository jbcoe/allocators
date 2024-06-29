/* Copyright (c) 2024 The XYZ Authors. All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
==============================================================================*/

#include "dyn_optional.h"

#include "gtest/gtest.h"

namespace xyz {
namespace test {

class DynOptionalTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Setup code if needed
  }

  void TearDown() override {
    // Cleanup code if needed
  }
};

TEST_F(DynOptionalTest, DefaultConstructor) {
  dyn_optional<int> opt;
  EXPECT_FALSE(opt);
}

TEST_F(DynOptionalTest, ParameterizedConstructor) {
  dyn_optional<int> opt(5);
  ASSERT_TRUE(opt);
  EXPECT_EQ(*opt, 5);
}

TEST_F(DynOptionalTest, CopyConstructor) {
  dyn_optional<int> original(10);
  dyn_optional<int> copy(original);
  ASSERT_TRUE(copy);
  EXPECT_EQ(*copy, 10);
}

TEST_F(DynOptionalTest, MoveConstructor) {
  dyn_optional<int> original(15);
  dyn_optional<int> moved(std::move(original));
  ASSERT_TRUE(moved);
  EXPECT_EQ(*moved, 15);
  EXPECT_FALSE(original);
}

TEST_F(DynOptionalTest, CopyAssignment) {
  dyn_optional<int> original(20);
  dyn_optional<int> assigned;
  assigned = original;
  ASSERT_TRUE(assigned);
  EXPECT_EQ(*assigned, 20);
}

TEST_F(DynOptionalTest, MoveAssignment) {
  dyn_optional<int> original(25);
  dyn_optional<int> assigned;
  assigned = std::move(original);
  ASSERT_TRUE(assigned);
  EXPECT_EQ(*assigned, 25);
  EXPECT_FALSE(original);
}

TEST_F(DynOptionalTest, BooleanConversion) {
  dyn_optional<int> opt;
  EXPECT_FALSE(opt);
  opt = 30;
  EXPECT_TRUE(opt);
}

TEST_F(DynOptionalTest, Accessors) {
  dyn_optional<int> opt(35);
  ASSERT_TRUE(opt);
  *opt = 40;
  EXPECT_EQ(*opt, 40);
}

TEST_F(DynOptionalTest, Swap) {
  dyn_optional<int> opt1(45);
  dyn_optional<int> opt2(50);
  opt1.swap(opt2);
  EXPECT_EQ(*opt1, 50);
  EXPECT_EQ(*opt2, 45);
}

TEST_F(DynOptionalTest, Reset) {
  dyn_optional<int> opt(55);
  ASSERT_TRUE(opt);
  opt.reset();
  EXPECT_FALSE(opt);
}

}  // namespace test
}  // namespace xyz
