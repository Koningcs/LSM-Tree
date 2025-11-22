#include "lsm/wal.h"
#include <iostream>
#include <filesystem>

namespace lsm {

namespace fs = std::filesystem;

// 1. 构造函数
WAL::WAL(const std::string& dir) {
    // 使用跨平台的 std::filesystem
    if (!fs::exists(dir)) {
        fs::create_directories(dir);
    }

    log_path_ = dir + "/wal.log";
    log_file_.open(log_path_, std::ios::app | std::ios::binary);
    if (!log_file_.is_open()) {
        std::cerr << "Failed to open WAL file: " << log_path_ << std::endl;
        exit(1);
    }
}

// 析构函数
WAL::~WAL() {
    if (log_file_.is_open()) {
        log_file_.close();
    }
}

// 2. 写入 Put 操作
void WAL::AppendPut(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    uint32_t key_len = key.size();
    uint32_t val_len = value.size();
    char type = static_cast<char>(LogType::PUT);    
    
    log_file_.write(&type, sizeof(type));
    log_file_.write(reinterpret_cast<const char*>(&key_len), sizeof(key_len));
    log_file_.write(key.c_str(), key_len);
    log_file_.write(reinterpret_cast<const char*>(&val_len), sizeof(val_len));
    log_file_.write(value.c_str(), val_len);
    log_file_.flush();
}

// 写入 Delete 操作
void WAL::AppendDelete(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    uint32_t key_len = key.size();
    uint32_t val_len = 0;
    char type = static_cast<char>(LogType::DELETE);    
    
    log_file_.write(&type, sizeof(type));
    log_file_.write(reinterpret_cast<const char*>(&key_len), sizeof(key_len));
    log_file_.write(key.c_str(), key_len);
    log_file_.write(reinterpret_cast<const char*>(&val_len), sizeof(val_len));
    log_file_.flush();
}

void WAL::Recover(std::function<void(const std::string& key, const std::string& value, bool is_delete)> callback) {

    std::ifstream log_file(log_path_, std::ios::binary);
    if (!log_file.is_open()) {
        return;
    }

    char _type;
    uint32_t _key_len;
    uint32_t _val_len;
    std::string _key;
    std::string _value;
    
    while(true) {
        log_file.read(&_type, sizeof(_type));
        if (log_file.eof()) break;
        if (log_file.fail()) {
            break;
        }
        
        log_file.read(reinterpret_cast<char*>(&_key_len), sizeof(_key_len));
        _key.resize(_key_len);
        log_file.read(const_cast<char*>(_key.data()), _key_len);

        log_file.read(reinterpret_cast<char*>(&_val_len), sizeof(_val_len));
        _value.resize(_val_len);
        log_file.read(const_cast<char*>(_value.data()), _val_len);

        bool is_delete = _type == static_cast<char>(LogType::DELETE);
        callback(_key, _value, is_delete);
    }

    log_file.close();
}

void WAL::Clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    log_file_.close();
    log_file_.open(log_path_, std::ios::out | std::ios::trunc | std::ios::binary);
}

} // namespace lsm
