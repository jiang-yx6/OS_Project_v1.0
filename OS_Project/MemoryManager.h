#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <vector>
#include <mutex>
#include <random>

class MemoryManager {
public:
    enum class AllocationMethod {
        FirstFit,
        BestFit
    };

private:
    struct Block {
        int start;   // 起始地址（单位：页）
        int size;    // 大小（单位：页）
        bool isFree; // 是否空闲
    };

    std::vector<Block> memory; // 模拟内存块
    AllocationMethod method;   // 内存分配方法
    std::mutex memMutex;       // 保护内存操作的互斥锁
    const int pageSize = 1024; // 页面大小（1KB）

public:
    MemoryManager(int totalPages, AllocationMethod method);
    ~MemoryManager();

    // 分配内存
    bool allocate(int pages);

    // 释放内存
    void deallocate(int startPage);

    // 打印内存状态到终端
    void printMemoryState() const;

private:
    // 首次适应算法
    int firstFit(int pages);

    // 最佳适应算法
    int bestFit(int pages);

    // 分割内存块
    void splitBlock(int index, int pages);

    // 合并相邻空闲块
    void mergeFreeBlocks();

    // 随机释放一块已分配的内存
    bool randomlyDeallocate();
};

// 测试函数声明
void Memorytest();

#endif // MEMORY_MANAGER_H