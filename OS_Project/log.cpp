#include "log.h"

// ��̬��Ա�Ķ���
Logger* Logger::instance = nullptr;
std::mutex Logger::mutex;