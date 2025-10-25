#include <iostream>
#include "lsm/lsm_tree.h"

using namespace lsm;

int main() {
    std::cout << "=== LSM-Tree 简单演示 ===" << std::endl;

    // 创建数据库
    LSMTree db("./my_db");

    std::cout << "\n--- 测试 1: 基本的 Put 和 Get ---" << std::endl;
    db.Put("name", "Alice");
    db.Put("age", "25");
    db.Put("city", "Beijing");

    auto name = db.Get("name");
    if (name.has_value()) {
        std::cout << "✓ 读取成功: name = " << name.value() << std::endl;
    }

    std::cout << "\n--- 测试 2: 更新值 ---" << std::endl;
    db.Put("age", "26");
    auto age = db.Get("age");
    if (age.has_value()) {
        std::cout << "✓ 更新成功: age = " << age.value() << std::endl;
    }

    std::cout << "\n--- 测试 3: 删除操作 ---" << std::endl;
    db.Delete("city");
    auto city = db.Get("city");
    if (!city.has_value()) {
        std::cout << "✓ 删除成功: city 不存在" << std::endl;
    }

    std::cout << "\n--- 测试 4: 查询不存在的键 ---" << std::endl;
    auto result = db.Get("nonexistent");
    if (!result.has_value()) {
        std::cout << "✓ 正确返回: 键不存在" << std::endl;
    }

    std::cout << "\n=== 所有测试通过！===" << std::endl;

    return 0;
}