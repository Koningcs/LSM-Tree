#include "lsm/lsm_tree.h"
#include <iostream>
using namespace std;
namespace lsm {

// ============================================
// 构造函数
// ============================================
LSMTree::LSMTree(const std::string& db_path)
    : db_path_(db_path) {
    // 目前只是保存路径，后续会在这里：
    // 1. 创建数据库目录
    // 2. 加载已有的 SSTable 文件
    // 3. 恢复 WAL 日志
    std::cout << "[LSMTree] init database, PATH: " << db_path_ << std::endl;
}

// ============================================
// 析构函数
// ============================================
LSMTree::~LSMTree() {
    // 目前什么都不做，后续会在这里：
    // 1. 刷新 MemTable 到磁盘
    // 2. 关闭所有文件
    // 3. 停止后台线程
    std::cout << "[LSMTree] close database" << std::endl;
}

// ============================================
// Put 操作 - 写入键值对
// ============================================
void LSMTree::Put(const std::string& key, const std::string& value) {
    // 当前简化版实现：
    // 1. 直接写入 MemTable（使用 std::map）
    // 2. value 包装成 optional，有值表示这是正常数据

    memtable_[key] = value;

    std::cout << "[LSMTree] Put: " << key << " = " << value << std::endl;

    // TODO（后续实现）：
    // 1. 先写 WAL 日志
    // 2. 检查 MemTable 大小
    // 3. 如果超过阈值，触发刷盘
}

// ============================================
// Get 操作 - 读取值
// ============================================
std::optional<std::string> LSMTree::Get(const std::string& key) {
    // 当前简化版实现：
    // 1. 只查询 MemTable

    auto it = memtable_.find(key);

    if (it != memtable_.end()) {
        // 找到了 key
        if (it->second.has_value()) {
            // 有值，返回
            std::cout << "[LSMTree] Get: " << key << " = " << it->second.value() << std::endl;
            return it->second.value();
        } else {
            // 值是 nullopt，表示已被删除
            std::cout << "[LSMTree] Get: " << key << " (already delete)" << std::endl;
            return std::nullopt;
        }
    }

    // 没找到
    std::cout << "[LSMTree] Get: " << key << " (can not find)" << std::endl;
    return std::nullopt;

    // TODO（后续实现）：
    // 1. 查询 Immutable MemTable
    // 2. 从 Level 0 到 Level N 依次查询 SSTable
    // 3. 使用布隆过滤器优化查询
}

// ============================================
// Delete 操作 - 删除键
// ============================================
void LSMTree::Delete(const std::string& key) {
    // 关键点：LSM-Tree 的删除不是真删除！
    // 而是写入一个"墓碑"标记（Tombstone）

    // 墓碑标记用 std::nullopt 表示
    memtable_[key] = std::nullopt;

    std::cout << "[LSMTree] Delete: " << key << std::endl;

    // TODO（后续实现）：
    // 1. 先写 WAL 日志
    // 2. 墓碑标记在 Compaction 时才真正删除数据
}
