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
//        int start;   // ��ʼ��ַ����λ��ҳ��
//        int size;    // ��С����λ��ҳ��
//        bool isFree; // �Ƿ����
//    };
//    vector<Block> memory; // ģ���ڴ��
//    AllocationMethod method;   // �ڴ���䷽��
//
//    mutex memMutex;       // �����ڴ�����Ļ�����
//    const int pageSize = 4096; // ҳ���С��1KB��
//
//public:
//    MemoryManager(int totalPages, AllocationMethod method);
//    ~MemoryManager();
//
//    // �����ڴ�
//    bool allocate(int pages);
//
//    // �ͷ��ڴ�
//    void deallocate(int startPage);
//
//    // ��ӡ�ڴ�״̬���ն�
//    void printMemoryState() const;
//
//private:
//    // �״���Ӧ�㷨
//    int firstFit(int pages);
//
//    // �����Ӧ�㷨
//    int bestFit(int pages);
//
//    // �ָ��ڴ��
//    void splitBlock(int index, int pages);
//
//    // �ϲ����ڿ��п�
//    void mergeFreeBlocks();
//
//    // ����ͷ�һ���ѷ�����ڴ�
//    bool randomlyDeallocate();
//};
//
//// ���Ժ�������
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

    // ����һ�������
    int allocatePhysicalBlock();

    // �ͷ�һ�������
    void freePhysicalBlock(int physicalBlockNumber);

    // ҳ���û��㷨
    void replacePage();

    // �����ڴ������ҳ��
    bool allocateMemory(int virtualPageNumber);

    // �ͷ��ڴ�
    void freeMemory(int virtualPageNumber);

    // ��ȡָ���߼���ź�ƫ����������
    char getData(int logicalBlockNumber, int offset);

    // ��ӡ�ڴ�״̬
    void printMemoryState() const;

    // ���õ����㷨
    void setReplacementAlgorithm(ReplacementAlgorithm algorithm);

private:
    struct MemoryBlock {
        bool isAllocated;
        int virtualPageNumber;
        char data[BLOCK_SIZE];
    };

    ReplacementAlgorithm replacementAlgorithm;
    std::vector<MemoryBlock> memory;
    std::map<int, int> pageTable; // ����ҳ�� -> ������
    std::deque<int> fifoQueue;    // FIFO ����
    std::vector<int> lruList;     // LRU �б�

    // ģ����߼����ȡ����
    char* readBlock(int virtualPageNumber) {
        static char buffer[BLOCK_SIZE];
        std::fill(buffer, buffer + BLOCK_SIZE, 'A' + (virtualPageNumber % 26)); // ���ģ������
        return buffer;
    }

    // ģ�⽫����д���߼���
    void writeBlock(const char* data, int virtualPageNumber) {
        // ģ��д�ز���
        std::cout << "Writing data back to logical block " << virtualPageNumber << "\n";
    }
};
#endif // MEMORY_MANAGER_H