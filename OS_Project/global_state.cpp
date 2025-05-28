#include "global_state.h"

std::atomic<bool> g_is_program_running(true);

void signal_handler(int signum) {
    if (signum == SIGINT) {
        std::cout << "\nCtrl+C detected. Shutting down gracefully...\n";
        g_is_program_running.store(false); // 设置终止标志为 false
    }
}