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

    // ANSI ת������ɫ����
    const std::string RESET = "\033[0m";
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN = "\033[36m";

    // ��ͬ������־��Ӧ����ɫ
    std::map<std::string, std::string> colorMap = {
        {"����", GREEN},
        {"״̬�仯", YELLOW},
        {"ʱ��Ƭ", BLUE},
        {"���", MAGENTA},
        {"����", CYAN},
        {"����", RED}
    };

    // ˽�й��캯����ʵ�ֵ���ģʽ
    Logger() {
        #ifdef _WIN32
            system("color");
        #endif
        logFile.open("process_manager.log", std::ios::app);
    }

    void writeLog(const std::string& type, const std::string& message) {
        std::string timestamp = getCurrentTimestamp();
        std::string color = colorMap[type];

        // ��������ɫ����Ϣ
        std::string coloredMessage = color + timestamp + " [" + type + "] " + message + RESET;
        std::string nocolorMessage = timestamp + " [" + type + "] " + message;

        // ������ļ��Ϳ���̨
        logFile << nocolorMessage << std::endl;
        std::cout << coloredMessage << std::endl;
    }

public:
    // ����״̬ת��Ϊ�ַ���
    static std::string stateToString(ProcessState state) {
        switch (state) {
        case READY: return "����";
        case RUNNING: return "����";
        case BLOCKED: return "����";
        case OVER: return "����";
        default: return "δ֪״̬";
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

    // ��ȡ��ǰʱ���
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto now_c = std::chrono::system_clock::to_time_t(now);
        struct tm timeinfo;
        localtime_s(&timeinfo, &now_c);  // ʹ�ø���ȫ�� localtime_s
        std::stringstream ss;
        ss << std::put_time(&timeinfo, "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    // �޸�������־������ʹ�� writeLog
    void logProcessCreation(const std::string& name, int pid, int priority, int operaTime) {
        std::lock_guard<std::mutex> lock(mutex);
        std::stringstream ss;
        ss << "��������: " << name
            << ", PID: " << pid
            << ", ���ȼ�: " << priority
            << ", Ԥ������ʱ��: " << operaTime << "��";
        writeLog("����", ss.str());
    }

    void logStateChange(const std::string& name, int pid, ProcessState oldState, ProcessState newState) {
        std::lock_guard<std::mutex> lock(mutex);
        std::stringstream ss;
        ss << "��������: " << name
            << ", PID: " << pid
            << ", ״̬�� " << stateToString(oldState)
            << " ��Ϊ " << stateToString(newState);
        writeLog("״̬�仯", ss.str());
    }

    void logTimeSlice(const std::string& name, int pid, int remainingTime) {
        std::lock_guard<std::mutex> lock(mutex);
        std::stringstream ss;
        ss << "��������: " << name
            << ", PID: " << pid
            << ", ʣ������ʱ��: " << remainingTime << "��";
        writeLog("ʱ��Ƭ", ss.str());
    }

    void logProcessCompletion(const std::string& name, int pid) {
        std::lock_guard<std::mutex> lock(mutex);
        std::stringstream ss;
        ss << "��������: " << name
            << ", PID: " << pid
            << " �����ִ��";
        writeLog("���", ss.str());
    }

    void logScheduling(const std::string& name, int pid) {
        std::lock_guard<std::mutex> lock(mutex);
        std::stringstream ss;
        ss << "��������: " << name
            << ", PID: " << pid
            << " ������ִ��";
        writeLog("����", ss.str());
    }

    void logError(const std::string& errorMessage) {
        std::lock_guard<std::mutex> lock(mutex);
        writeLog("����", errorMessage);
    }

    ~Logger() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }
};

#endif // LOGGER_H