#pragma once
#ifndef GLOBAL_STATE_H
#define GLOBAL_STATE_H

#include <atomic>
#include <csignal>
#include <iostream>

// 全局原子标志，指示程序是否应运行
extern std::atomic<bool> g_is_program_running;

// 信号处理函数
void signal_handler(int signum);

#endif // GLOBAL_STATE_H