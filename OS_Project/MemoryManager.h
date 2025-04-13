//#ifndef MEMORY_MANAGER_H
//#define MEMORY_MANAGER_H
//
//#include <vector>
//#include <mutex>
//#include <random>
//using namespace std;
//class MemoryManager {
//public:
//    enum class AllocationMethod {
//        FirstFit,
//        BestFit
//    };
//    enum class Replacement {
//        LRU,
//        Random,
//        FIFO
//    };
//
//private:
//    struct Block {
//        int start;   // 起始地址（单位：页）
//        int size;    // 大小（单位：页）
//        bool isFree; // 是否空闲
//    };
//    vector<Block> memory; // 模拟内存段
//    AllocationMethod method;   // 内存分配方法
//
//    mutex memMutex;       // 保护内存操作的互斥锁
//    const int pageSize = 4096; // 页面大小（1KB）
//
//public:
//    MemoryManager(int totalPages, AllocationMethod method);
//    ~MemoryManager();
//
//    // 分配内存
//    bool allocate(int pages);
//
//    // 释放内存
//    void deallocate(int startPage);
//
//    // 打印内存状态到终端
//    void printMemoryState() const;
//
//private:
//    // 首次适应算法
//    int firstFit(int pages);
//
//    // 最佳适应算法
//    int bestFit(int pages);
//
//    // 分割内存块
//    void splitBlock(int index, int pages);
//
//    // 合并相邻空闲块
//    void mergeFreeBlocks();
//
//    // 随机释放一块已分配的内存
//    bool randomlyDeallocate();
//};
//
//// 测试函数声明
//void Memorytest();
//
//#endif // MEMORY_MANAGER_H
#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <iostream>
#include <map>
#include <vector>
#include <deque>
#include "file.h"

#define MEMORY_BLOCKS 16
#define BLOCK_SIZE 4096

class MemoryManager {
public:
    enum class ReplacementAlgorithm { FIFO, LRU, Random };

    MemoryManager(ReplacementAlgorithm algorithm);
    ~MemoryManager();

    // 分配一个物理块
    int allocatePhysicalBlock();

    // 释放一个物理块
    void freePhysicalBlock(int physicalBlockNumber);

    // 页面置换算法
    void replacePage();

    // 分配内存给虚拟页号
    bool allocateMemory(int virtualPageNumber);

    // 释放内存
    void freeMemory(int virtualPageNumber);

    // 获取指定逻辑块号和偏移量的数据
    char getData(int logicalBlockNumber, int offset);

    // 打印内存状态
    void printMemoryState() const;

    // 设置调度算法
    void setReplacementAlgorithm(ReplacementAlgorithm algorithm);

private:
    struct MemoryBlock {
        bool isAllocated;
        int virtualPageNumber;
        char data[BLOCK_SIZE];
    };

    ReplacementAlgorithm replacementAlgorithm;
    std::vector<MemoryBlock> memory;
    std::map<int, int> pageTable; // 虚拟页号 -> 物理块号
    std::deque<int> fifoQueue;    // FIFO 队列
    std::vector<int> lruList;     // LRU 列表

    // 模拟从逻辑块读取数据
    char* readBlock(int virtualPageNumber) {
        static char buffer[BLOCK_SIZE];
        std::fill(buffer, buffer + BLOCK_SIZE, 'A' + (virtualPageNumber % 26)); // 填充模拟数据
        return buffer;
    }

    // 模拟将数据写回逻辑块
    void writeBlock(const char* data, int virtualPageNumber) {
        // 模拟写回操作
        std::cout << "Writing data back to logical block " << virtualPageNumber << "\n";
    }
};
#endif // MEMORY_MANAGER_H