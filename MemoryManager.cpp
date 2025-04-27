#include "MemoryManager.h"
#include "file.h"
#include <algorithm>
#include <random>
#include <stdexcept>
#include <string>
#include <iomanip>
#include <fstream>

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
    int blockNumber = -1;

    switch (allocationStrategy) {
    case AllocationStrategy::FirstFit:
        blockNumber = allocateWithFirstFit();
        if (blockNumber != -1) {
            std::cout << "使用First-Fit策略分配了物理块 " << blockNumber << std::endl;
        }
        break;

    case AllocationStrategy::BestFit:
        blockNumber = allocateWithBestFit();
        if (blockNumber != -1) {
            std::cout << "使用Best-Fit策略分配了物理块 " << blockNumber << std::endl;
        }
        break;

    case AllocationStrategy::WorstFit:
        blockNumber = allocateWithWorstFit();
        if (blockNumber != -1) {
            std::cout << "使用Worst-Fit策略分配了物理块 " << blockNumber << std::endl;
        }
        break;

    case AllocationStrategy::NextFit:
        blockNumber = allocateWithNextFit();
        if (blockNumber != -1) {
            std::cout << "使用Next-Fit策略分配了物理块 " << blockNumber << std::endl;
        }
        break;
    }

    return blockNumber;
}

// FirstFit策略实现
int MemoryManager::allocateWithFirstFit() {
    for (int i = 0; i < MEMORY_BLOCKS; ++i) {
        if (!memory[i].isAllocated) {
            memory[i].isAllocated = true;
            return i;
        }
    }
    return -1;  // 没有空闲块
}

// BestFit策略实现
int MemoryManager::allocateWithBestFit() {
    // 对于固定大小的块，BestFit等同于FirstFit
    return allocateWithFirstFit();

    // 如果块大小不同，则实现如下
    /*
    int bestBlock = -1;
    int minSize = INT_MAX;

    for (int i = 0; i < MEMORY_BLOCKS; ++i) {
        if (!memory[i].isAllocated) {
            // 假设我们有一个计算块大小的方式
            int blockSize = getBlockSize(i);
            if (blockSize < minSize) {
                minSize = blockSize;
                bestBlock = i;
            }
        }
    }

    if (bestBlock != -1) {
        memory[bestBlock].isAllocated = true;
    }

    return bestBlock;
    */
}

// WorstFit策略实现
int MemoryManager::allocateWithWorstFit() {
    int worstBlock = -1;
    int maxSize = -1;

    // 找出最大的空闲块
    for (int i = 0; i < MEMORY_BLOCKS; ++i) {
        if (!memory[i].isAllocated) {
            // 计算连续空闲块大小
            int size = 1;
            int j = i + 1;
            while (j < MEMORY_BLOCKS && !memory[j].isAllocated) {
                size++;
                j++;
            }

            if (size > maxSize) {
                maxSize = size;
                worstBlock = i;
            }

            // 跳过已计算的空闲块
            i = j - 1;
        }
    }

    if (worstBlock != -1) {
        memory[worstBlock].isAllocated = true;
    }

    return worstBlock;
}

// NextFit策略实现
int MemoryManager::allocateWithNextFit() {
    // 从上次分配位置开始查找
    if (lastAllocatedBlock == -1) {
        lastAllocatedBlock = 0;  // 初始化为0
    }

    // 从lastAllocatedBlock开始到末尾查找
    for (int i = lastAllocatedBlock; i < MEMORY_BLOCKS; ++i) {
        if (!memory[i].isAllocated) {
            memory[i].isAllocated = true;
            lastAllocatedBlock = i + 1;  // 更新下一次查找的起始位置
            if (lastAllocatedBlock >= MEMORY_BLOCKS) {
                lastAllocatedBlock = 0;  // 循环回到开始
            }
            return i;
        }
    }

    // 如果未找到，从开始到lastAllocatedBlock查找
    for (int i = 0; i < lastAllocatedBlock; ++i) {
        if (!memory[i].isAllocated) {
            memory[i].isAllocated = true;
            lastAllocatedBlock = i + 1;
            return i;
        }
    }

    return -1;  // 没有空闲块
}

// 释放一个物理块
void MemoryManager::freePhysicalBlock(int physicalBlockNumber) {
    if (physicalBlockNumber < 0 || physicalBlockNumber >= MEMORY_BLOCKS) {
        throw std::out_of_range("Invalid physical block number");
    }

    // 如果有映射的虚拟页号，将数据写回逻辑块
    if (memory[physicalBlockNumber].virtualPageNumber != -1) {
        std::cout << "Writing data back to logical block " << memory[physicalBlockNumber].virtualPageNumber << std::endl;
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
        //std::cout << "Virtual page " << virtualPageNumber << " not found in page table.\n";
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

    // 更新LRU列表
    if (replacementAlgorithm == ReplacementAlgorithm::LRU) {
        auto lruIt = std::find(lruList.begin(), lruList.end(), physicalBlockNumber);
        if (lruIt != lruList.end()) {
            lruList.erase(lruIt);
            lruList.push_back(physicalBlockNumber);  // 移到最后表示最近使用
        }
    }

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

// 打印内存使用统计信息
void MemoryManager::printMemoryStatistics() const {
    int allocatedBlocks = 0;
    int freeBlocks = 0;
    int fragmentCount = 0;
    int totalFragmentSize = 0;
    int maxFragmentSize = 0;
    int minFragmentSize = MEMORY_BLOCKS;
    bool inFragment = false;
    int currentFragmentSize = 0;

    for (int i = 0; i < MEMORY_BLOCKS; ++i) {
        if (memory[i].isAllocated) {
            allocatedBlocks++;
            if (inFragment) {
                inFragment = false;
                fragmentCount++;
                totalFragmentSize += currentFragmentSize;
                maxFragmentSize = std::max(maxFragmentSize, currentFragmentSize);
                minFragmentSize = std::min(minFragmentSize, currentFragmentSize);
                currentFragmentSize = 0;
            }
        }
        else {
            freeBlocks++;
            if (!inFragment) {
                inFragment = true;
                currentFragmentSize = 1;
            }
            else {
                currentFragmentSize++;
            }
        }
    }

    // 检查末尾的片段
    if (inFragment) {
        fragmentCount++;
        totalFragmentSize += currentFragmentSize;
        maxFragmentSize = std::max(maxFragmentSize, currentFragmentSize);
        minFragmentSize = std::min(minFragmentSize, currentFragmentSize);
    }

    double fragPercent = 0.0;
    double avgFragSize = 0.0;

    if (fragmentCount > 0) {
        avgFragSize = static_cast<double>(totalFragmentSize) / fragmentCount;
        fragPercent = (1.0 - static_cast<double>(minFragmentSize) / maxFragmentSize) * 100.0;
    }
    else {
        minFragmentSize = 0; // 没有碎片时设为0
    }

    std::cout << "\n===== 内存使用统计 =====\n";
    std::cout << "总内存块数: " << MEMORY_BLOCKS << std::endl;
    std::cout << "已分配块数: " << allocatedBlocks << " ("
        << (static_cast<double>(allocatedBlocks) / MEMORY_BLOCKS * 100) << "%)" << std::endl;
    std::cout << "空闲块数: " << freeBlocks << " ("
        << (static_cast<double>(freeBlocks) / MEMORY_BLOCKS * 100) << "%)" << std::endl;
    std::cout << "空闲片段数: " << fragmentCount << std::endl;

    if (fragmentCount > 0) {
        std::cout << "平均每片段大小: " << avgFragSize << " 块" << std::endl;
        std::cout << "最大空闲片段: " << maxFragmentSize << " 块" << std::endl;
        std::cout << "最小空闲片段: " << minFragmentSize << " 块" << std::endl;
        std::cout << "碎片化程度: " << fragPercent << "%" << std::endl;
    }
}

// 设置调度算法
void MemoryManager::setReplacementAlgorithm(ReplacementAlgorithm algorithm) {
    replacementAlgorithm = algorithm;
}

// 从磁盘读取块数据
char* MemoryManager::readBlock(int blockNumber) {
    // 这里应该实现从磁盘读取数据的代码
    // 为简化示例，我们只返回一个随机生成的数据块
    static char data[BLOCK_SIZE];

    // 生成A-Z的随机字符
    for (int i = 0; i < BLOCK_SIZE; ++i) {
        data[i] = 'A' + (blockNumber * 13 + i) % 26;
    }

    return data;
}

// 将块数据写回磁盘
void MemoryManager::writeBlock(const char* data, int blockNumber) {
    // 这里应该实现将数据写回磁盘的代码
    // 在实际应用中，这会涉及到文件IO操作
    // 在本示例中我们不执行实际写入
}

// 内存碎片整理
void MemoryManager::defragmentMemory() {
    std::cout << "\n--- 开始内存碎片整理 ---\n";

    // 检查是否需要碎片整理
    int freeBlocks = 0;
    for (int i = 0; i < MEMORY_BLOCKS; ++i) {
        if (!memory[i].isAllocated) {
            freeBlocks++;
        }
    }

    if (freeBlocks <= 1) {
        std::cout << "无需碎片整理，空闲块数不足或已经连续。\n";
        return;
    }

    // 收集已分配块的信息
    std::vector<std::pair<int, int>> allocatedBlocks; // <虚拟页号, 物理块号>
    for (const auto& entry : pageTable) {
        allocatedBlocks.push_back({ entry.first, entry.second });
    }

    // 按物理块号排序，以便移动
    std::sort(allocatedBlocks.begin(), allocatedBlocks.end(),
        [](const auto& a, const auto& b) { return a.second < b.second; });

    // 计算整理前的碎片情况
    printMemoryStatistics();

    // 执行碎片整理
    int nextFreeBlock = 0;
    std::vector<std::pair<int, int>> blockMoves; // <虚拟页, 旧物理块->新物理块>

    for (auto& pair : allocatedBlocks) {
        int virtualPage = pair.first;
        int oldPhysicalBlock = pair.second;

        // 跳过已经在低端的块
        if (oldPhysicalBlock == nextFreeBlock) {
            nextFreeBlock++;
            continue;
        }

        // 找到一个要移动的块
        while (nextFreeBlock < MEMORY_BLOCKS && memory[nextFreeBlock].isAllocated) {
            nextFreeBlock++;
        }

        if (nextFreeBlock >= MEMORY_BLOCKS) {
            break; // 没有更多空闲块了
        }

        // 记录移动信息
        blockMoves.push_back({ virtualPage, oldPhysicalBlock });

        // 将虚拟页的数据从旧物理块移动到新物理块
        std::memcpy(memory[nextFreeBlock].data, memory[oldPhysicalBlock].data, BLOCK_SIZE);
        memory[nextFreeBlock].isAllocated = true;
        memory[nextFreeBlock].virtualPageNumber = virtualPage;

        // 更新页表
        pageTable[virtualPage] = nextFreeBlock;

        // 释放旧物理块
        memory[oldPhysicalBlock].isAllocated = false;
        memory[oldPhysicalBlock].virtualPageNumber = -1;

        // 更新FIFO队列和LRU列表
        auto fifoIt = std::find(fifoQueue.begin(), fifoQueue.end(), oldPhysicalBlock);
        if (fifoIt != fifoQueue.end()) {
            *fifoIt = nextFreeBlock;
        }

        auto lruIt = std::find(lruList.begin(), lruList.end(), oldPhysicalBlock);
        if (lruIt != lruList.end()) {
            *lruIt = nextFreeBlock;
        }

        nextFreeBlock++;
    }

    // 打印移动信息
    std::cout << "\n共移动了 " << blockMoves.size() << " 个内存块：\n";
    for (const auto& move : blockMoves) {
        std::cout << "虚拟页 " << move.first
            << " 从物理块 " << move.second
            << " 移动到物理块 " << pageTable[move.first] << "\n";
    }

    std::cout << "\n--- 碎片整理完成 ---\n";

    // 显示整理后的内存状态和统计信息
    printMemoryState();
    printMemoryStatistics();
}
void MemoryManager::MemoryManagerTest() {
    // 添加选择页面置换算法的功能
    int replacementChoice;
    std::cout << "\n--- 请选择页面置换算法 ---\n";
    std::cout << "1: FIFO (先进先出)\n";
    std::cout << "2: LRU (最近最少使用)\n";
    std::cout << "3: Random (随机置换)\n";
    std::cout << "请输入选择: ";
    std::cin >> replacementChoice;

    MemoryManager::ReplacementAlgorithm algorithm;
    switch (replacementChoice) {
    case 1:
        algorithm = MemoryManager::ReplacementAlgorithm::FIFO;
        std::cout << "已选择 FIFO 页面置换算法\n";
        break;
    case 2:
        algorithm = MemoryManager::ReplacementAlgorithm::LRU;
        std::cout << "已选择 LRU 页面置换算法\n";
        break;
    case 3:
        algorithm = MemoryManager::ReplacementAlgorithm::Random;
        std::cout << "已选择 Random 页面置换算法\n";
        break;
    default:
        algorithm = MemoryManager::ReplacementAlgorithm::FIFO;
        std::cout << "无效选择，使用默认 FIFO 算法\n";
    }

    // 使用选择的页面置换算法创建内存管理器
    MemoryManager manager(algorithm);

    // 添加选择内存分配策略的功能
    int strategyChoice;
    std::cout << "\n--- 请选择内存分配策略 ---\n";
    std::cout << "1: First-Fit (首次适应)\n";
    std::cout << "2: Best-Fit (最佳适应)\n";
    std::cout << "3: Worst-Fit (最差适应)\n";
    std::cout << "4: Next-Fit (下次适应)\n";
    std::cout << "请输入选择: ";
    std::cin >> strategyChoice;

    // 设置所选的分配策略
    switch (strategyChoice) {
    case 1:
        manager.setAllocationStrategy(MemoryManager::AllocationStrategy::FirstFit);
        std::cout << "已选择 First-Fit 策略\n";
        break;
    case 2:
        manager.setAllocationStrategy(MemoryManager::AllocationStrategy::BestFit);
        std::cout << "已选择 Best-Fit 策略\n";
        break;
    case 3:
        manager.setAllocationStrategy(MemoryManager::AllocationStrategy::WorstFit);
        std::cout << "已选择 Worst-Fit 策略\n";
        break;
    case 4:
        manager.setAllocationStrategy(MemoryManager::AllocationStrategy::NextFit);
        std::cout << "已选择 Next-Fit 策略\n";
        break;
    default:
        std::cout << "无效选择，使用默认 First-Fit 策略\n";
        manager.setAllocationStrategy(MemoryManager::AllocationStrategy::FirstFit);
    }

    // 尝试分配几个页面，创造一些碎片
    std::cout << "\n--- 创建内存碎片场景 ---" << std::endl;
    // 分配3个虚拟页
    for (int i = 0; i < 3; ++i) {
        manager.allocateMemory(i);
    }

    // 打印内存状态
    manager.printMemoryState();
    // 打印内存统计信息
    manager.printMemoryStatistics();

    // 释放中间的页，制造碎片
    std::cout << "\n--- 释放虚拟页1，制造碎片 ---" << std::endl;
    manager.freeMemory(1);
    manager.printMemoryState();
    // 打印碎片化后的内存统计信息
    manager.printMemoryStatistics();

    // 测试内存分配
    std::cout << "\n--- 测试选定的内存分配策略 ---" << std::endl;
    // 分配新页面，应该使用选定的分配策略
    manager.allocateMemory(3);

    // 打印最终内存状态
    manager.printMemoryState();
    // 打印分配后的内存统计信息
    manager.printMemoryStatistics();

    // 添加更多的测试，特别是对Next-Fit策略的测试
    std::cout << "\n--- 继续测试分配策略 ---" << std::endl;
    // 再释放一个页面
    manager.freeMemory(0);
    std::cout << "释放虚拟页0后的内存状态：" << std::endl;
    manager.printMemoryState();
    manager.printMemoryStatistics();

    // 分配两个新页面，观察分配模式
    std::cout << "分配两个新页面：" << std::endl;
    manager.allocateMemory(4);
    manager.allocateMemory(5);
    manager.printMemoryState();
    // 打印最终分配后的内存统计信息
    manager.printMemoryStatistics();

    // 尝试触发页面置换算法的测试
    std::cout << "\n--- 测试页面置换算法 ---" << std::endl;
    // 模拟内存不足的情况，通过填满剩余内存然后尝试分配新页面
    int filledPages = 0;
    std::cout << "填充剩余内存空间..." << std::endl;
    for (int i = 6; i < 6 + MEMORY_BLOCKS; ++i) {
        bool result = manager.allocateMemory(i);
        if (!result) {
            std::cout << "内存已满，无法分配更多页面。共填充了 " << filledPages << " 页。" << std::endl;
            break;
        }
        filledPages++;
    }
    manager.printMemoryState();
    manager.printMemoryStatistics();

    // 尝试分配新页面，触发页面置换
    std::cout << "尝试分配新页面，预期会触发页面置换..." << std::endl;
    bool replacementTriggered = manager.allocateMemory(100);
    std::cout << "页面分配 " << (replacementTriggered ? "成功" : "失败") << std::endl;
    if (replacementTriggered) {
        std::cout << "页面置换被触发，检查哪个页面被替换了" << std::endl;
        manager.printMemoryState();
        manager.printMemoryStatistics();
    }

    // 添加更多测试用例来测试内存碎片整理功能
    std::cout << "\n--- 测试内存碎片整理 ---" << std::endl;
    // 先手动制造更多的不连续碎片
    std::cout << "手动释放几个不连续的页面来制造更多碎片..." << std::endl;
    manager.freeMemory(8);
    manager.freeMemory(10);
    manager.freeMemory(12);
    manager.freeMemory(14);
    manager.freeMemory(16);

    std::cout << "制造碎片后的内存状态：" << std::endl;
    manager.printMemoryState();
    manager.printMemoryStatistics();

    // 执行内存碎片整理
    std::cout << "\n执行内存碎片整理..." << std::endl;
    manager.defragmentMemory();

    // 原有的测试代码
    try {
        char data = manager.getData(3, 10);
        std::cout << "Data at logical block 3, offset 10: " << data << "\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }

    // 最终内存使用情况统计
    std::cout << "\n--- 测试结束前的内存使用情况 ---" << std::endl;
    manager.printMemoryStatistics();

    // 释放所有内存
    std::cout << "\n--- 释放所有内存 ---" << std::endl;
    for (int i = 0; i <= 100; ++i) {
        manager.freeMemory(i);
    }

    // 再次打印内存状态
    manager.printMemoryState();
    // 打印释放后的内存统计信息
    manager.printMemoryStatistics();

    return;
}