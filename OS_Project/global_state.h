#pragma once
#ifndef GLOBAL_STATE_H
#define GLOBAL_STATE_H

#include <atomic>
#include <csignal>
#include <iostream>

// ȫ��ԭ�ӱ�־��ָʾ�����Ƿ�Ӧ����
extern std::atomic<bool> g_is_program_running;

// �źŴ�����
void signal_handler(int signum);

#endif // GLOBAL_STATE_H