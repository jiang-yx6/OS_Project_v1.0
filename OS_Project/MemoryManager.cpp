#include "MemoryManager.h"
#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>
#include <random>
#include <chrono>
#include <mutex>
#include <condition_variable>

// ���캯��
MemoryManager::MemoryManager(int totalPages, AllocationMethod method)
    : method(method) {
    memory.push_back(Block{ 0, totalPages, true }); // ��ʼ��һ������ڴ�
}

// ��������
MemoryManager::~MemoryManager() {}

// �����ڴ�
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

    // ����ڴ治�㣬��������ͷ�һ���ѷ�����ڴ�
    if (randomlyDeallocate()) {
        // ���³��Է���
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

    return false; // ����ʧ��
}

// �ͷ��ڴ�
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

// ��ӡ�ڴ�״̬���ն�
void MemoryManager::printMemoryState() const {
    std::cout << "��ǰ�ڴ�״̬:\n";
    for (const auto& block : memory) {
        std::cout << "��ʼҳ: " << block.start << ", ��С: " << block.size
            << " ҳ, ״̬: " << (block.isFree ? "����" : "�ѷ���") << "\n";
    }
    std::cout << "--------------------------\n";
}

// �״���Ӧ�㷨
int MemoryManager::firstFit(int pages) {
    for (int i = 0; i < memory.size(); ++i) {
        if (memory[i].isFree && memory[i].size >= pages) {
            splitBlock(i, pages);
            return i;
        }
    }
    return -1; // δ�ҵ����ʵĿ�
}

// �����Ӧ�㷨
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

// �ָ��ڴ��
void MemoryManager::splitBlock(int index, int pages) {
    if (memory[index].size > pages) {
        Block newBlock = { memory[index].start + pages, memory[index].size - pages, true };
        memory.insert(memory.begin() + index + 1, newBlock);
    }
    memory[index].size = pages;
    memory[index].isFree = false;
}

// �ϲ����ڿ��п�
void MemoryManager::mergeFreeBlocks() {
    for (int i = 0; i < memory.size() - 1; ++i) {
        if (memory[i].isFree && memory[i + 1].isFree) {
            memory[i].size += memory[i + 1].size;
            memory.erase(memory.begin() + i + 1);
            --i; // ����һ�����¼��
        }
    }
}

// ����ͷ�һ���ѷ�����ڴ�
bool MemoryManager::randomlyDeallocate() {
    std::vector<int> allocatedIndices;
    for (int i = 0; i < memory.size(); ++i) {
        if (!memory[i].isFree) {
            allocatedIndices.push_back(i);
        }
    }

    if (allocatedIndices.empty()) {
        return false; // û���ѷ�����ڴ��
    }

    // ���ѡ��һ���ѷ���Ŀ�
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, allocatedIndices.size() - 1);
    int randomIndex = allocatedIndices[dist(gen)];

    // �ͷŸÿ�
    memory[randomIndex].isFree = true;
    mergeFreeBlocks();
    std::cout << "�ڴ治�㣡����ͷ�����ʼҳΪ " << memory[randomIndex].start << " ���ڴ�顣\n";
    return true;
}

// ���Ժ���ʵ��
void Memorytest() {
    try {
        // ȫ�ֱ���
        std::mutex globalMutex;
        std::condition_variable cv;
        bool isAllocating = true; // �����߳�ͬ��

        // �û��ڴ�������4ҳ��32ҳ��
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> userMemoryDist(16, 32); // �û��ڴ���ҳ��
        int totalPages = userMemoryDist(gen);

        // �������У�������ɷ�����ͷŲ�����
        std::uniform_int_distribution<> requestSizeDist(1, 8); // ÿ�������ҳ����Χ
        int numRequests = 10; // ����������
        std::vector<int> requestSequence(numRequests);
        for (int i = 0; i < numRequests; ++i) {
            requestSequence[i] = requestSizeDist(gen);
        }

        // �û�ѡ���ڴ����ʽ
        std::cout << "ѡ���ڴ����ʽ (1: �״���Ӧ, 2: �����Ӧ): ";
        int choice;
        std::cin >> choice;
        MemoryManager::AllocationMethod method = MemoryManager::AllocationMethod::FirstFit;
        if (choice == 2) {
            method = MemoryManager::AllocationMethod::BestFit;
        }

        MemoryManager manager(totalPages, method);

        // �ڴ�����߳�
        std::thread allocThread([&manager, &requestSequence, &globalMutex, &cv, &isAllocating]() {
            for (int pages : requestSequence) {
                {
                    std::unique_lock<std::mutex> lock(globalMutex);
                    cv.wait(lock, [&isAllocating] { return isAllocating; });
                    std::cout << "���Է��� " << pages << " ҳ�ڴ�...\n";
                    bool success = manager.allocate(pages);
                    if (success) {
                        std::cout << "�ɹ����� " << pages << " ҳ�ڴ档\n";
                    }
                    else {
                        std::cout << "����ʧ�ܣ��ڴ治�㡣\n";
                    }
                    manager.printMemoryState(); // ��ӡ�ڴ�״̬
                    isAllocating = false; // �л��������߳�
                }
                cv.notify_one();
                std::this_thread::sleep_for(std::chrono::milliseconds(500)); // ģ���ӳ�
            }
            });

        // �����ڴ�״̬�߳�
        std::thread trackThreadObj([&manager, &globalMutex, &cv, &isAllocating, numRequests]() {
            for (int i = 0; i < numRequests; ++i) {
                {
                    std::unique_lock<std::mutex> lock(globalMutex);
                    cv.wait(lock, [&isAllocating] { return !isAllocating; });
                    std::cout << "�����ڴ�״̬...\n";
                    manager.printMemoryState(); // ��ӡ�ڴ�״̬
                    isAllocating = true; // �л��ط����߳�
                }
                cv.notify_one();
                std::this_thread::sleep_for(std::chrono::milliseconds(500)); // ģ���ӳ�
            }
            });

        // �ȴ��߳����
        allocThread.join();
        trackThreadObj.join();

        std::cout << "ģ����ɣ�\n";
    }
    catch (const std::exception& e) {
        std::cerr << "��������: " << e.what() << "\n";
    }
}