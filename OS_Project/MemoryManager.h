#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <vector>
#include <mutex>
#include <random>

class MemoryManager {
public:
    enum class AllocationMethod {
        FirstFit,
        BestFit
    };

private:
    struct Block {
        int start;   // ��ʼ��ַ����λ��ҳ��
        int size;    // ��С����λ��ҳ��
        bool isFree; // �Ƿ����
    };

    std::vector<Block> memory; // ģ���ڴ��
    AllocationMethod method;   // �ڴ���䷽��
    std::mutex memMutex;       // �����ڴ�����Ļ�����
    const int pageSize = 1024; // ҳ���С��1KB��

public:
    MemoryManager(int totalPages, AllocationMethod method);
    ~MemoryManager();

    // �����ڴ�
    bool allocate(int pages);

    // �ͷ��ڴ�
    void deallocate(int startPage);

    // ��ӡ�ڴ�״̬���ն�
    void printMemoryState() const;

private:
    // �״���Ӧ�㷨
    int firstFit(int pages);

    // �����Ӧ�㷨
    int bestFit(int pages);

    // �ָ��ڴ��
    void splitBlock(int index, int pages);

    // �ϲ����ڿ��п�
    void mergeFreeBlocks();

    // ����ͷ�һ���ѷ�����ڴ�
    bool randomlyDeallocate();
};

// ���Ժ�������
void Memorytest();

#endif // MEMORY_MANAGER_H