//#include "MemoryManager.h"
//#include <iostream>
//#include <stdexcept>
//#include <thread>
//#include <vector>
//#include <random>
//#include <chrono>
//#include <mutex>
//#include <condition_variable>
//
//// 构造函数
//MemoryManager::MemoryManager(int totalPages, AllocationMethod method)
//    : method(method) {
//    memory.push_back(Block{ 0, totalPages, true }); // 初始化一块空闲内存
//}
//
//// 析构函数
//MemoryManager::~MemoryManager() {}
//
//// 分配内存
//bool MemoryManager::allocate(int pages) {
//    std::lock_guard<std::mutex> lock(memMutex);
//    int index = -1;
//
//    if (method == AllocationMethod::FirstFit) {
//        index = firstFit(pages);
//    }
//    else if (method == AllocationMethod::BestFit) {
//        index = bestFit(pages);
//    }
//
//    if (index != -1) {
//        memory[index].isFree = false;
//        return true;
//    }
//
//    // 如果内存不足，尝试随机释放一块已分配的内存
//    if (randomlyDeallocate()) {
//        // 重新尝试分配
//        if (method == AllocationMethod::FirstFit) {
//            index = firstFit(pages);
//        }
//        else if (method == AllocationMethod::BestFit) {
//            index = bestFit(pages);
//        }
//        if (index != -1) {
//            memory[index].isFree = false;
//            return true;
//        }
//    }
//
//    return false; // 分配失败
//}
//
//// 释放内存
//void MemoryManager::deallocate(int startPage) {
//    std::lock_guard<std::mutex> lock(memMutex);
//    for (auto& block : memory) {
//        if (block.start == startPage && !block.isFree) {
//            block.isFree = true;
//            mergeFreeBlocks();
//            return;
//        }
//    }
//}
//
//// 打印内存状态到终端
//void MemoryManager::printMemoryState() const {
//    std::cout << "当前内存状态:\n";
//    for (const auto& block : memory) {
//        std::cout << "起始页: " << block.start << ", 大小: " << block.size
//            << " 页, 状态: " << (block.isFree ? "空闲" : "已分配") << "\n";
//    }
//    std::cout << "--------------------------\n";
//}
//
//// 首次适应算法
//int MemoryManager::firstFit(int pages) {
//    for (int i = 0; i < memory.size(); ++i) {
//        if (memory[i].isFree && memory[i].size >= pages) {
//            splitBlock(i, pages);
//            return i;
//        }
//    }
//    return -1; // 未找到合适的块
//}
//
//// 最佳适应算法
//int MemoryManager::bestFit(int pages) {
//    int bestIndex = -1;
//    int minSize = INT32_MAX;
//
//    for (int i = 0; i < memory.size(); ++i) {
//        if (memory[i].isFree && memory[i].size >= pages && memory[i].size < minSize) {
//            minSize = memory[i].size;
//            bestIndex = i;
//        }
//    }
//
//    if (bestIndex != -1) {
//        splitBlock(bestIndex, pages);
//    }
//    return bestIndex;
//}
//
//// 分割内存块
//void MemoryManager::splitBlock(int index, int pages) {
//    if (memory[index].size > pages) {
//        Block newBlock = { memory[index].start + pages, memory[index].size - pages, true };
//        memory.insert(memory.begin() + index + 1, newBlock);
//    }
//    memory[index].size = pages;
//    memory[index].isFree = false;
//}
//
//// 合并相邻空闲块
//void MemoryManager::mergeFreeBlocks() {
//    for (int i = 0; i < memory.size() - 1; ++i) {
//        if (memory[i].isFree && memory[i + 1].isFree) {
//            memory[i].size += memory[i + 1].size;
//            memory.erase(memory.begin() + i + 1);
//            --i; // 回退一步重新检查
//        }
//    }
//}
//
//// 随机释放一块已分配的内存
//bool MemoryManager::randomlyDeallocate() {
//    std::vector<int> allocatedIndices;
//    for (int i = 0; i < memory.size(); ++i) {
//        if (!memory[i].isFree) {
//            allocatedIndices.push_back(i);
//        }
//    }
//
//    if (allocatedIndices.empty()) {
//        return false; // 没有已分配的内存块
//    }
//
//    // 随机选择一个已分配的块
//    std::random_device rd;
//    std::mt19937 gen(rd());
//    std::uniform_int_distribution<> dist(0, allocatedIndices.size() - 1);
//    int randomIndex = allocatedIndices[dist(gen)];
//
//    // 释放该块
//    memory[randomIndex].isFree = true;
//    mergeFreeBlocks();
//    std::cout << "内存不足！随机释放了起始页为 " << memory[randomIndex].start << " 的内存块。\n";
//    return true;
//}
//
//// 测试函数实现
//void Memorytest() {
//    try {
//        // 全局变量
//        std::mutex globalMutex;
//        std::condition_variable cv;
//        bool isAllocating = true; // 控制线程同步
//
//        // 用户内存容量（4页到32页）
//        std::random_device rd;
//        std::mt19937 gen(rd());
//        std::uniform_int_distribution<> userMemoryDist(16, 32); // 用户内存总页数
//        int totalPages = userMemoryDist(gen);
//
//        // 请求序列（随机生成分配和释放操作）
//        std::uniform_int_distribution<> requestSizeDist(1, 8); // 每次请求的页数范围
//        int numRequests = 10; // 总请求数量
//        std::vector<int> requestSequence(numRequests);
//        for (int i = 0; i < numRequests; ++i) {
//            requestSequence[i] = requestSizeDist(gen);
//        }
//
//        // 用户选择内存管理方式
//        std::cout << "选择内存管理方式 (1: 首次适应, 2: 最佳适应): ";
//        int choice;
//        std::cin >> choice;
//        MemoryManager::AllocationMethod method = MemoryManager::AllocationMethod::FirstFit;
//        if (choice == 2) {
//            method = MemoryManager::AllocationMethod::BestFit;
//        }
//
//        MemoryManager manager(totalPages, method);
//
//        // 内存分配线程
//        std::thread allocThread([&manager, &requestSequence, &globalMutex, &cv, &isAllocating]() {
//            for (int pages : requestSequence) {
//                {
//                    std::unique_lock<std::mutex> lock(globalMutex);
//                    cv.wait(lock, [&isAllocating] { return isAllocating; });
//                    std::cout << "尝试分配 " << pages << " 页内存...\n";
//                    bool success = manager.allocate(pages);
//                    if (success) {
//                        std::cout << "成功分配 " << pages << " 页内存。\n";
//                    }
//                    else {
//                        std::cout << "分配失败！内存不足。\n";
//                    }
//                    manager.printMemoryState(); // 打印内存状态
//                    isAllocating = false; // 切换到跟踪线程
//                }
//                cv.notify_one();
//                std::this_thread::sleep_for(std::chrono::milliseconds(500)); // 模拟延迟
//            }
//            });
//
//        // 跟踪内存状态线程
//        std::thread trackThreadObj([&manager, &globalMutex, &cv, &isAllocating, numRequests]() {
//            for (int i = 0; i < numRequests; ++i) {
//                {
//                    std::unique_lock<std::mutex> lock(globalMutex);
//                    cv.wait(lock, [&isAllocating] { return !isAllocating; });
//                    std::cout << "跟踪内存状态...\n";
//                    manager.printMemoryState(); // 打印内存状态
//                    isAllocating = true; // 切换回分配线程
//                }
//                cv.notify_one();
//                std::this_thread::sleep_for(std::chrono::milliseconds(500)); // 模拟延迟
//            }
//            });
//
//        // 等待线程完成
//        allocThread.join();
//        trackThreadObj.join();
//
//        std::cout << "模拟完成！\n";
//    }
//    catch (const std::exception& e) {
//        std::cerr << "发生错误: " << e.what() << "\n";
//    }
//}
#include "MemoryManager.h"
#include "file.h"
#include <algorithm>
#include <random>
#include <stdexcept>
#include<string>

// 构造函数
MemoryManager::MemoryManager(ReplacementAlgorithm algorithm)
    : replacementAlgorithm(algorithm), memory(MEMORY_BLOCKS) {
    for (auto& block : memory) {
        block.isAllocated = false;
        block.virtualPageNumber = -1; // 初始化为无效值
    }
}

// 析构函数
MemoryManager::~MemoryManager() {}

// 分配一个物理块
int MemoryManager::allocatePhysicalBlock() {
    for (int i = 0; i < MEMORY_BLOCKS; ++i) {
        if (!memory[i].isAllocated) {
            memory[i].isAllocated = true;
            return i; // 返回分配的物理块号
        }
    }
    return -1; // 内存不足
}

// 释放一个物理块
void MemoryManager::freePhysicalBlock(int physicalBlockNumber) {
    if (physicalBlockNumber < 0 || physicalBlockNumber >= MEMORY_BLOCKS) {
        throw std::out_of_range("Invalid physical block number");
    }

    // 如果有映射的虚拟页号，将数据写回逻辑块
    if (memory[physicalBlockNumber].virtualPageNumber != -1) {
        writeBlock(memory[physicalBlockNumber].data, memory[physicalBlockNumber].virtualPageNumber);
    }

    memory[physicalBlockNumber].isAllocated = false;
    memory[physicalBlockNumber].virtualPageNumber = -1;
}

// 页面置换算法
void MemoryManager::replacePage() {
    int physicalBlockNumber = -1;

    switch (replacementAlgorithm) {
    case ReplacementAlgorithm::FIFO: {
        if (fifoQueue.empty()) {
            throw std::runtime_error("No pages to replace in FIFO queue.");
        }
        physicalBlockNumber = fifoQueue.front();
        fifoQueue.pop_front();
        break;
    }
    case ReplacementAlgorithm::LRU: {
        if (lruList.empty()) {
            throw std::runtime_error("No pages to replace in LRU list.");
        }
        physicalBlockNumber = lruList.front();
        lruList.erase(lruList.begin());
        break;
    }
    case ReplacementAlgorithm::Random: {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, MEMORY_BLOCKS - 1);
        do {
            physicalBlockNumber = dist(gen);
        } while (!memory[physicalBlockNumber].isAllocated);
        break;
    }
    }

    // 找到对应的虚拟页号并从页表中移除
    int virtualPageNumber = memory[physicalBlockNumber].virtualPageNumber;
    if (virtualPageNumber != -1) {
        pageTable.erase(virtualPageNumber);
    }

    // 释放物理块
    freePhysicalBlock(physicalBlockNumber);
}

// 分配内存给虚拟页号
bool MemoryManager::allocateMemory(int virtualPageNumber) {
    // 如果虚拟页号已经存在，直接返回
    if (pageTable.find(virtualPageNumber) != pageTable.end()) {
        return true;
    }

    int physicalBlockNumber = allocatePhysicalBlock();
    if (physicalBlockNumber == -1) {
        // 内存不足，触发页面置换
        replacePage();
        physicalBlockNumber = allocatePhysicalBlock();
        if (physicalBlockNumber == -1) {
            std::cout << "Memory allocation failed after page replacement.\n";
            return false;
        }
    }

    // 更新页表
    pageTable[virtualPageNumber] = physicalBlockNumber;

    // 更新调度队列或列表
    fifoQueue.push_back(physicalBlockNumber);
    lruList.push_back(physicalBlockNumber);

    // 从逻辑块读取数据到物理块
    char* data = readBlock(virtualPageNumber);
    std::memcpy(memory[physicalBlockNumber].data, data, BLOCK_SIZE);

    // 更新物理块信息
    memory[physicalBlockNumber].virtualPageNumber = virtualPageNumber;

    std::cout << "Allocated physical block " << physicalBlockNumber
        << " to virtual page " << virtualPageNumber << "\n";
    return true;
}

// 释放内存
void MemoryManager::freeMemory(int virtualPageNumber) {
    auto it = pageTable.find(virtualPageNumber);
    if (it == pageTable.end()) {
        std::cout << "Virtual page " << virtualPageNumber << " not found in page table.\n";
        return;
    }

    int physicalBlockNumber = it->second;

    // 释放物理块
    freePhysicalBlock(physicalBlockNumber);

    // 从页表中移除
    pageTable.erase(it);

    // 从调度队列中移除
    fifoQueue.erase(std::remove(fifoQueue.begin(), fifoQueue.end(), physicalBlockNumber), fifoQueue.end());
    lruList.erase(std::remove(lruList.begin(), lruList.end(), physicalBlockNumber), lruList.end());

    std::cout << "Freed physical block " << physicalBlockNumber << " from virtual page " << virtualPageNumber << "\n";
}

// 获取指定逻辑块号和偏移量的数据
char MemoryManager::getData(int logicalBlockNumber, int offset) {
    // 检查偏移量是否有效
    if (offset < 0 || offset >= BLOCK_SIZE) {
        throw std::out_of_range("Invalid offset: " + std::to_string(offset));
    }

    // 检查逻辑块号是否在内存中
    auto it = pageTable.find(logicalBlockNumber);
    if (it == pageTable.end()) {
        std::cout << "Page fault: Logical block " << logicalBlockNumber << " not in memory.\n";

        // 调入逻辑块到内存
        if (!allocateMemory(logicalBlockNumber)) {
            throw std::runtime_error("Failed to load logical block into memory.");
        }

        // 再次查找物理块号
        it = pageTable.find(logicalBlockNumber);
        if (it == pageTable.end()) {
            throw std::runtime_error("Logical block still not in memory after allocation.");
        }
    }

    int physicalBlockNumber = it->second;

    // 返回指定偏移量的数据
    return memory[physicalBlockNumber].data[offset];
}

// 打印内存状态
void MemoryManager::printMemoryState() const {
    std::cout << "Memory Status:\n";
    for (int i = 0; i < MEMORY_BLOCKS; ++i) {
        std::cout << "Block " << i << ": " << (memory[i].isAllocated ? "Allocated" : "Free")
            << ", Virtual Page: " << memory[i].virtualPageNumber << "\n";
    }
    std::cout << "Page Table:\n";
    for (const auto& entry : pageTable) {
        std::cout << "Virtual Page " << entry.first << " -> Physical Block " << entry.second << "\n";
    }
}

// 设置调度算法
void MemoryManager::setReplacementAlgorithm(ReplacementAlgorithm algorithm) {
    replacementAlgorithm = algorithm;
}
