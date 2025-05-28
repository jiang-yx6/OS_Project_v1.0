#pragma once
#include <iostream>
#include <map>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>
#include <thread>
#include <chrono>
#include <vector>
#include "state.h"
using namespace std;
class FileResource {
public:
    std::string filename;
    atomic<bool> is_locked;
    int holder_pid; // 当前持有者 PID
    std::queue<int> wait_queue; // 等待该文件的进程 PID 队列
    std::mutex mtx;
    std::condition_variable cv;

    FileResource(const std::string& name,int pid)
        : filename(name), is_locked(false), holder_pid(pid) {
    }
};


class FileResourceManager {
private:
    std::map<std::string, FileResource*> fileResources;
    std::mutex mtx;

public:
    // 获取或创建一个文件资源
    FileResource* getFileResource(const std::string& filename,int pid) {
        std::lock_guard<std::mutex> lock(mtx);
        if (!fileResources.count(filename)) {
            fileResources[filename] = new FileResource(filename,pid);
        }
        return fileResources[filename];
    }

    bool tryRequestFileAccess(int pid, const std::string& filename) {
        FileResource* res = getFileResource(filename,pid);
        std::lock_guard<std::mutex> lock(res->mtx);
        if (res->holder_pid == pid) {
            //std::cout << "PID Same" << std::endl;
            return true;
        }
        if (res->is_locked.load()) {
            //std::cout << "PID[" << pid << "] blocked on file: " << filename << std::endl;

            return false; // 加锁失败
        }

        res->is_locked.store(true);
        //std::cout << "PID[" << pid << "] acquired file: " << filename << std::endl;

        res->holder_pid = pid;
        return true;
    }

    // 进程请求访问文件
    void requestFileAccess(int pid, const std::string& filename) {
        FileResource* res = getFileResource(filename, pid);

        std::unique_lock<std::mutex> lock(res->mtx);//获取文件锁
        if (res->is_locked) {
            //std::cout << "PID[" << pid << "] blocked on file: " << filename << std::endl;
            res->wait_queue.push(pid);
            this_thread::sleep_for(std::chrono::milliseconds(2000));
            res->cv.wait(lock, [&]() { return !res->is_locked; });
        }

        res->is_locked = true;
        res->holder_pid = pid;
        //std::cout << "PID[" << pid << "] acquired file: " << filename << std::endl;
    }

    // 进程释放文件
    void releaseFileAccess(int pid, const std::string& filename) {
        FileResource* res = getFileResource(filename, pid);
        std::lock_guard<std::mutex> lock(res->mtx);
        if (res->holder_pid != pid) return;

        res->is_locked.store(false);
        res->holder_pid = -1;
        //std::cout << "PID[" << pid << "] released file: " << filename  << " "<< res->is_locked.load() << " " <<res->holder_pid << std::endl;
    }

};
