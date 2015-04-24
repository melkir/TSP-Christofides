#include "../header/threads.h"

void Thread::start() {
    int r;
    if ((r = pthread_create(&_id, NULL, &Thread::exec, this)) != 0) {
        cout << strerror(r) << endl;
        throw "Error";
    }
}

/** Allow thread to wait for the termination status */
void Thread::join() {
    void *status;
    int r = pthread_join(_id, &status);
    if (r) {
        printf("ERROR in thread %d; return code from pthread_join() is %d\n", my_id, r);
        exit(-1);
    }
}

/** Function that is to be executed by the thread */
void *Thread::exec(void *thr) {
    reinterpret_cast<Thread *>(thr)->run();
    return NULL;
}
