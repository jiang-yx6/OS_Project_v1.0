#pragma once
#ifndef PROCESS_STATE_H
#define PROCESS_STATE_H

// 进程状态枚举
typedef enum {
    READY,      // 就绪
    RUNNING,    // 运行
    BLOCKED,    // 阻塞
    OVER        // 结束
} ProcessState;

#endif // PROCESS_STATE_H