#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include <iostream>
#include <unordered_map>
#include <deque>
#include <vector>
#include <list>
#include <cstring> 
#include <string>
#define MAX_PAGES_PER_SEGMENT 8// 每个段的最大页数
#define MEMORY_BLOCKS 16
#define BLOCK_SIZE 4096
using namespace std;
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
        NextFit,    // 下次适应
        QuickFit,   // 快速适应
        BuddySystem // 伙伴系统支持
    };
    // 内存物理块结构
    struct MemoryBlock {
        bool isAllocated;            // 是否已分配
        int virtualPageNumber;       // 对应的虚拟页号，-1表示未分配
        char data[BLOCK_SIZE];       // 块的实际数据
    };

    //buddy块结构
    struct BuddyBlock {
        int size;            // 块大小（以 BLOCK_SIZE 为单位）
        bool isAllocated;    // 是否已分配
        int leftChild;       // 左孩子索引
        int rightChild;      // 右孩子索引
        int parent;          // 父节点索引
        int buddyIndex;      // 对应伙伴索引
    };

    // 段描述符
    struct SegmentDescriptor {
        int segmentId;                        // 段ID
        int totalPages;                       // 总页数
        std::unordered_map<int, int> pageTable; // 段内的页表（虚拟页号 -> 物理块号）
    };

    // 添加到 MemoryManager 类中作为成员变量
    std::vector<SegmentDescriptor> segmentList; // 所有段列表
    std::unordered_map<int, std::vector<int>> quickFitMap; // 替换之前的 static 变量
    bool quickFitInitialized = false;                     // 初始化标记
    int allocateWithBuddySystem(size_t sizeInBlocks); // 按大小分配
    void freeWithBuddySystem(int blockIndex);        // 释放并尝试合并
    void splitBlock(int index);                      // 分裂块
    int findBuddy(int index);                        // 查找伙伴
    void mergeBuddies(int index);                    // 合并伙伴
    std::vector<BuddyBlock> buddyBlocks;  // Buddy 系统使用的块数组
    int maxOrder;                         // 最大阶数（2^maxOrder = 总内存块数）

    // 构造函数，指定页面置换算法
    MemoryManager(ReplacementAlgorithm algorithm = ReplacementAlgorithm::FIFO);
    // 析构函数
    ~MemoryManager();


    // 页面置换
    void replacePage();
    // 分配一个物理块给虚拟页
    bool allocateMemory(int virtualPageNumber);
    // 分配一个物理块
    int allocatePhysicalBlock();
    // 释放一个物理块
    void freePhysicalBlock(int physicalBlockNumber);
    // 释放分配给虚拟页的内存
    void freeMemory(int virtualPageNumber);
    // 内存碎片整理功能
    void defragmentMemory();
    // 内存池
    std::vector<MemoryBlock> memory;
    // 页表，从虚拟页号映射到物理块号
    std::unordered_map<int, int> pageTable;


    // 设置页面替换算法
    void setReplacementAlgorithm(ReplacementAlgorithm algorithm);
    // 设置内存分配策略
    void setAllocationStrategy(AllocationStrategy strategy) {
        allocationStrategy = strategy;
        lastAllocatedBlock = -1;  // 重置最后分配的块索引
    }


    // 打印当前内存状态
    void printMemoryState() const;
    // 只打印页表
    void printPageTable() const {
        std::cout << "Page Table (Virtual -> Physical):\n";
        for (const auto& entry : pageTable) {
            std::cout << "Virtual Page " << entry.first
                << " -> Physical Block " << entry.second << "\n";
        }
    }
    // 打印内存使用统计信息
    void printMemoryStatistics() const;


    // 当前使用的页面置换算法
    ReplacementAlgorithm replacementAlgorithm;
    // 当前使用的内存分配策略
    AllocationStrategy allocationStrategy = AllocationStrategy::FirstFit;
    // 最后分配的块索引，用于NextFit策略
    int lastAllocatedBlock = -1;


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
    // 基于快速适配策略
    int allocateWithQuickFit();


    //用于与文件管理对接
    // 获取指定逻辑块号和偏移量的数据
    char getData(int logicalBlockNumber, int offset);
    // 从磁盘读取块数据
    char* readBlock(int blockNumber);
    // 将块数据写回磁盘
    void writeBlock(const char* data, int blockNumber);


    //用于与进程管理对接

    void allocatePCBmemory(string pname,int pid);
    void deletePCBmomory(string pname, int pid);

    //基于搜索的内存测试函数
    void SearchingBasedTest();
    //基于索引的内存分配函数
    void IndexBasedTest();
    //请求分页式内存测试
    void PageBasedTest();
    //段页式内存测试
    void SegmentBasedtest();
    // 内存管理器总测试函数
    void MemoryManagerTest();

};


#endif // MEMORYMANAGER_H
