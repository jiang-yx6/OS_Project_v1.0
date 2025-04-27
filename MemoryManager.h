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
    // ҳ���û��㷨ö��
    enum class ReplacementAlgorithm {
        FIFO,   // �Ƚ��ȳ�
        LRU,    // �������ʹ��
        Random  // ����û�
    };

    // �ڴ�������ö��
    enum class AllocationStrategy {
        FirstFit,  // �״���Ӧ
        BestFit,   // �����Ӧ
        WorstFit,  // �����Ӧ
        NextFit    // �´���Ӧ
    };

    // ���캯����ָ��ҳ���û��㷨
    MemoryManager(ReplacementAlgorithm algorithm = ReplacementAlgorithm::FIFO);

    // ��������
    ~MemoryManager();

    // ����һ������������ҳ
    bool allocateMemory(int virtualPageNumber);

    // �ͷŷ��������ҳ���ڴ�
    void freeMemory(int virtualPageNumber);
    //�������ļ�����Խ�
    // ��ȡָ���߼���ź�ƫ����������
    char getData(int logicalBlockNumber, int offset);

    // ����ҳ���滻�㷨
    void setReplacementAlgorithm(ReplacementAlgorithm algorithm);

    // �����ڴ�������
    void setAllocationStrategy(AllocationStrategy strategy) {
        allocationStrategy = strategy;
        lastAllocatedBlock = -1;  // ����������Ŀ�����
    }

    // ��ӡ��ǰ�ڴ�״̬
    void printMemoryState() const;

    // ��ӡ�ڴ�ʹ��ͳ����Ϣ
    void printMemoryStatistics() const;

    // �ڴ���Ƭ������
    void defragmentMemory();
    // �ڴ���������Ժ���
    void MemoryManagerTest();
private:
    // �ڴ������ṹ
    struct MemoryBlock {
        bool isAllocated;            // �Ƿ��ѷ���
        int virtualPageNumber;       // ��Ӧ������ҳ�ţ�-1��ʾδ����
        char data[BLOCK_SIZE];       // ���ʵ������
    };

    // ����һ�������
    int allocatePhysicalBlock();

    // �ͷ�һ�������
    void freePhysicalBlock(int physicalBlockNumber);

    // ҳ���û�
    void replacePage();

    // ��ǰʹ�õ�ҳ���û��㷨
    ReplacementAlgorithm replacementAlgorithm;

    // ��ǰʹ�õ��ڴ�������
    AllocationStrategy allocationStrategy = AllocationStrategy::FirstFit;

    // ������Ŀ�����������NextFit����
    int lastAllocatedBlock = -1;

    // �ڴ������
    std::vector<MemoryBlock> memory;

    // ҳ��������ҳ��ӳ�䵽������
    std::unordered_map<int, int> pageTable;

    // FIFO�㷨�Ķ���
    std::deque<int> fifoQueue;

    // LRU�㷨���б�
    std::list<int> lruList;

    // ʹ��FirstFit���Է����
    int allocateWithFirstFit();

    // ʹ��BestFit���Է����
    int allocateWithBestFit();

    // ʹ��WorstFit���Է����
    int allocateWithWorstFit();

    // ʹ��NextFit���Է����
    int allocateWithNextFit();

    // �Ӵ��̶�ȡ������
    char* readBlock(int blockNumber);

    // ��������д�ش���
    void writeBlock(const char* data, int blockNumber);


};


#endif // MEMORYMANAGER_H