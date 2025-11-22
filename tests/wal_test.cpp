#include <gtest/gtest.h>
#include "lsm/lsm_tree.h"
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

class WALTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 每个测试前清理环境
        if (fs::exists(test_dir)) {
            fs::remove_all(test_dir);
        }
        fs::create_directories(test_dir);
    }

    void TearDown() override {
        // 测试后清理
        if (fs::exists(test_dir)) {
            fs::remove_all(test_dir);
        }
    }

    const std::string test_dir = "test_wal_db";
};

TEST_F(WALTest, TestDurability) {
    // 1. 启动数据库，写入数据
    {
        lsm::LSMTree db(test_dir);
        db.Put("key1", "value1");
        db.Put("key2", "value2");
        db.Delete("key1"); // 删除 key1
        db.Put("key3", "value3");
    }

    // 2. 检查 WAL 文件是否存在
    ASSERT_TRUE(fs::exists(test_dir + "/wal.log"));

    // 3. 重新打开数据库（模拟崩溃恢复）
    {
        lsm::LSMTree db(test_dir);

        // 验证 key1 被删除
        auto val1 = db.Get("key1");
        EXPECT_FALSE(val1.has_value());

        // 验证 key2 存在
        auto val2 = db.Get("key2");
        ASSERT_TRUE(val2.has_value());
        EXPECT_EQ(val2.value(), "value2");

        // 验证 key3 存在
        auto val3 = db.Get("key3");
        ASSERT_TRUE(val3.has_value());
        EXPECT_EQ(val3.value(), "value3");
    }
}
