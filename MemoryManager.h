#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include <iostream>
#include <unordered_map>
#include <deque>
#include <vector>
#include <list>
#include <cstring> 
#include <string>
#define MAX_PAGES_PER_SEGMENT 8// ÿ���ε����ҳ��
#define MEMORY_BLOCKS 16
#define BLOCK_SIZE 4096
using namespace std;
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
        NextFit,    // �´���Ӧ
        QuickFit,   // ������Ӧ
        BuddySystem // ���ϵͳ֧��
    };
    // �ڴ������ṹ
    struct MemoryBlock {
        bool isAllocated;            // �Ƿ��ѷ���
        int virtualPageNumber;       // ��Ӧ������ҳ�ţ�-1��ʾδ����
        char data[BLOCK_SIZE];       // ���ʵ������
    };

    //buddy��ṹ
    struct BuddyBlock {
        int size;            // ���С���� BLOCK_SIZE Ϊ��λ��
        bool isAllocated;    // �Ƿ��ѷ���
        int leftChild;       // ��������
        int rightChild;      // �Һ�������
        int parent;          // ���ڵ�����
        int buddyIndex;      // ��Ӧ�������
    };

    // ��������
    struct SegmentDescriptor {
        int segmentId;                        // ��ID
        int totalPages;                       // ��ҳ��
        std::unordered_map<int, int> pageTable; // ���ڵ�ҳ������ҳ�� -> �����ţ�
    };

    // ��ӵ� MemoryManager ������Ϊ��Ա����
    std::vector<SegmentDescriptor> segmentList; // ���ж��б�
    std::unordered_map<int, std::vector<int>> quickFitMap; // �滻֮ǰ�� static ����
    bool quickFitInitialized = false;                     // ��ʼ�����
    int allocateWithBuddySystem(size_t sizeInBlocks); // ����С����
    void freeWithBuddySystem(int blockIndex);        // �ͷŲ����Ժϲ�
    void splitBlock(int index);                      // ���ѿ�
    int findBuddy(int index);                        // ���һ��
    void mergeBuddies(int index);                    // �ϲ����
    std::vector<BuddyBlock> buddyBlocks;  // Buddy ϵͳʹ�õĿ�����
    int maxOrder;                         // ��������2^maxOrder = ���ڴ������

    // ���캯����ָ��ҳ���û��㷨
    MemoryManager(ReplacementAlgorithm algorithm = ReplacementAlgorithm::FIFO);
    // ��������
    ~MemoryManager();


    // ҳ���û�
    void replacePage();
    // ����һ������������ҳ
    bool allocateMemory(int virtualPageNumber);
    // ����һ�������
    int allocatePhysicalBlock();
    // �ͷ�һ�������
    void freePhysicalBlock(int physicalBlockNumber);
    // �ͷŷ��������ҳ���ڴ�
    void freeMemory(int virtualPageNumber);
    // �ڴ���Ƭ������
    void defragmentMemory();
    // �ڴ��
    std::vector<MemoryBlock> memory;
    // ҳ��������ҳ��ӳ�䵽������
    std::unordered_map<int, int> pageTable;


    // ����ҳ���滻�㷨
    void setReplacementAlgorithm(ReplacementAlgorithm algorithm);
    // �����ڴ�������
    void setAllocationStrategy(AllocationStrategy strategy) {
        allocationStrategy = strategy;
        lastAllocatedBlock = -1;  // ����������Ŀ�����
    }


    // ��ӡ��ǰ�ڴ�״̬
    void printMemoryState() const;
    // ֻ��ӡҳ��
    void printPageTable() const {
        std::cout << "Page Table (Virtual -> Physical):\n";
        for (const auto& entry : pageTable) {
            std::cout << "Virtual Page " << entry.first
                << " -> Physical Block " << entry.second << "\n";
        }
    }
    // ��ӡ�ڴ�ʹ��ͳ����Ϣ
    void printMemoryStatistics() const;


    // ��ǰʹ�õ�ҳ���û��㷨
    ReplacementAlgorithm replacementAlgorithm;
    // ��ǰʹ�õ��ڴ�������
    AllocationStrategy allocationStrategy = AllocationStrategy::FirstFit;
    // ������Ŀ�����������NextFit����
    int lastAllocatedBlock = -1;


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
    // ���ڿ����������
    int allocateWithQuickFit();


    //�������ļ�����Խ�
    // ��ȡָ���߼���ź�ƫ����������
    char getData(int logicalBlockNumber, int offset);
    // �Ӵ��̶�ȡ������
    char* readBlock(int blockNumber);
    // ��������д�ش���
    void writeBlock(const char* data, int blockNumber);


    //��������̹���Խ�

    void allocatePCBmemory(string pname,int pid);
    void deletePCBmomory(string pname, int pid);

    //�����������ڴ���Ժ���
    void SearchingBasedTest();
    //�����������ڴ���亯��
    void IndexBasedTest();
    //�����ҳʽ�ڴ����
    void PageBasedTest();
    //��ҳʽ�ڴ����
    void SegmentBasedtest();
    // �ڴ�������ܲ��Ժ���
    void MemoryManagerTest();

};


#endif // MEMORYMANAGER_H
