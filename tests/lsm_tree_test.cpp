#include <gtest/gtest.h>
#include "lsm/lsm_tree.h"

using namespace lsm;

// ============================================
// 测试用例 1: 创建 LSMTree 对象
// ============================================
TEST(LSMTreeTest, ConstructorTest) {
    // 创建一个 LSMTree 对象，指定数据库路径
    LSMTree db("./test_db");

    // 只要能创建成功，测试就通过
    SUCCEED();
}

// ============================================
// 测试用例 2: 基本的 Put 和 Get
// ============================================
TEST(LSMTreeTest, BasicPutGet) {
    LSMTree db("./test_db");

    // 写入一个键值对
    db.Put("name", "Alice");

    // 读取这个键
    auto result = db.Get("name");

    // 断言：应该找到这个键
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), "Alice");
}

// ============================================
// 测试用例 3: 查询不存在的键
// ============================================
TEST(LSMTreeTest, GetNonExistentKey) {
    LSMTree db("./test_db");

    // 查询一个不存在的键
    auto result = db.Get("nonexistent");

    // 断言：应该返回 nullopt
    EXPECT_FALSE(result.has_value());
}

// ============================================
// 测试用例 4: 覆盖写入（Update）
// ============================================
TEST(LSMTreeTest, UpdateValue) {
    LSMTree db("./test_db");

    // 第一次写入
    db.Put("counter", "1");
    EXPECT_EQ(db.Get("counter").value(), "1");

    // 第二次写入（覆盖）
    db.Put("counter", "2");
    EXPECT_EQ(db.Get("counter").value(), "2");

    // 第三次写入（再次覆盖）
    db.Put("counter", "3");
    EXPECT_EQ(db.Get("counter").value(), "3");
}

// ============================================
// 测试用例 5: 删除操作
// ============================================
TEST(LSMTreeTest, DeleteKey) {
    LSMTree db("./test_db");

    // 先写入
    db.Put("temp", "temporary data");
    ASSERT_TRUE(db.Get("temp").has_value());

    // 删除
    db.Delete("temp");

    // 再查询，应该返回 nullopt
    auto result = db.Get("temp");
    EXPECT_FALSE(result.has_value());
}

// ============================================
// 测试用例 6: 多个键值对
// ============================================
TEST(LSMTreeTest, MultipleKeyValuePairs) {
    LSMTree db("./test_db");

    // 写入多个键值对
    db.Put("user:1", "Alice");
    db.Put("user:2", "Bob");
    db.Put("user:3", "Charlie");

    // 验证都能读取到
    EXPECT_EQ(db.Get("user:1").value(), "Alice");
    EXPECT_EQ(db.Get("user:2").value(), "Bob");
    EXPECT_EQ(db.Get("user:3").value(), "Charlie");

    // 删除中间的一个
    db.Delete("user:2");

    // 验证删除效果
    EXPECT_TRUE(db.Get("user:1").has_value());
    EXPECT_FALSE(db.Get("user:2").has_value());
    EXPECT_TRUE(db.Get("user:3").has_value());
}

// ============================================
// 测试用例 7: 空字符串
// ============================================
TEST(LSMTreeTest, EmptyStringValue) {
    LSMTree db("./test_db");

    // 写入空字符串作为 value
    db.Put("empty", "");

    // 应该能读取到空字符串（而不是 nullopt）
    auto result = db.Get("empty");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), "");
}

