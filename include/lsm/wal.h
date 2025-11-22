#pragma once

#include <string>
#include <fstream>
#include <mutex>
#include <functional>

namespace lsm {

/**
 * Write-Ahead Log (WAL)
 * 
 * 想象一下，如果我们的数据库突然断电了，内存里的 MemTable 数据就全丢了。
 * 这就是 WAL 存在的意义：在写入内存之前，先把操作记录到磁盘上。
 * 
 * 就像记账一样：
 * 1. 先在日记本（WAL）上记一笔："张三存了 100 元"
 * 2. 然后再更新内存里的余额
 * 
 * 这样即使断电，重启后翻翻日记本，就能把余额恢复回来。
 */
class WAL {
public:
    // 构造函数：打开或创建日志文件
    explicit WAL(const std::string& dir);
    
    // 析构函数：确保文件正确关闭
    ~WAL();

    /**
     * 核心操作：追加写入
     * 
     * 我们需要记录两种操作：
     * 1. Put(key, value)
     * 2. Delete(key)
     * 
     * 思考：为什么不需要 Update？
     * 因为 LSM-Tree 里，Update 其实就是写入一个新的 Value，覆盖旧的。
     */
    void AppendPut(const std::string& key, const std::string& value);
    void AppendDelete(const std::string& key);

    /**
     * 恢复数据
     * 
     * 数据库启动时，我们需要读取 WAL 文件，重放里面的操作，
     * 把 MemTable 恢复到崩溃前的状态。
     * 
     * 这里使用回调函数，每读到一条记录，就通知上层（LSMTree）处理。
     */
    void Recover(std::function<void(const std::string& key, const std::string& value, bool is_delete)> callback);

    // 清空日志（当 MemTable 成功刷写到 SSTable 后，旧日志就可以删了）
    void Clear();



    // 定义日志类型
    enum class LogType : char {
        PUT = 0,
        DELETE = 1
    };
private:
    std::string log_path_;
    std::ofstream log_file_;
    std::mutex mutex_; // 保证多线程写入时的线程安全
};

} // namespace lsm
