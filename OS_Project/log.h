#pragma once

#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <sstream>
#include <ctime>
#include <mutex>
#include <string>
#include <iomanip>
#include <chrono>
#include <iostream>
#include <map>

#include"state.h"

class Logger {
private:
    static Logger* instance;
    static std::mutex mutex;
    std::ofstream logFile;

    // ANSI 转义码颜色定义
    const std::string RESET = "\033[0m";
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN = "\033[36m";

    // 不同类型日志对应的颜色
    std::map<std::string, std::string> colorMap = {
        {"创建", GREEN},
        {"状态变化", YELLOW},
        {"时间片", BLUE},
        {"完成", MAGENTA},
        {"调度", CYAN},
        {"错误", RED}
    };

    // 私有构造函数，实现单例模式
    Logger() {
        #ifdef _WIN32
            system("color");
        #endif
        logFile.open("process_manager.log", std::ios::app);
    }

    void writeLog(const std::string& type, const std::string& message) {
        std::string timestamp = getCurrentTimestamp();
        std::string color = colorMap[type];

        // 构建带颜色的消息
        std::string coloredMessage = color + timestamp + " [" + type + "] " + message + RESET;
        std::string nocolorMessage = timestamp + " [" + type + "] " + message;

        // 输出到文件和控制台
        logFile << nocolorMessage << std::endl;
        std::cout << coloredMessage << std::endl;
    }

public:
    // 进程状态转换为字符串
    static std::string stateToString(ProcessState state) {
        switch (state) {
        case READY: return "就绪";
        case RUNNING: return "运行";
        case BLOCKED: return "阻塞";
        case OVER: return "结束";
        default: return "未知状态";
        }
    }

    static Logger* getInstance() {
        if (instance == nullptr) {
            std::lock_guard<std::mutex> lock(mutex);
            if (instance == nullptr) {
                instance = new Logger();
            }
        }
        return instance;
    }

    // 获取当前时间戳
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto now_c = std::chrono::system_clock::to_time_t(now);
        struct tm timeinfo;
        localtime_s(&timeinfo, &now_c);  // 使用更安全的 localtime_s
        std::stringstream ss;
        ss << std::put_time(&timeinfo, "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    // 修改所有日志方法，使用 writeLog
    void logProcessCreation(const std::string& name, int pid, int priority, int operaTime) {
        std::lock_guard<std::mutex> lock(mutex);
        std::stringstream ss;
        ss << "进程名称: " << name
            << ", PID: " << pid
            << ", 优先级: " << priority
            << ", 预计运行时间: " << operaTime << "秒";
        writeLog("创建", ss.str());
    }

    void logStateChange(const std::string& name, int pid, ProcessState oldState, ProcessState newState) {
        std::lock_guard<std::mutex> lock(mutex);
        std::stringstream ss;
        ss << "进程名称: " << name
            << ", PID: " << pid
            << ", 状态从 " << stateToString(oldState)
            << " 变为 " << stateToString(newState);
        writeLog("状态变化", ss.str());
    }

    void logTimeSlice(const std::string& name, int pid, int remainingTime) {
        std::lock_guard<std::mutex> lock(mutex);
        std::stringstream ss;
        ss << "进程名称: " << name
            << ", PID: " << pid
            << ", 剩余运行时间: " << remainingTime << "秒";
        writeLog("时间片", ss.str());
    }

    void logProcessCompletion(const std::string& name, int pid) {
        std::lock_guard<std::mutex> lock(mutex);
        std::stringstream ss;
        ss << "进程名称: " << name
            << ", PID: " << pid
            << " 已完成执行";
        writeLog("完成", ss.str());
    }

    void logScheduling(const std::string& name, int pid) {
        std::lock_guard<std::mutex> lock(mutex);
        std::stringstream ss;
        ss << "进程名称: " << name
            << ", PID: " << pid
            << " 被调度执行";
        writeLog("调度", ss.str());
    }

    void logError(const std::string& errorMessage) {
        std::lock_guard<std::mutex> lock(mutex);
        writeLog("错误", errorMessage);
    }

    ~Logger() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }
};

#endif // LOGGER_H