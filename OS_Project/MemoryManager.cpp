#include "MemoryManager.h"
#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>
#include <random>
#include <chrono>
#include <mutex>
#include <condition_variable>

// 构造函数
MemoryManager::MemoryManager(int totalPages, AllocationMethod method)
    : method(method) {
    memory.push_back(Block{ 0, totalPages, true }); // 初始化一块空闲内存
}

// 析构函数
MemoryManager::~MemoryManager() {}

// 分配内存
bool MemoryManager::allocate(int pages) {
    std::lock_guard<std::mutex> lock(memMutex);
    int index = -1;

    if (method == AllocationMethod::FirstFit) {
        index = firstFit(pages);
    }
    else if (method == AllocationMethod::BestFit) {
        index = bestFit(pages);
    }

    if (index != -1) {
        memory[index].isFree = false;
        return true;
    }

    // 如果内存不足，尝试随机释放一块已分配的内存
    if (randomlyDeallocate()) {
        // 重新尝试分配
        if (method == AllocationMethod::FirstFit) {
            index = firstFit(pages);
        }
        else if (method == AllocationMethod::BestFit) {
            index = bestFit(pages);
        }
        if (index != -1) {
            memory[index].isFree = false;
            return true;
        }
    }

    return false; // 分配失败
}

// 释放内存
void MemoryManager::deallocate(int startPage) {
    std::lock_guard<std::mutex> lock(memMutex);
    for (auto& block : memory) {
        if (block.start == startPage && !block.isFree) {
            block.isFree = true;
            mergeFreeBlocks();
            return;
        }
    }
}

// 打印内存状态到终端
void MemoryManager::printMemoryState() const {
    std::cout << "当前内存状态:\n";
    for (const auto& block : memory) {
        std::cout << "起始页: " << block.start << ", 大小: " << block.size
            << " 页, 状态: " << (block.isFree ? "空闲" : "已分配") << "\n";
    }
    std::cout << "--------------------------\n";
}

// 首次适应算法
int MemoryManager::firstFit(int pages) {
    for (int i = 0; i < memory.size(); ++i) {
        if (memory[i].isFree && memory[i].size >= pages) {
            splitBlock(i, pages);
            return i;
        }
    }
    return -1; // 未找到合适的块
}

// 最佳适应算法
int MemoryManager::bestFit(int pages) {
    int bestIndex = -1;
    int minSize = INT32_MAX;

    for (int i = 0; i < memory.size(); ++i) {
        if (memory[i].isFree && memory[i].size >= pages && memory[i].size < minSize) {
            minSize = memory[i].size;
            bestIndex = i;
        }
    }

    if (bestIndex != -1) {
        splitBlock(bestIndex, pages);
    }
    return bestIndex;
}

// 分割内存块
void MemoryManager::splitBlock(int index, int pages) {
    if (memory[index].size > pages) {
        Block newBlock = { memory[index].start + pages, memory[index].size - pages, true };
        memory.insert(memory.begin() + index + 1, newBlock);
    }
    memory[index].size = pages;
    memory[index].isFree = false;
}

// 合并相邻空闲块
void MemoryManager::mergeFreeBlocks() {
    for (int i = 0; i < memory.size() - 1; ++i) {
        if (memory[i].isFree && memory[i + 1].isFree) {
            memory[i].size += memory[i + 1].size;
            memory.erase(memory.begin() + i + 1);
            --i; // 回退一步重新检查
        }
    }
}

// 随机释放一块已分配的内存
bool MemoryManager::randomlyDeallocate() {
    std::vector<int> allocatedIndices;
    for (int i = 0; i < memory.size(); ++i) {
        if (!memory[i].isFree) {
            allocatedIndices.push_back(i);
        }
    }

    if (allocatedIndices.empty()) {
        return false; // 没有已分配的内存块
    }

    // 随机选择一个已分配的块
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, allocatedIndices.size() - 1);
    int randomIndex = allocatedIndices[dist(gen)];

    // 释放该块
    memory[randomIndex].isFree = true;
    mergeFreeBlocks();
    std::cout << "内存不足！随机释放了起始页为 " << memory[randomIndex].start << " 的内存块。\n";
    return true;
}

// 测试函数实现
void Memorytest() {
    try {
        // 全局变量
        std::mutex globalMutex;
        std::condition_variable cv;
        bool isAllocating = true; // 控制线程同步

        // 用户内存容量（4页到32页）
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> userMemoryDist(16, 32); // 用户内存总页数
        int totalPages = userMemoryDist(gen);

        // 请求序列（随机生成分配和释放操作）
        std::uniform_int_distribution<> requestSizeDist(1, 8); // 每次请求的页数范围
        int numRequests = 10; // 总请求数量
        std::vector<int> requestSequence(numRequests);
        for (int i = 0; i < numRequests; ++i) {
            requestSequence[i] = requestSizeDist(gen);
        }

        // 用户选择内存管理方式
        std::cout << "选择内存管理方式 (1: 首次适应, 2: 最佳适应): ";
        int choice;
        std::cin >> choice;
        MemoryManager::AllocationMethod method = MemoryManager::AllocationMethod::FirstFit;
        if (choice == 2) {
            method = MemoryManager::AllocationMethod::BestFit;
        }

        MemoryManager manager(totalPages, method);

        // 内存分配线程
        std::thread allocThread([&manager, &requestSequence, &globalMutex, &cv, &isAllocating]() {
            for (int pages : requestSequence) {
                {
                    std::unique_lock<std::mutex> lock(globalMutex);
                    cv.wait(lock, [&isAllocating] { return isAllocating; });
                    std::cout << "尝试分配 " << pages << " 页内存...\n";
                    bool success = manager.allocate(pages);
                    if (success) {
                        std::cout << "成功分配 " << pages << " 页内存。\n";
                    }
                    else {
                        std::cout << "分配失败！内存不足。\n";
                    }
                    manager.printMemoryState(); // 打印内存状态
                    isAllocating = false; // 切换到跟踪线程
                }
                cv.notify_one();
                std::this_thread::sleep_for(std::chrono::milliseconds(500)); // 模拟延迟
            }
            });

        // 跟踪内存状态线程
        std::thread trackThreadObj([&manager, &globalMutex, &cv, &isAllocating, numRequests]() {
            for (int i = 0; i < numRequests; ++i) {
                {
                    std::unique_lock<std::mutex> lock(globalMutex);
                    cv.wait(lock, [&isAllocating] { return !isAllocating; });
                    std::cout << "跟踪内存状态...\n";
                    manager.printMemoryState(); // 打印内存状态
                    isAllocating = true; // 切换回分配线程
                }
                cv.notify_one();
                std::this_thread::sleep_for(std::chrono::milliseconds(500)); // 模拟延迟
            }
            });

        // 等待线程完成
        allocThread.join();
        trackThreadObj.join();

        std::cout << "模拟完成！\n";
    }
    catch (const std::exception& e) {
        std::cerr << "发生错误: " << e.what() << "\n";
    }
}