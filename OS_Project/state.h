#pragma once
#ifndef PROCESS_STATE_H
#define PROCESS_STATE_H

// ����״̬ö��
typedef enum {
    READY,      // ����
    RUNNING,    // ����
    BLOCKED,    // ����
    OVER        // ����
} ProcessState;

#endif // PROCESS_STATE_H