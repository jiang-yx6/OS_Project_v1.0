#include "log.h"

// 静态成员的定义
Logger* Logger::instance = nullptr;
std::mutex Logger::mutex;