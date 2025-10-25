#pragma once

#include <string>
#include <optional>
#include <map>

namespace lsm {

/**
 * LSM-Tree 核心类
 *
 * 这是一个简化版的 LSM-Tree 实现，包含三个核心组件：
 * 1. MemTable - 内存中的跳表
 * 2. SSTable - 磁盘上的有序表
 * 3. WAL - 预写日志
 *
 * 当前版本仅实现 MemTable，数据仅存储在内存中
 */
class LSMTree {
public:
    /**
     * 构造函数
     * @param db_path 数据库存储的路径
     */
    explicit LSMTree(const std::string& db_path);

    /**
     * 析构函数
     */
    ~LSMTree();

    /**
     * 写入键值对
     * @param key 键
     * @param value 值
     */
    void Put(const std::string& key, const std::string& value);

    /**
     * 读取键对应的值
     * @param key 键
     * @return 如果 key 存在返回 value，否则返回 std::nullopt
     */
    std::optional<std::string> Get(const std::string& key);

    /**
     * 删除键
     * @param key 键
     */
    void Delete(const std::string& key);

private:
    std::string db_path_;  // 数据库路径

    /**
     * MemTable - 使用 std::map 模拟内存中的跳表
     *
     * 为什么使用 map：
     * - std::map 是红黑树，自动按 key 排序
     * - 生产环境中会使用跳表（Skip List），但实现复杂
     *
     * 为什么 value 是 optional<string>：
     * - 如果是 nullopt 表示这个 key 被删除了
     * - 这样可以区分"不存在"和"被删除"
     */
    std::map<std::string, std::optional<std::string>> memtable_;
};

} // namespace lsm
