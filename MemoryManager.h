#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include <iostream>
#include <unordered_map>
#include <deque>
#include <vector>
#include <list>
#include <cstring>

#define MEMORY_BLOCKS 16
#define BLOCK_SIZE 4096

class MemoryManager {
public:
    // 页面置换算法枚举
    enum class ReplacementAlgorithm {
        FIFO,   // 先进先出
        LRU,    // 最近最少使用
        Random  // 随机置换
    };

    // 内存分配策略枚举
    enum class AllocationStrategy {
        FirstFit,  // 首次适应
        BestFit,   // 最佳适应
        WorstFit,  // 最差适应
        NextFit    // 下次适应
    };

    // 构造函数，指定页面置换算法
    MemoryManager(ReplacementAlgorithm algorithm = ReplacementAlgorithm::FIFO);

    // 析构函数
    ~MemoryManager();

    // 分配一个物理块给虚拟页
    bool allocateMemory(int virtualPageNumber);

    // 释放分配给虚拟页的内存
    void freeMemory(int virtualPageNumber);
    //用于与文件管理对接
    // 获取指定逻辑块号和偏移量的数据
    char getData(int logicalBlockNumber, int offset);

    // 设置页面替换算法
    void setReplacementAlgorithm(ReplacementAlgorithm algorithm);

    // 设置内存分配策略
    void setAllocationStrategy(AllocationStrategy strategy) {
        allocationStrategy = strategy;
        lastAllocatedBlock = -1;  // 重置最后分配的块索引
    }

    // 打印当前内存状态
    void printMemoryState() const;

    // 打印内存使用统计信息
    void printMemoryStatistics() const;

    // 内存碎片整理功能
    void defragmentMemory();
    // 内存管理器测试函数
    void MemoryManagerTest();
private:
    // 内存物理块结构
    struct MemoryBlock {
        bool isAllocated;            // 是否已分配
        int virtualPageNumber;       // 对应的虚拟页号，-1表示未分配
        char data[BLOCK_SIZE];       // 块的实际数据
    };

    // 分配一个物理块
    int allocatePhysicalBlock();

    // 释放一个物理块
    void freePhysicalBlock(int physicalBlockNumber);

    // 页面置换
    void replacePage();

    // 当前使用的页面置换算法
    ReplacementAlgorithm replacementAlgorithm;

    // 当前使用的内存分配策略
    AllocationStrategy allocationStrategy = AllocationStrategy::FirstFit;

    // 最后分配的块索引，用于NextFit策略
    int lastAllocatedBlock = -1;

    // 内存块数组
    std::vector<MemoryBlock> memory;

    // 页表，从虚拟页号映射到物理块号
    std::unordered_map<int, int> pageTable;

    // FIFO算法的队列
    std::deque<int> fifoQueue;

    // LRU算法的列表
    std::list<int> lruList;

    // 使用FirstFit策略分配块
    int allocateWithFirstFit();

    // 使用BestFit策略分配块
    int allocateWithBestFit();

    // 使用WorstFit策略分配块
    int allocateWithWorstFit();

    // 使用NextFit策略分配块
    int allocateWithNextFit();

    // 从磁盘读取块数据
    char* readBlock(int blockNumber);

    // 将块数据写回磁盘
    void writeBlock(const char* data, int blockNumber);


};


#endif // MEMORYMANAGER_H