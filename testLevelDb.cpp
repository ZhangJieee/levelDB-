#include <atomic>
#include <cassert>
#include<iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <memory>
// #include "include/leveldb/db.h"
using namespace std;

// int result = 0;

// void GetResultCore(int total, int size, std::vector<int> goods, int idx, int currentTotal) {
//     if (currentTotal == total) {
//         result++;
//         return;
//     }

//     if (idx >= size) {
//         return;
//     }

//     GetResultCore(total, size, goods, idx + 1, currentTotal + goods[idx]);
//     GetResultCore(total, size, goods, idx + 1, currentTotal);
// }

// int GetResult(int total, int size, std::vector<int> goods) {
//     GetResultCore(total, size, goods, 0, 0);
// }

// int result1 = 10000;

// void GetResult1Core(std::vector<std::vector<int>> materix, std::vector<int> target1, int x, int y, int steps, std::vector<int> target2, std::vector<std::vector<bool>>& flags) {
//     if (x >= materix.size() || x < 0 || y >= materix[0].size() || y < 0 /*|| flags[x][y]*/){
//         return;
//     }

//     if (materix[x][y] != materix[target1[0]][target1[1]] && materix[x][y] != 0) {
//         return;
//     }

//     flags[x][y] = true;
//     const int xValue[4] = {0, 0, 1, -1};
//     const int yValue[4] = {1, -1, 0, 0};

//     if (target2[0] == x && target2[1] == y) {
//         if (steps != 0 && steps < result1) {
//             result1 = steps;
//         }

//         return;
//     }

//     for (int i = 0; i < 4; i++) {
//         GetResult1Core(materix, target1, x + xValue[i], y + yValue[i], steps + 1, target2, flags);
//     }
// }

// int GetResult1(std::vector<std::vector<int>> materix, std::vector<int> target1, std::vector<int> target2) {
//     if (materix.empty()) return 0;

//     std::vector<std::vector<bool>> flags(materix.size(), std::vector<bool>(materix[0].size()));
//     GetResult1Core(materix, target1, target1[0], target1[1], 0, target2, flags);
// }

volatile int lock = 0;
void *worker(void*)
{
  while (__sync_lock_test_and_set(&lock, 1));
  // critical section
  __sync_lock_release(&lock);
}

template<typename T>
static inline void atomic_swap(T& a, T& b){
    T* ptr = &a;
    b =__sync_lock_test_and_set(ptr, b);
    __sync_lock_release(&ptr);
}

sem_t beginSema1;
sem_t beginSema2;
sem_t endSema;

int X, Y;
int r1;

void *thread1Func(void *param)
{
    for (;;)                                  // Loop indefinitely
    {
        sem_wait(&beginSema1);                // Wait for signal from main thread
        while (rand() % 8 != 0) {}  // Add a short, random delay

        // ----- THE TRANSACTION! -----
        X = 1;

        // 防止编译层面的内存乱序
        // asm volatile("" ::: "memory");        // Prevent compiler reordering

        // CPU内存屏障，mfence指令，StoreLoad
        // asm volatile("mfence" ::: "memory");
        __sync_synchronize();
        //   while (__sync_lock_test_and_set(&lock, 1));
          r1 = Y;
//   __sync_lock_release(&lock);
        // atomic_swap(r1, Y);
        // __sync_lock_test_and_set(&r1, Y);
        // __sync_fetch_and_add(&r1, 1);
        // r1++;

        sem_post(&endSema);                   // Notify transaction complete
    }
    return NULL;  // Never returns
}

void *thread2Func(void *param) 
{
        for (;;)                                  // Loop indefinitely
    {
        sem_wait(&beginSema2);                // Wait for signal from main thread
        while (rand() % 8 != 0) {}  // Add a short, random delay

        // ----- THE TRANSACTION! -----
        Y = 1;

        // 防止编译层面的内存乱序
        // asm volatile("" ::: "memory");        // Prevent compiler reordering

        // CPU内存屏障，mfence指令，StoreLoad
        // asm volatile("mfence" ::: "memory");
        __sync_synchronize();
                //   while (__sync_lock_test_and_set(&lock, 1));
          r1 = X;
//   __sync_lock_release(&lock);
        // r1 = X;
        // __sync_lock_test_and_set(&r1, X);
        // atomic_swap(r1, X);
        // __sync_fetch_and_add(&r1, 1);
        // r1++;

        sem_post(&endSema);                   // Notify transaction complete
    }
    return NULL;  // Never returns
}

// int A, B;

// void foo()
// {
//     A = B + 1;
//     asm volatile("" ::: "memory");
//     B = 0;
// }

#define COMPILER_BARRIER() asm volatile("lfence" ::: "memory")
int Value;
int IsPublished = 0;
// // std::atomic<int> IsPublished(0);

// // void sendValue(int x)
// // {
// //     Value = x;
// //     // <-- reordering is prevented here!
// //     IsPublished.store(1, std::memory_order_release);
// // }

// void sendValue(int x)
// {
//     Value = x;
//     COMPILER_BARRIER();          // prevent reordering of stores
//     IsPublished = 1;
// }

// int tryRecvValue()
// {
//     if (IsPublished)
//     {
//         COMPILER_BARRIER();      // prevent reordering of loads
//         return Value;
//     }
//     return -1;  // or some other value to mean not yet received
// }

// void doSomeStuff(int *a, int *b) 
// { 
//     *a = 5;
//     sendValue(123);       //防止相邻赋值的重新排序
//     *b = *a; 
// }

// int A, B;

// void foo()
// {
//     if (A)
//         B++;
// }

// void foo1()
// {
//     register int r = B;    // Promote B to a register before checking A.
//     if (A)
//         r++;
//     B = r;          // Surprise! A new memory store where there previously was none.
// }

// inline void MemoryBarrier() {
//   __asm__ __volatile__("" : : : "memory");
// }

// class AtomicPointer {
//  private:
//   void* rep_;
//  public:
//   AtomicPointer() { }
//   explicit AtomicPointer(void* p) : rep_(p) {}
//   inline void* NoBarrier_Load() const { return rep_; }
//   inline void NoBarrier_Store(void* v) { rep_ = v; }
//   inline void* Acquire_Load() const {
//     void* result = rep_; // read-acquire
//     MemoryBarrier();
//     return result;
//   }
//   inline void Release_Store(void* v) {
//     MemoryBarrier();
//     rep_ = v; // write-release
//   }
// };

// float f, fo, foa;

// int sharedValue;
// std::atomic<int> flag;
// void* IncrementSharedValue10000000Times1(void*)
// {
// int count = 0;
// while (count < 10000000)
// {
//     usleep(random()%2);
//     int expected = 0;
//     if (flag.compare_exchange_strong(expected, 1, memory_order_relaxed))
//     {
//         // Lock was successful
//         sharedValue++;
//         flag.store(0, memory_order_relaxed);
//         count++;
//         printf("1 : %d\n", count);
//     }
// }
// printf("finish 1\n");
// }

// void* IncrementSharedValue10000000Times2(void*)
// {
// int count = 0;
// while (count < 10000000)
// {
//     usleep(random()%3);
//     int expected = 0;
//     if (flag.compare_exchange_strong(expected, 1, memory_order_relaxed))
//     {
//         // Lock was successful
//         sharedValue++;
//         flag.store(0, memory_order_relaxed);
//         count++;
//         printf("2 : %d\n", count);
//     }
// }
// printf("finish 2\n");
// }

	// mov	edi, 1
	// call	sleep@PLT
	// mov	edx, 1
	// xor	eax, eax
	// mov	DWORD PTR 12[rsp], 0
	// lock cmpxchg	DWORD PTR flag[rip], edx
	// jne	.L9
	// add	ebx, 1
	// add	DWORD PTR sharedValue[rip], 1
	// cmp	ebx, 9999999
	// mov	DWORD PTR flag[rip], 0
	// jle	.L4

int TestCore(vector<int>& nums, int start, int end) {
    if(start >= end) return 0;
    if(start == end - 1) {
        if (nums[start] > nums[end]) {
            swap(nums[start], nums[end]);
            return 1;
        }
        return 0;
    }

    int mid = start + (end - start)/2;
    int leftSize = TestCore(nums, start, mid);
    int rightSize = TestCore(nums, mid + 1, end);

    int curSize = 0;
    int leftIdx = start;
    int leftEnd = mid;

    int rightIdx = mid + 1;
    int rightEnd = end;

    vector<int> tmpNums(end - start + 1);
    int tmpIdx = tmpNums.size() - 1;
    while(leftIdx <= leftEnd && rightIdx <= rightEnd) {
        if(nums[leftEnd] < nums[rightEnd])
        {
            tmpNums[tmpIdx] = nums[rightEnd];
            tmpIdx--;
            rightEnd--;
        } else {
            curSize += rightEnd - rightIdx + 1;
            tmpNums[tmpIdx] = nums[leftEnd];
            tmpIdx--;
            leftEnd--;
        }
    }

    while(leftIdx <= leftEnd) {
        tmpNums[tmpIdx--] = nums[leftEnd--];
    }

    while(rightIdx <= rightEnd) {
        tmpNums[tmpIdx--] = nums[rightEnd--];
    }

    for(int i = 0; i < tmpNums.size(); ++i)
    {
        nums[start + i] = tmpNums[i];
    }

    return curSize + leftSize + rightSize;
}

int Test( vector<int>& nums) {
    if(nums.empty()) return 0;

    return TestCore(nums, 0, nums.size() - 1);
}

vector<vector<int>> result;
void TestQCore(vector<int>& nums, int start, int end) {
    if (start == end + 1) {
        result.push_back(nums);
        return;
    }

    // 1 2 2 3

    // 1 2 2 3
    // 1 3 2 2
    // 1 2 3 2  -- 
    // 2 1 2 3  -- 
    // 2 1 3 2
    // 2 2 1 3
    // 2 2 3 1
    // 2 3 2 1
    // 2 3 1 2
    // 3 1 2 2
    // 3 2 1 2
    // 3 2 2 1
    for (int i = start; i <= end; ++i) {
        if (i > start && nums[i] == nums[i - 1]) continue;
        swap(nums[i], nums[start]);
        TestQCore(nums, start + 1, end);
        swap(nums[i], nums[start]);
    }
}

void TestQ(vector<int>& nums) {
    if (nums.empty()) return;

    TestQCore(nums, 0, nums.size() - 1);
    return;
}

class Bptr;
class Aptr
{
public:
Aptr(){
}
~Aptr()
{
    cout<<"delete A"<<endl;
}

std::weak_ptr<Bptr> sharedB;
};

class Bptr
{
public:
Bptr(){
}

~Bptr()
{
    cout<<"delete B"<<endl;
}

std::weak_ptr<Aptr> sharedA;
};

class SingleTon {
    public:
    ~SingleTon(){}
    SingleTon(const SingleTon&) = delete;
    SingleTon& operator=(const SingleTon&) = delete;
    static SingleTon& getInstance()
    {
        static SingleTon instance;
        return instance;
    }

    private:
    SingleTon();
};

class A
{
public:
    A(){}
    ~A(){}

    int a;
};

class B: virtual public A
{
public:
    B(){}
    ~B(){}

    void func(){}
    int b;
};

class C : virtual public A
{
public:
    C(){}
    ~C(){}
};

class D : public B, public C 
{
public:
    D(){}
    ~D(){}
};

// 10 9 8 7 6 5 4 3 2 1
//       10
//     9    8
//   7   6  5  4
//  3 2 1

// 1.先构建一个大顶堆
// 2.将堆顶和最后一个元素交换
// 3.堆调整
// 4.重复2，3动作

void Swap(int *a, int *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void HeapAdjust(int head, int *array, int size)
{
    int left = head * 2 + 1;
    int right = head * 2 + 2;
    printf("head:%d\tleft:%d\tright:%d\n", head, left, right);

    int max = head;
    if(left < size && array[left] > array[max])
    {
        max = left;
    }

    if(right < size && array[right] > array[max])
    {
        max = right;
    }

    if(max != head)
    {
        int tmp = array[max];
        array[max] = array[head];
        array[head] = tmp;

        HeapAdjust(max, array, size);
    }
}

void HeapSort(int *array, int size)
{
    int i;

    for(i = size>>1; i > 0; i--)
    {
        HeapAdjust(i - 1, array, size);
    }

    for(i = size - 1; i > 0; i--)
    {
        Swap(&array[0], &array[i]);
        HeapAdjust(0, array, i);
    }
}

void Print(int *array, int size)
{
    int i = 0;
    
    for(; i < size; i++)
    {
        printf("%d ", array[i]);
    }
    printf("\n");
}

const char Zero = '0';
const char One = '1';

int getChangeMinTimes(string& s)
{
	if(s.size() < 2)
	{
		return 0;
	}

	int times = 0;
	int size = s.size();
	int left, right;
	left = 0;
	right = size - 1;
	
	while(left < right)
	{
		while(left < right && s[left] == Zero) left++;
		while(left < right && s[right] == One) right--;
		
		if(left >= right)
		{
			break;
		}
		std::swap(s[left], s[right]);
		times++;
	}

	return times;
}

int main(){
    // leveldb::DB* db;
    // leveldb::Options options;
    // options.create_if_missing = true;
    // leveldb::Status status = leveldb::DB::Open(options, "./test_level_db", &db);
    // assert(status.ok());

    // std::string value;
    // status = db->Put(leveldb::WriteOptions(), "Test", "B");
    // if (status.ok()) {
    //     status = db->Put(leveldb::WriteOptions(), "Test", "C");
    //     status = db->Get(leveldb::ReadOptions(), "Test", &value);
    //     std::cout<<"value : "<<value<<std::endl;
    // } else {
    //     std::cout<<"err : "<<status.ToString()<<std::endl;
    // }

    // return 0;

    // GetResult1({{1, 0, 2, 0, 3}, {0, 2, 0, 0, 1}, {0, 3, 0, 0, 0}}, {0, 4}, {2, 1});
    // cout<<"result : "<<result1<<endl;

    // Initialize the semaphores
    // sem_init(&beginSema1, 0, 0);
    // sem_init(&beginSema2, 0, 0);
    // sem_init(&endSema, 0, 0);

    // Spawn the threads
    // pthread_t thread1, thread2;
    // cpu_set_t cpus;
    // CPU_ZERO(&cpus);
    // CPU_SET(0, &cpus);
    // pthread_setaffinity_np(thread1, sizeof(cpu_set_t), &cpus);
    // pthread_setaffinity_np(thread2, sizeof(cpu_set_t), &cpus);

    // pthread_create(&thread1, NULL, thread1Func, NULL);
    // pthread_create(&thread2, NULL, thread2Func, NULL);

    // pthread_join(thread1, NULL);
    // pthread_join(thread2, NULL);
    // printf("final value : %d\n", sharedValue);

    // Repeat the experiment ad infinitum
    // int detected = 0;
    // int found = 0;
    // for (int iterations = 1; iterations <= 1000000; iterations++)
    // {
        // Reset X and Y
        // X = 0;
        // Y = 0;
        // Signal both threads
        // sem_post(&beginSema1);
        // sem_post(&beginSema2);
        // Wait for both threads
        // sem_wait(&endSema);
        // sem_wait(&endSema);
        // Check if there was a simultaneous reorder
        // if (r1 == 0)
        // {
        //     detected++;
        //     printf("%d reorders detected after %d iterations\n", detected, iterations);
        // }
        // printf("total : %d\n", r1);

        // if (r1 == 1 && r2 == 1)
        // {
        //     found++;
        //     printf("%d part reorders detected after %d iterations\n", found, iterations);
        // }
    // }

    // int *a = new int(1);
    // AtomicPointer ptr(a);

    // f = 1.0;
    // fo = 1.2;

    // int *b = new int(2);
    // ptr.Release_Store(b);

    // foa = f + fo;
    // void* c = ptr.Acquire_Load();
    // printf("%d\n", *(int*)c);

    // vector<int> vs({5, 7, 6, 4});
    // cout<<"result : "<<Test(vs)<<endl;
    // std::string s;

    // vector<int> vs1({1,2,2,3});
    // TestQ(vs1);
    // for (auto& items : result) {
    //     cout<<"result 1 : "<<"\t";
    //     for (auto& item : items) {
    //         cout<<item<<"\t";
    //     }
    //     cout<<endl;
    // }

    string a;
    while(cin >> a)
    {
        cout<< getChangeMinTimes(a) <<endl;
    }

    return 0;  // Never returns
}