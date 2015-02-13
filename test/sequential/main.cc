#include "testobject.ph"
#include <unistd.h>
#define TIME_SCALE 100000

int main(int argc, char** argv) {
    TestObject obj1("localhost");

    printf("Test with synchronous methods\n");
    obj1.syncConcMethod("call1", 1.0 * TIME_SCALE);
    obj1.syncConcMethod("call2", 0.8 * TIME_SCALE);
    obj1.syncConcMethod("call3", 0.0 * TIME_SCALE);

    obj1.syncMutexMethod("-----", 0.0 * TIME_SCALE);

    obj1.syncSeqMethod("call1", 1.0 * TIME_SCALE);
    obj1.syncSeqMethod("call2", 0.8 * TIME_SCALE);
    obj1.syncSeqMethod("call3", 0.0 * TIME_SCALE);

    obj1.syncMutexMethod("-----", 0.0 * TIME_SCALE);

    obj1.syncMutexMethod("call1", 1.0 * TIME_SCALE);
    obj1.syncMutexMethod("call2", 0.8 * TIME_SCALE);
    obj1.syncMutexMethod("call3", 0.0 * TIME_SCALE);

    obj1.syncMutexMethod("-----", 0.0 * TIME_SCALE);

    printf("Test with asynchronous methods\n");
    obj1.asyncConcMethod("call1", 1.5 * TIME_SCALE);
    usleep(1.0 * TIME_SCALE);
    obj1.asyncConcMethod("call2", 0.8 * TIME_SCALE);
    usleep(1.0 * TIME_SCALE);
    obj1.asyncConcMethod("call3", 0.4 * TIME_SCALE);

    obj1.syncMutexMethod("-----", 0.0 * TIME_SCALE);

    obj1.asyncSeqMethod("call1", 1.0 * TIME_SCALE);
    obj1.asyncSeqMethod("call2", 0.8 * TIME_SCALE);
    obj1.asyncConcMethod("call3", 0.1 * TIME_SCALE);
    obj1.asyncSeqMethod("call4", 0.0 * TIME_SCALE);

    obj1.syncMutexMethod("-----", 0.0 * TIME_SCALE);

    obj1.asyncMutexMethod("call1", 1.0 * TIME_SCALE);
    obj1.asyncSeqMethod("call2", 0.8 * TIME_SCALE);
    obj1.asyncMutexMethod("call3", 0.8 * TIME_SCALE);
    obj1.asyncConcMethod("call4", 0.8 * TIME_SCALE);
    obj1.asyncMutexMethod("call5", 0.0 * TIME_SCALE);

    return 0;
}
