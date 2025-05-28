#include "MemoryManager.h"
#include "file.h"
#include <algorithm>
#include <random>
#include <stdexcept>
#include <string>
#include <iomanip>
#include <fstream>

// ���캯��
MemoryManager::MemoryManager(ReplacementAlgorithm algorithm)
    : replacementAlgorithm(algorithm), memory(MEMORY_BLOCKS) {
    for (auto& block : memory) {
        block.isAllocated = false;
        block.virtualPageNumber = -1; // ��ʼ��Ϊ��Чֵ
    }
}

// ��������
MemoryManager::~MemoryManager() {}

// ����һ�������
int MemoryManager::allocatePhysicalBlock() {
    int blockNumber = -1;

    switch (allocationStrategy) {
    case AllocationStrategy::FirstFit:
        blockNumber = allocateWithFirstFit();
        if (blockNumber != -1) {
            std::cout << "ʹ��First-Fit���Է���������� " << blockNumber << std::endl;
        }
        break;

    case AllocationStrategy::BestFit:
        blockNumber = allocateWithBestFit();
        if (blockNumber != -1) {
            std::cout << "ʹ��Best-Fit���Է���������� " << blockNumber << std::endl;
        }
        break;

    case AllocationStrategy::WorstFit:
        blockNumber = allocateWithWorstFit();
        if (blockNumber != -1) {
            std::cout << "ʹ��Worst-Fit���Է���������� " << blockNumber << std::endl;
        }
        break;

    case AllocationStrategy::NextFit:
        blockNumber = allocateWithNextFit();
        if (blockNumber != -1) {
            std::cout << "ʹ��Next-Fit���Է���������� " << blockNumber << std::endl;
        }
        break;
    case AllocationStrategy::QuickFit:  // ������һ��
        blockNumber = allocateWithQuickFit();
        break;
    }

    return blockNumber;
}

// FirstFit����ʵ��
int MemoryManager::allocateWithFirstFit() {
    for (int i = 0; i < MEMORY_BLOCKS; ++i) {
        if (!memory[i].isAllocated) {
            memory[i].isAllocated = true;
            return i;
        }
    }
    return -1;  // û�п��п�
}

// BestFit����ʵ��
int MemoryManager::allocateWithBestFit() {
    // ���ڹ̶���С�Ŀ飬BestFit��ͬ��FirstFit
    return allocateWithFirstFit();

    // ������С��ͬ����ʵ������
    /*
    int bestBlock = -1;
    int minSize = INT_MAX;

    for (int i = 0; i < MEMORY_BLOCKS; ++i) {
        if (!memory[i].isAllocated) {
            // ����������һ��������С�ķ�ʽ
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

// WorstFit����ʵ��
int MemoryManager::allocateWithWorstFit() {
    int worstBlock = -1;
    int maxSize = -1;

    // �ҳ����Ŀ��п�
    for (int i = 0; i < MEMORY_BLOCKS; ++i) {
        if (!memory[i].isAllocated) {
            // �����������п��С
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

            // �����Ѽ���Ŀ��п�
            i = j - 1;
        }
    }

    if (worstBlock != -1) {
        memory[worstBlock].isAllocated = true;
    }

    return worstBlock;
}

// NextFit����ʵ��
int MemoryManager::allocateWithNextFit() {
    // ���ϴη���λ�ÿ�ʼ����
    if (lastAllocatedBlock == -1) {
        lastAllocatedBlock = 0;  // ��ʼ��Ϊ0
    }

    // ��lastAllocatedBlock��ʼ��ĩβ����
    for (int i = lastAllocatedBlock; i < MEMORY_BLOCKS; ++i) {
        if (!memory[i].isAllocated) {
            memory[i].isAllocated = true;
            lastAllocatedBlock = i + 1;  // ������һ�β��ҵ���ʼλ��
            if (lastAllocatedBlock >= MEMORY_BLOCKS) {
                lastAllocatedBlock = 0;  // ѭ���ص���ʼ
            }
            return i;
        }
    }

    // ���δ�ҵ����ӿ�ʼ��lastAllocatedBlock����
    for (int i = 0; i < lastAllocatedBlock; ++i) {
        if (!memory[i].isAllocated) {
            memory[i].isAllocated = true;
            lastAllocatedBlock = i + 1;
            return i;
        }
    }

    return -1;  // û�п��п�
}


// �ͷ�һ�������
void MemoryManager::freePhysicalBlock(int physicalBlockNumber) {
    if (physicalBlockNumber < 0 || physicalBlockNumber >= MEMORY_BLOCKS) {
        throw std::out_of_range("Invalid physical block number");
    }

    // �����ӳ�������ҳ�ţ�������д���߼���
    if (memory[physicalBlockNumber].virtualPageNumber != -1) {
        std::cout << "Writing data back to logical block " << memory[physicalBlockNumber].virtualPageNumber << std::endl;
        writeBlock(memory[physicalBlockNumber].data, memory[physicalBlockNumber].virtualPageNumber);
    }

    memory[physicalBlockNumber].isAllocated = false;
    memory[physicalBlockNumber].virtualPageNumber = -1;
}

//// ҳ���û��㷨
//void MemoryManager::replacePage() {
//    int physicalBlockNumber = -1;
//
//    switch (replacementAlgorithm) {
//    case ReplacementAlgorithm::FIFO: {
//        if (fifoQueue.empty()) {
//            throw std::runtime_error("No pages to replace in FIFO queue.");
//        }
//        physicalBlockNumber = fifoQueue.front();
//        fifoQueue.pop_front();
//        break;
//    }
//    case ReplacementAlgorithm::LRU: {
//        if (lruList.empty()) {
//            throw std::runtime_error("No pages to replace in LRU list.");
//        }
//        physicalBlockNumber = lruList.front();
//        lruList.erase(lruList.begin());
//        break;
//    }
//    case ReplacementAlgorithm::Random: {
//        std::random_device rd;
//        std::mt19937 gen(rd());
//        std::uniform_int_distribution<> dist(0, MEMORY_BLOCKS - 1);
//        do {
//            physicalBlockNumber = dist(gen);
//        } while (!memory[physicalBlockNumber].isAllocated);
//        break;
//    }
//    }
//
//    // �ҵ���Ӧ������ҳ�Ų���ҳ�����Ƴ�
//    int virtualPageNumber = memory[physicalBlockNumber].virtualPageNumber;
//    if (virtualPageNumber != -1) {
//        pageTable.erase(virtualPageNumber);
//    }
//
//    // �ͷ������
//    freePhysicalBlock(physicalBlockNumber);
//}
void MemoryManager::replacePage() {
    int physicalBlockNumber = -1;

    switch (replacementAlgorithm) {
    case ReplacementAlgorithm::FIFO: {
        if (fifoQueue.empty()) {
            std::cout << "No pages available for replacement (FIFO).\n";
            return;
        }
        physicalBlockNumber = fifoQueue.front();
        fifoQueue.pop_front();
        break;
    }
    case ReplacementAlgorithm::LRU: {
        if (lruList.empty()) {
            std::cout << "No pages available for replacement (LRU).\n";
            return;
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

    if (physicalBlockNumber != -1) {
        int virtualPageNumber = memory[physicalBlockNumber].virtualPageNumber;
        if (virtualPageNumber != -1) {
            pageTable.erase(virtualPageNumber);
        }
        freePhysicalBlock(physicalBlockNumber);
    }
}
// �����ڴ������ҳ��
bool MemoryManager::allocateMemory(int virtualPageNumber) {
    // �������ҳ���Ѿ����ڣ�ֱ�ӷ���
    if (pageTable.find(virtualPageNumber) != pageTable.end()) {
        return true;
    }

    int physicalBlockNumber = allocatePhysicalBlock();
    if (physicalBlockNumber == -1) {
        // �ڴ治�㣬����ҳ���û�
        replacePage();
        physicalBlockNumber = allocatePhysicalBlock();
        if (physicalBlockNumber == -1) {
            std::cout << "Memory allocation failed after page replacement.\n";
            return false;
        }
    }

    // ����ҳ��
    pageTable[virtualPageNumber] = physicalBlockNumber;

    // ���µ��ȶ��л��б�
    fifoQueue.push_back(physicalBlockNumber);
    lruList.push_back(physicalBlockNumber);

    // ���߼����ȡ���ݵ������
    char* data = readBlock(virtualPageNumber);
    std::memcpy(memory[physicalBlockNumber].data, data, BLOCK_SIZE);

    // �����������Ϣ
    memory[physicalBlockNumber].virtualPageNumber = virtualPageNumber;

    std::cout << "Allocated physical block " << physicalBlockNumber
        << " to virtual page " << virtualPageNumber << "\n";
    return true;
}

// �ͷ��ڴ�
void MemoryManager::freeMemory(int virtualPageNumber) {
    auto it = pageTable.find(virtualPageNumber);
    if (it == pageTable.end()) {
        //std::cout << "Virtual page " << virtualPageNumber << " not found in page table.\n";
        return;
    }

    int physicalBlockNumber = it->second;

    // �ͷ������
    freePhysicalBlock(physicalBlockNumber);

    // ��ҳ�����Ƴ�
    pageTable.erase(it);

    // �ӵ��ȶ������Ƴ�
    fifoQueue.erase(std::remove(fifoQueue.begin(), fifoQueue.end(), physicalBlockNumber), fifoQueue.end());
    lruList.erase(std::remove(lruList.begin(), lruList.end(), physicalBlockNumber), lruList.end());

    std::cout << "Freed physical block " << physicalBlockNumber << " from virtual page " << virtualPageNumber << "\n";
}

// ��ȡָ���߼���ź�ƫ����������
char MemoryManager::getData(int logicalBlockNumber, int offset) {
    // ���ƫ�����Ƿ���Ч
    if (offset < 0 || offset >= BLOCK_SIZE) {
        throw std::out_of_range("Invalid offset: " + std::to_string(offset));
    }

    // ����߼�����Ƿ����ڴ���
    auto it = pageTable.find(logicalBlockNumber);
    if (it == pageTable.end()) {
        std::cout << "Page fault: Logical block " << logicalBlockNumber << " not in memory.\n";

        // �����߼��鵽�ڴ�
        if (!allocateMemory(logicalBlockNumber)) {
            throw std::runtime_error("Failed to load logical block into memory.");
        }

        // �ٴβ���������
        it = pageTable.find(logicalBlockNumber);
        if (it == pageTable.end()) {
            throw std::runtime_error("Logical block still not in memory after allocation.");
        }
    }

    int physicalBlockNumber = it->second;

    // ����LRU�б�
    if (replacementAlgorithm == ReplacementAlgorithm::LRU) {
        auto lruIt = std::find(lruList.begin(), lruList.end(), physicalBlockNumber);
        if (lruIt != lruList.end()) {
            lruList.erase(lruIt);
            lruList.push_back(physicalBlockNumber);  // �Ƶ�����ʾ���ʹ��
        }
    }

    // ����ָ��ƫ����������
    return memory[physicalBlockNumber].data[offset];
}
void MemoryManager::allocatePCBmemory(string pname, int pid) {
    std::cout << "[Memory] Allocating memory for process '" << pname << "' (PID: " << pid << ")" << std::endl;

    // ����ÿ��������Ҫһ���ڴ����Ϊ������ʱ�ڴ�
    int virtualPage = pid;  // ʹ�� PID ��Ϊ����ҳ�ţ�����չ��
    bool success = allocateMemory(virtualPage);

    if (success) {
        std::cout << "[Memory] Process '" << pname << "' allocated virtual page " << virtualPage << std::endl;
    }
    else {
        std::cerr << "[Memory] Failed to allocate memory for process '" << pname << "'" << std::endl;
    }
    return;
}
void MemoryManager::deletePCBmomory(string pname, int pid) {
    std::cout << "[Memory] Releasing memory for process '" << pname << "' (PID: " << pid << ")" << std::endl;

    int virtualPage = pid;  // ͬ��ʹ�� PID ��Ϊ����ҳ��
    auto it = pageTable.find(virtualPage);

    if (it != pageTable.end()) {
        int physicalBlock = it->second;

        // ��ҳ����ɾ��ӳ��
        pageTable.erase(it);

        // �ͷ������ڴ��
        memory[physicalBlock].isAllocated = false;
        memory[physicalBlock].virtualPageNumber = -1;

        // ��ո��ڴ�����ݣ���ѡ��
        memset(memory[physicalBlock].data, 0, BLOCK_SIZE);

        // ���� FIFO/LRU �Ƚṹ
        fifoQueue.erase(std::remove(fifoQueue.begin(), fifoQueue.end(), physicalBlock), fifoQueue.end());
        lruList.erase(std::remove(lruList.begin(), lruList.end(), physicalBlock), lruList.end());

        std::cout << "[Memory] Memory for PID " << pid << " released." << std::endl;
    }
    else {
        std::cerr << "[Memory] No allocated memory found for PID " << pid << std::endl;
    }
    return;
}
// ��ӡ�ڴ�״̬
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

// ��ӡ�ڴ�ʹ��ͳ����Ϣ
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

    // ���ĩβ��Ƭ��
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
        minFragmentSize = 0; // û����Ƭʱ��Ϊ0
    }

    std::cout << "\n===== �ڴ�ʹ��ͳ�� =====\n";
    std::cout << "���ڴ����: " << MEMORY_BLOCKS << std::endl;
    std::cout << "�ѷ������: " << allocatedBlocks << " ("
        << (static_cast<double>(allocatedBlocks) / MEMORY_BLOCKS * 100) << "%)" << std::endl;
    std::cout << "���п���: " << freeBlocks << " ("
        << (static_cast<double>(freeBlocks) / MEMORY_BLOCKS * 100) << "%)" << std::endl;
    std::cout << "����Ƭ����: " << fragmentCount << std::endl;

    if (fragmentCount > 0) {
        std::cout << "ƽ��ÿƬ�δ�С: " << avgFragSize << " ��" << std::endl;
        std::cout << "������Ƭ��: " << maxFragmentSize << " ��" << std::endl;
        std::cout << "��С����Ƭ��: " << minFragmentSize << " ��" << std::endl;
        std::cout << "��Ƭ���̶�: " << fragPercent << "%" << std::endl;
    }
}

// ���õ����㷨
void MemoryManager::setReplacementAlgorithm(ReplacementAlgorithm algorithm) {
    replacementAlgorithm = algorithm;
}

// �Ӵ��̶�ȡ������
char* MemoryManager::readBlock(int blockNumber) {
    // ����Ӧ��ʵ�ִӴ��̶�ȡ���ݵĴ���
    // Ϊ��ʾ��������ֻ����һ��������ɵ����ݿ�
    static char data[BLOCK_SIZE];

    // ����A-Z������ַ�
    for (int i = 0; i < BLOCK_SIZE; ++i) {
        data[i] = 'A' + (blockNumber * 13 + i) % 26;
    }

    return data;
}

// ��������д�ش���
void MemoryManager::writeBlock(const char* data, int blockNumber) {

}

// �ڴ���Ƭ����
void MemoryManager::defragmentMemory() {
    std::cout << "\n--- ��ʼ�ڴ���Ƭ���� ---\n";

    // ����Ƿ���Ҫ��Ƭ����
    int freeBlocks = 0;
    for (int i = 0; i < MEMORY_BLOCKS; ++i) {
        if (!memory[i].isAllocated) {
            freeBlocks++;
        }
    }

    if (freeBlocks <= 1) {
        std::cout << "������Ƭ�������п���������Ѿ�������\n";
        return;
    }

    // �ռ��ѷ�������Ϣ
    std::vector<std::pair<int, int>> allocatedBlocks; // <����ҳ��, ������>
    for (const auto& entry : pageTable) {
        allocatedBlocks.push_back({ entry.first, entry.second });
    }

    // �������������Ա��ƶ�
    std::sort(allocatedBlocks.begin(), allocatedBlocks.end(),
        [](const auto& a, const auto& b) { return a.second < b.second; });

    // ��������ǰ����Ƭ���
    printMemoryStatistics();

    // ִ����Ƭ����
    int nextFreeBlock = 0;
    std::vector<std::pair<int, int>> blockMoves; // <����ҳ, �������->�������>

    for (auto& pair : allocatedBlocks) {
        int virtualPage = pair.first;
        int oldPhysicalBlock = pair.second;

        // �����Ѿ��ڵͶ˵Ŀ�
        if (oldPhysicalBlock == nextFreeBlock) {
            nextFreeBlock++;
            continue;
        }

        // �ҵ�һ��Ҫ�ƶ��Ŀ�
        while (nextFreeBlock < MEMORY_BLOCKS && memory[nextFreeBlock].isAllocated) {
            nextFreeBlock++;
        }

        if (nextFreeBlock >= MEMORY_BLOCKS) {
            break; // û�и�����п���
        }

        // ��¼�ƶ���Ϣ
        blockMoves.push_back({ virtualPage, oldPhysicalBlock });

        // ������ҳ�����ݴӾ�������ƶ����������
        std::memcpy(memory[nextFreeBlock].data, memory[oldPhysicalBlock].data, BLOCK_SIZE);
        memory[nextFreeBlock].isAllocated = true;
        memory[nextFreeBlock].virtualPageNumber = virtualPage;

        // ����ҳ��
        pageTable[virtualPage] = nextFreeBlock;

        // �ͷž������
        memory[oldPhysicalBlock].isAllocated = false;
        memory[oldPhysicalBlock].virtualPageNumber = -1;

        // ����FIFO���к�LRU�б�
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

    // ��ӡ�ƶ���Ϣ
    std::cout << "\n���ƶ��� " << blockMoves.size() << " ���ڴ�飺\n";
    for (const auto& move : blockMoves) {
        std::cout << "����ҳ " << move.first
            << " ������� " << move.second
            << " �ƶ�������� " << pageTable[move.first] << "\n";
    }

    std::cout << "\n--- ��Ƭ������� ---\n";

    // ��ʾ�������ڴ�״̬��ͳ����Ϣ
    printMemoryState();
    printMemoryStatistics();
}
//�����������ڴ�������
void MemoryManager::SearchingBasedTest() {

    // ���ѡ��ҳ���û��㷨�Ĺ���

    int replacementChoice;
    std::cout << "\n--- ��ѡ��ҳ���û��㷨 ---\n";
    std::cout << "1: FIFO (�Ƚ��ȳ�)\n";
    std::cout << "2: LRU (�������ʹ��)\n";
    std::cout << "3: Random (����û�)\n";
    std::cout << "������ѡ��: ";
    std::cin >> replacementChoice;

    MemoryManager::ReplacementAlgorithm algorithm;
    switch (replacementChoice) {
    case 1:
        algorithm = MemoryManager::ReplacementAlgorithm::FIFO;
        std::cout << "��ѡ�� FIFO ҳ���û��㷨\n";
        break;
    case 2:
        algorithm = MemoryManager::ReplacementAlgorithm::LRU;
        std::cout << "��ѡ�� LRU ҳ���û��㷨\n";
        break;
    case 3:
        algorithm = MemoryManager::ReplacementAlgorithm::Random;
        std::cout << "��ѡ�� Random ҳ���û��㷨\n";
        break;
    default:
        algorithm = MemoryManager::ReplacementAlgorithm::FIFO;
        std::cout << "��Чѡ��ʹ��Ĭ�� FIFO �㷨\n";
    }

    // ʹ��ѡ���ҳ���û��㷨�����ڴ������
    MemoryManager manager(algorithm);

    // ���ѡ���ڴ������ԵĹ���
    int strategyChoice;
    std::cout << "\n--- ��ѡ���ڴ������� ---\n";
    std::cout << "1: First-Fit (�״���Ӧ)\n";
    std::cout << "2: Best-Fit (�����Ӧ)\n";
    std::cout << "3: Worst-Fit (�����Ӧ)\n";
    std::cout << "4: Next-Fit (�´���Ӧ)\n";
    std::cout << "������ѡ��: ";
    std::cin >> strategyChoice;

    // ������ѡ�ķ������
    switch (strategyChoice) {
    case 1:
        manager.setAllocationStrategy(MemoryManager::AllocationStrategy::FirstFit);
        std::cout << "��ѡ�� First-Fit ����\n";
        break;
    case 2:
        manager.setAllocationStrategy(MemoryManager::AllocationStrategy::BestFit);
        std::cout << "��ѡ�� Best-Fit ����\n";
        break;
    case 3:
        manager.setAllocationStrategy(MemoryManager::AllocationStrategy::WorstFit);
        std::cout << "��ѡ�� Worst-Fit ����\n";
        break;
    case 4:
        manager.setAllocationStrategy(MemoryManager::AllocationStrategy::NextFit);
        std::cout << "��ѡ�� Next-Fit ����\n";
        break;
    default:
        std::cout << "��Чѡ��ʹ��Ĭ�� First-Fit ����\n";
        manager.setAllocationStrategy(MemoryManager::AllocationStrategy::FirstFit);
    }

    // ���Է��伸��ҳ�棬����һЩ��Ƭ
    std::cout << "\n--- �����ڴ���Ƭ���� ---" << std::endl;
    // ����3������ҳ
    for (int i = 0; i < 3; ++i) {
        manager.allocateMemory(i);
    }

    // ��ӡ�ڴ�״̬
    manager.printMemoryState();
    // ��ӡ�ڴ�ͳ����Ϣ
    manager.printMemoryStatistics();

    // �ͷ��м��ҳ��������Ƭ
    std::cout << "\n--- �ͷ�����ҳ1��������Ƭ ---" << std::endl;
    manager.freeMemory(1);
    manager.printMemoryState();
    // ��ӡ��Ƭ������ڴ�ͳ����Ϣ
    manager.printMemoryStatistics();

    // �����ڴ����
    std::cout << "\n--- ����ѡ�����ڴ������� ---" << std::endl;
    // ������ҳ�棬Ӧ��ʹ��ѡ���ķ������
    manager.allocateMemory(3);

    // ��ӡ�����ڴ�״̬
    manager.printMemoryState();
    // ��ӡ�������ڴ�ͳ����Ϣ
    manager.printMemoryStatistics();

    // ��Ӹ���Ĳ��ԣ��ر��Ƕ�Next-Fit���ԵĲ���
    std::cout << "\n--- �������Է������ ---" << std::endl;
    // ���ͷ�һ��ҳ��
    manager.freeMemory(0);
    std::cout << "�ͷ�����ҳ0����ڴ�״̬��" << std::endl;
    manager.printMemoryState();
    manager.printMemoryStatistics();

    // ����������ҳ�棬�۲����ģʽ
    std::cout << "����������ҳ�棺" << std::endl;
    manager.allocateMemory(4);
    manager.allocateMemory(5);
    manager.printMemoryState();
    // ��ӡ���շ������ڴ�ͳ����Ϣ
    manager.printMemoryStatistics();

    // ���Դ���ҳ���û��㷨�Ĳ���
    std::cout << "\n--- ����ҳ���û��㷨 ---" << std::endl;
    // ģ���ڴ治��������ͨ������ʣ���ڴ�Ȼ���Է�����ҳ��
    int filledPages = 0;
    std::cout << "���ʣ���ڴ�ռ�..." << std::endl;
    for (int i = 6; i < 6 + MEMORY_BLOCKS; ++i) {
        bool result = manager.allocateMemory(i);
        if (!result) {
            std::cout << "�ڴ��������޷��������ҳ�档������� " << filledPages << " ҳ��" << std::endl;
            break;
        }
        filledPages++;
    }
    manager.printMemoryState();
    manager.printMemoryStatistics();

    // ���Է�����ҳ�棬����ҳ���û�
    std::cout << "���Է�����ҳ�棬Ԥ�ڻᴥ��ҳ���û�..." << std::endl;
    bool replacementTriggered = manager.allocateMemory(100);
    std::cout << "ҳ����� " << (replacementTriggered ? "�ɹ�" : "ʧ��") << std::endl;
    if (replacementTriggered) {
        std::cout << "ҳ���û�������������ĸ�ҳ�汻�滻��" << std::endl;
        manager.printMemoryState();
        manager.printMemoryStatistics();
    }

    // ��Ӹ�����������������ڴ���Ƭ������
    std::cout << "\n--- �����ڴ���Ƭ���� ---" << std::endl;
    // ���ֶ��������Ĳ�������Ƭ
    std::cout << "�ֶ��ͷż�����������ҳ�������������Ƭ..." << std::endl;
    manager.freeMemory(8);
    manager.freeMemory(10);
    manager.freeMemory(12);
    manager.freeMemory(14);
    manager.freeMemory(16);

    std::cout << "������Ƭ����ڴ�״̬��" << std::endl;
    manager.printMemoryState();
    manager.printMemoryStatistics();

    // ִ���ڴ���Ƭ����
    std::cout << "\nִ���ڴ���Ƭ����..." << std::endl;
    manager.defragmentMemory();

    // ԭ�еĲ��Դ���
    try {
        char data = manager.getData(3, 10);
        std::cout << "Data at logical block 3, offset 10: " << data << "\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }

    // �����ڴ�ʹ�����ͳ��
    std::cout << "\n--- ���Խ���ǰ���ڴ�ʹ����� ---" << std::endl;
    manager.printMemoryStatistics();

    // �ͷ������ڴ�
    std::cout << "\n--- �ͷ������ڴ� ---" << std::endl;
    for (int i = 0; i <= 100; ++i) {
        manager.freeMemory(i);
    }

    // �ٴδ�ӡ�ڴ�״̬
    manager.printMemoryState();
    // ��ӡ�ͷź���ڴ�ͳ����Ϣ
    manager.printMemoryStatistics();

    return;
}
// �������䣨���ڿ��п�������
int MemoryManager::allocateWithQuickFit() {
    if (!quickFitInitialized) {
        // ��һ��ʹ�� QuickFit����ʼ�� freeBlocksMap
        for (int i = 0; i < MEMORY_BLOCKS; ++i) {
            if (!memory[i].isAllocated) {
                quickFitMap[1].push_back(i); // ����ÿ�����СΪ 1
            }
        }
        quickFitInitialized = true;
    }

    for (auto it = quickFitMap.begin(); it != quickFitMap.end(); ++it) {
        if (!it->second.empty()) {
            int blockNumber = it->second.back();
            it->second.pop_back();
            memory[blockNumber].isAllocated = true;
            return blockNumber;
        }
    }

    // ���û���ҵ����ÿ飬���˵� FirstFit
    int block = allocateWithFirstFit();
    if (block != -1) {
        quickFitMap[1].push_back(block);  // ���·���Ŀ�����б�
    }
    return block;
}
int MemoryManager::allocateWithBuddySystem(size_t sizeInBlocks) {
    if (sizeInBlocks > MEMORY_BLOCKS) return -1;

    int order = 0;
    while ((1 << order) < sizeInBlocks) ++order;

    for (int i = 0; i < buddyBlocks.size(); ++i) {
        if (!buddyBlocks[i].isAllocated && buddyBlocks[i].size >= (1 << order)) {
            while (buddyBlocks[i].size > (1 << order)) splitBlock(i);
            buddyBlocks[i].isAllocated = true;
            return i;
        }
    }
    return -1;
}

// Buddy System �ͷ�
void MemoryManager::freeWithBuddySystem(int blockIndex) {
    if (blockIndex >= 0 && blockIndex < buddyBlocks.size()) {
        buddyBlocks[blockIndex].isAllocated = false;
        mergeBuddies(blockIndex);
    }
}

// ���ѿ�
void MemoryManager::splitBlock(int index) {
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left >= buddyBlocks.size() || right >= buddyBlocks.size()) {
        // �� splitBlock �������ҵ������У�
        buddyBlocks.resize(std::max(buddyBlocks.size(), static_cast<size_t>(std::max(left, right)) + 1));
    }

    buddyBlocks[left] = { buddyBlocks[index].size / 2, false, -1, -1, index, right };
    buddyBlocks[right] = { buddyBlocks[index].size / 2, false, -1, -1, index, left };
    buddyBlocks[index].size /= 2;
}

// �ϲ����
void MemoryManager::mergeBuddies(int index) {
    int buddy = buddyBlocks[index].buddyIndex;
    int parent = buddyBlocks[index].parent;

    if (parent != -1 &&
        buddy != -1 &&
        !buddyBlocks[buddy].isAllocated &&
        buddyBlocks[buddy].size == buddyBlocks[index].size) {

        buddyBlocks[parent].leftChild = -1;
        buddyBlocks[parent].rightChild = -1;
        buddyBlocks[parent].size *= 2;
        buddyBlocks[parent].isAllocated = false;
        mergeBuddies(parent);
    }
}

//�����������ڴ���亯������
void MemoryManager::IndexBasedTest() {
    int choice;
    std::cout << "\n=== Index-Based Memory Management Test ===\n";
    std::cout << "1. Buddy System Allocation\n";
    std::cout << "2. Quick Fit Allocation\n";
    std::cout << "3. Exit\n";
    std::cout << "Enter your choice (1-3): ";
    std::cin >> choice;

    switch (choice) {
    case 1: {
        // ====== Buddy System Allocation ======
        std::cout << "\n--- Starting Buddy System Test ---\n";

        maxOrder = 0;
        int totalBlocks = MEMORY_BLOCKS;
        while ((1 << maxOrder) < totalBlocks) ++maxOrder;
        buddyBlocks.clear();
        buddyBlocks.resize(1 << (maxOrder + 1));
        buddyBlocks[0] = { totalBlocks, false, -1, -1, -1, -1 };

        std::vector<int> allocatedIndices;

        std::cout << "\n--- Allocating Blocks ---\n";
        int idx1 = allocateWithBuddySystem(4);
        std::cout << "[Allocation] Requested block size: 4, Assigned index: " << idx1 << "\n";

        int idx2 = allocateWithBuddySystem(2);
        std::cout << "[Allocation] Requested block size: 2, Assigned index: " << idx2 << "\n";

        int idx3 = allocateWithBuddySystem(1);
        std::cout << "[Allocation] Requested block size: 1, Assigned index: " << idx3 << "\n";

        allocatedIndices.push_back(idx1);
        allocatedIndices.push_back(idx2);
        allocatedIndices.push_back(idx3);

        std::cout << "\n--- Current Buddy Block Status After Allocation ---\n";
        for (int i = 0; i < buddyBlocks.size(); ++i) {
            if (buddyBlocks[i].size > 0) {
                std::cout << "Index: " << i
                    << " | Size: " << buddyBlocks[i].size
                    << " | Allocated: " << (buddyBlocks[i].isAllocated ? "Yes" : "No")
                    << " | Parent: " << buddyBlocks[i].parent
                    << " | Left: " << buddyBlocks[i].leftChild
                    << " | Right: " << buddyBlocks[i].rightChild
                    << " | Buddy: " << buddyBlocks[i].buddyIndex << "\n";
            }
        }

        std::cout << "\n--- Releasing One Block (index: " << idx2 << ") ---\n";
        if (!allocatedIndices.empty() && allocatedIndices.size() > 1) {
            freeWithBuddySystem(allocatedIndices[1]);
        }

        std::cout << "\n--- Buddy Block Status After Freeing Index " << idx2 << " ---\n";
        for (int i = 0; i < buddyBlocks.size(); ++i) {
            if (buddyBlocks[i].size > 0) {
                std::cout << "Index: " << i
                    << " | Size: " << buddyBlocks[i].size
                    << " | Allocated: " << (buddyBlocks[i].isAllocated ? "Yes" : "No")
                    << " | Parent: " << buddyBlocks[i].parent
                    << " | Left: " << buddyBlocks[i].leftChild
                    << " | Right: " << buddyBlocks[i].rightChild
                    << " | Buddy: " << buddyBlocks[i].buddyIndex << "\n";
            }
        }

        std::cout << "\n--- Reallocating Block of Size 2 ---\n";
        int idx4 = allocateWithBuddySystem(2);
        std::cout << "[Re-allocation] Requested block size: 2, Assigned index: " << idx4 << "\n";

        allocatedIndices.push_back(idx4);

        std::cout << "\n--- Final Buddy Block Status ---\n";
        for (int i = 0; i < buddyBlocks.size(); ++i) {
            if (buddyBlocks[i].size > 0) {
                std::cout << "Index: " << i
                    << " | Size: " << buddyBlocks[i].size
                    << " | Allocated: " << (buddyBlocks[i].isAllocated ? "Yes" : "No")
                    << " | Parent: " << buddyBlocks[i].parent
                    << " | Left: " << buddyBlocks[i].leftChild
                    << " | Right: " << buddyBlocks[i].rightChild
                    << " | Buddy: " << buddyBlocks[i].buddyIndex << "\n";
            }
        }

        break;
    }

    case 2: {
        std::cout << "\n--- Starting Quick Fit Test ---\n";

        // ����֮ǰ���Կ������µ�ռ��
        for (int i = 0; i < MEMORY_BLOCKS; ++i) {
            memory[i].isAllocated = false;
            memory[i].virtualPageNumber = -1;
        }
        pageTable.clear();
        fifoQueue.clear();
        lruList.clear();

        // ���ò��Ժ� QuickFit ��������
        setAllocationStrategy(AllocationStrategy::QuickFit);
        quickFitMap.clear();           // ��� QuickFit ����
        quickFitInitialized = false;   // ���ó�ʼ����־

        std::vector<int> quickFitFreeList;

        std::cout << "\n--- Allocating Blocks Using Quick Fit ---\n";
        for (int i = 0; i < 5; ++i) {
            int blockNum = allocatePhysicalBlock();
            if (blockNum != -1) {
                std::cout << "[QuickFit] Allocated block " << blockNum << " using Quick Fit\n";
                quickFitFreeList.push_back(blockNum);
            }
            else {
                std::cout << "[QuickFit] Failed to allocate block\n";
            }
        }
        std::cout << "\n--- Freeing Blocks in Reverse Order (Quick Fit) ---\n";
        for (auto it = quickFitFreeList.rbegin(); it != quickFitFreeList.rend(); ++it) {
            freePhysicalBlock(*it);
            std::cout << "[QuickFit] Freed block " << *it << "\n";
        }

        std::cout << "\n--- Final Memory State After Quick Fit Test ---\n";

        break;
    }
    case 3:
        std::cout << "Exiting Index-Based Test.\n";
        break;

    default:
        std::cout << "Invalid choice! Please enter a number between 1 and 3.\n";
        break;
    }
}
//�����ҳʽ�ڴ����
void MemoryManager::PageBasedTest() {
    std::cout << "\n=== �����ҳʽ�ڴ������� ===\n";

    // 1. ����ҳ���滻����
    int replaceChoice;
    std::cout << "ѡ��ҳ���滻�㷨:\n";
    std::cout << "1: FIFO\n";
    std::cout << "2: LRU\n";
    std::cout << "3: Random\n";
    std::cout << "��������(1-3): ";
    std::cin >> replaceChoice;

    ReplacementAlgorithm replaceAlgo;
    switch (replaceChoice) {
    case 1:
        replaceAlgo = ReplacementAlgorithm::FIFO;
        std::cout << "ʹ�� FIFO ҳ���滻�㷨\n";
        break;
    case 2:
        replaceAlgo = ReplacementAlgorithm::LRU;
        std::cout << "ʹ�� LRU ҳ���滻�㷨\n";
        break;
    case 3:
        replaceAlgo = ReplacementAlgorithm::Random;
        std::cout << "ʹ�� Random ҳ���滻�㷨\n";
        break;
    default:
        replaceAlgo = ReplacementAlgorithm::FIFO;
        std::cout << "Ĭ��ʹ�� FIFO ҳ���滻�㷨\n";
    }

    // �����ڴ������ʵ��
    MemoryManager manager(replaceAlgo);

    // 2. ��������ҳ
    std::vector<int> allocatedPages;

    std::cout << "\n--- ��ʼ��������ҳ ---\n";
    for (int i = 0; i < 10; ++i) {
        bool success = manager.allocateMemory(i);
        if (success) {
            std::cout << "�ɹ�������ҳ " << i << " ���ص��ڴ�\n";
            allocatedPages.push_back(i);
        }
        else {
            std::cout << "��������ҳ " << i << " ʧ�ܣ��ڴ治�����޷��滻��\n";
        }
    }

    // 3. ��ӡ��ǰ�ڴ�״̬
    std::cout << "\n--- ��ǰ�ڴ�״̬ ---\n";
    manager.printMemoryState();

    std::cout << "\n--- ��ǰҳ�� ---\n";
    manager.printPageTable(); // ��Ҫ�����˺�������ӡҳ��

    std::cout << "\n--- �ڴ�ʹ��ͳ�� ---\n";
    manager.printMemoryStatistics();

    // 4. ģ�����ĳ��ҳ������ȱҳ�� LRU ����
    int accessPage;
    std::cout << "\n--- ���Է���ĳҳ�Դ���ȱҳ����� LRU ---\n";
    std::cout << "����Ҫ���ʵ�����ҳ��(0-9): ";
    std::cin >> accessPage;

    try {
        char data = manager.getData(accessPage, 0); // ��������
        std::cout << "��������ҳ " << accessPage << " �ɹ���ƫ��0������Ϊ: " << data << "\n";
    }
    catch (...) {
        std::cout << "��������ҳ " << accessPage << " ʧ�ܡ�\n";
    }

    std::cout << "\n--- ���ʺ��ڴ�״̬ ---\n";
    manager.printMemoryState();

    std::cout << "\n--- ���ʺ�ҳ�� ---\n";
    manager.printPageTable();

    // 5. �ͷŲ���ҳ
    std::cout << "\n--- ��ʼ�ͷŲ���ҳ�� ---\n";
    for (size_t i = 0; i < allocatedPages.size(); i += 2) {
        manager.freeMemory(allocatedPages[i]);
        std::cout << "�ͷ�����ҳ " << allocatedPages[i] << "\n";
    }

    // 6. �ٴη�����ҳ����֤���ջ���
    std::cout << "\n--- �ٴγ��Է�����ҳ�� ---\n";
    for (int i = 10; i < 15; ++i) {
        bool success = manager.allocateMemory(i);
        if (success) {
            std::cout << "�ɹ�������ҳ " << i << " ���ص��ڴ�\n";
            allocatedPages.push_back(i);
        }
        else {
            std::cout << "��������ҳ " << i << " ʧ��\n";
        }
    }
    std::cout << "\n--- ��ʼ 20 �����ҳ����ʲ��� ---\n";
    for (int i = 0; i < 20; ++i) {
        int randomPage = rand() % 15;
        std::cout << "����ҳ��: " << randomPage << " | ";

        try {
            char data = manager.getData(randomPage, 0);
            std::cout << "���У�����Ϊ: '" << data << "'\n";
        }
        catch (...) {
            std::cout << "ȱҳ\n";
        }

        if ((i + 1) % 5 == 0) {
            std::cout << "--- �� " << i + 1 << " �η��ʺ�״̬ ---\n";
            manager.printMemoryState();
            manager.printPageTable();
            manager.printMemoryStatistics();
        }
    }

    // 7. ���մ�ӡ���
    std::cout << "\n--- �����ڴ�״̬ ---\n";
    manager.printMemoryState();


    std::cout << "\n--- �����ڴ�ͳ����Ϣ ---\n";
    manager.printMemoryStatistics();

    std::cout << "\n=== �����ҳʽ�ڴ������Խ��� ===\n";
}
//��ҳʽ�ڴ����
void MemoryManager::SegmentBasedtest() {
    std::cout << "\n=== ��ҳʽ�ڴ������� ===\n";

    SegmentDescriptor codeSegment = { 0, 5 };   // ID: 0, 5
    SegmentDescriptor dataSegment = { 1, 3 };   // ID: 1, 3
    SegmentDescriptor stackSegment = { 2, 4 };  // ID: 2, 4
    SegmentDescriptor heapSegment = { 3, 4 };   // ID: 3, 4

    segmentList.push_back(codeSegment);
    segmentList.push_back(dataSegment);
    segmentList.push_back(stackSegment);
    segmentList.push_back(heapSegment);

    std::cout << "��ʼ������Ϣ��\n";
    int totalPages = 0;
    for (const auto& seg : segmentList) {
        std::cout << "�� ID: " << seg.segmentId
            << ", ҳ��: " << seg.totalPages << "\n";
        totalPages += seg.totalPages;
    }

    if (totalPages > MEMORY_BLOCKS) {
        std::cout << "��ҳ�������ڴ�����(" << MEMORY_BLOCKS << ")\n";
        return;
    }

    // Ϊÿ���η����ڴ�
    std::cout << "\n--- ��ʼΪ�η����ڴ� ---\n";
    for (auto& segment : segmentList) {
        std::cout << "Ϊ�� ID: " << segment.segmentId << " �����ڴ�:\n";
        for (int i = 0; i < segment.totalPages; ++i) {
            int physicalBlock = allocatePhysicalBlock();
            if (physicalBlock == -1) {
                replacePage();  // �ڴ治��ʱ����ҳ���滻
                physicalBlock = allocatePhysicalBlock();
            }

            if (physicalBlock != -1) {
                // ��������ҳ�Ų�ӳ��
                int virtualPage = segment.segmentId * 100 + i;
                memory[physicalBlock].virtualPageNumber = virtualPage;
                memory[physicalBlock].isAllocated = true;

                // ����ȫ��ҳ��Ͷ�ҳ��
                pageTable[virtualPage] = physicalBlock;
                segment.pageTable[i] = physicalBlock;

                std::cout << "��[" << segment.segmentId
                    << "] ������ҳ[" << i
                    << "] ӳ�䵽�����[" << physicalBlock << "]\n";
            }
        }
    }

    // ��ӡ��ǰ�ڴ�״̬
    printMemoryState();

    // ģ����ʶ��е�ҳ�棨ȱҳ��
    std::cout << "\n--- ģ����ʶ��е�ҳ�� ---\n";
    try {
        char data = getData(102, 0); // ���ʶ�1������ҳ2
        std::cout << "���ʶ�1������ҳ2�ɹ�������Ϊ: '" << data << "'\n";
    }
    catch (...) {
        std::cout << "����ʧ�ܣ�����ȱҳ\n";
    }

    // ����ҳ���滻ǰ��ռ�������ڴ�
    std::cout << "\n--- ����ҳ���滻 ---\n";
    std::cout << "��������ڴ���ȷ�����Խ���ҳ���滻...\n";
    for (int i = 0; i < MEMORY_BLOCKS; ++i) {
        int virtualPage = 900 + i; // ʹ���µ�����ҳ�ţ������ͻ
        if (!allocateMemory(virtualPage)) {
            std::cout << "����ڴ�ʧ��\n";
            break;
        }
    }

    // ���ڿ��԰�ȫ�ص��� replacePage()
    replacePage();

    // �ٴη��ʱ��滻��ҳ�棨��֤�Ƿ����¼��أ�
    std::cout << "\n--- �ٴη��ʱ��滻��ҳ�� ---\n";
    try {
        char data = getData(0, 0); // ���ʶ�0������ҳ0�������ѱ��滻��
        std::cout << "ҳ�������¼��أ�����Ϊ: '" << data << "'\n";
    }
    catch (...) {
        std::cout << "�޷�����ҳ��\n";
    }

    // �ͷŶ��ڴ�
    std::cout << "\n--- �ͷŶ��ڴ� ---\n";
    for (auto& segment : segmentList) {
        std::cout << "�ͷŶ� ID: " << segment.segmentId << "\n";
        for (const auto& entry : segment.pageTable) {
            int virtualPage = segment.segmentId * 100 + entry.first;
            freeMemory(virtualPage);
        }
    }

    // ��ӡ�����ڴ�״̬
    printMemoryState();
    printMemoryStatistics();

    std::cout << "\n=== ��ҳʽ�ڴ������Խ��� ===\n";
}
void MemoryManager::MemoryManagerTest() {
    int mode;
    while (true) {
        std::cout << "\n=== �ڴ������� ===\n";
        std::cout << "1. ���Ի���������̬�������\n";
        std::cout << "2. ���Ի���������̬�������\n";
        std::cout << "3. ���������ҳʽ����\n";
        std::cout << "4. ���Զ�ҳʽ����\n";
        std::cout << "��ѡ��ģʽ: ";
        std::cin >> mode;

        switch (mode) {
        case 1: SearchingBasedTest(); break;
        case 2: IndexBasedTest(); break;
        case 3: PageBasedTest(); break;
        case 4: SegmentBasedtest(); break;
        default: std::cout << "��Чѡ�������" << endl;
        }
    }
    return;
}