#pragma once
#include "Linxc.h"

namespace threading
{
    typedef struct ConditionVariableImpl *ConditionVariable;
    typedef struct ThreadLockImpl *ThreadLock;

    typedef struct ThreadImpl *Thread;

    ConditionVariable CreateConditionVariable();
    void DestroyConditionVariable(ConditionVariable variable);
    void SetSignalled(ConditionVariable variable);
    void SetAllSignalled(ConditionVariable variable);
    void ExitSignalled(ConditionVariable variable);
    void AwaitSignalled(ConditionVariable variable, u64 timeout);

    ThreadLock CreateThreadLock();
    void DestroyThreadLock(ThreadLock lock);
    void LockThreadLock(ThreadLock lock);
    void UnlockThreadLock(ThreadLock lock);

#ifdef POSIX
#define THREAD_RESULT void*
#endif
#ifdef WINDOWS
#define THREAD_RESULT unsigned long
#endif

    def_delegate(ThreadFunc, THREAD_RESULT, void*);
    Thread StartThread(ThreadFunc func, void *inputArgs);
    void ShutdownThread(Thread thread);
}

#ifdef ASTRALCORE_THREADING_IMPL

#include "stdlib.h"

#ifdef WINDOWS
#define WIN32_LEAN_AND_MEAN
#include "windows.h"

namespace threading
{
    typedef struct ConditionVariableImpl
    {
        CONDITION_VARIABLE handle;
        CRITICAL_SECTION criticalSection;
    } ConditionVariableImpl;
    typedef struct ThreadLockImpl
    {
        //use a critical section instead of a win32 mutex because they are lighter
        CRITICAL_SECTION handle;
    } ThreadLockImpl;
    typedef struct ThreadImpl
    {
        HANDLE handle;
    } ThreadImpl;

    ConditionVariable CreateConditionVariable()
    {
        ConditionVariableImpl result;
        InitializeConditionVariable(&result.handle);
        InitializeCriticalSection(&result.criticalSection);

        ConditionVariable ptr = (ConditionVariable)malloc(sizeof(ConditionVariableImpl));
        *ptr = result;
        return ptr;
    }
    void DestroyConditionVariable(ConditionVariable variable)
    {
        DeleteCriticalSection(&variable->criticalSection);
        free(variable);
    }
    void SetSignalled(ConditionVariable variable)
    {
        WakeConditionVariable(&variable->handle);
    }
    void SetAllSignalled(ConditionVariable variable)
    {
        WakeAllConditionVariable(&variable->handle);
    }
    void ExitSignalled(ConditionVariable variable)
    {
        LeaveCriticalSection(&variable->criticalSection);
    }
    void AwaitSignalled(ConditionVariable variable, u64 timeout)
    {
        EnterCriticalSection(&variable->criticalSection);

        if (!SleepConditionVariableCS(&variable->handle, &variable->criticalSection, timeout <= 0 ? INFINITE : timeout))
        {
            LeaveCriticalSection(&variable->criticalSection);
        }
    }

    ThreadLock CreateThreadLock()
    {
        ThreadLock result = (ThreadLock)malloc(sizeof(ThreadLockImpl));
        InitializeCriticalSection(&result->handle);
        return result;
    }
    void DestroyThreadLock(ThreadLock lock)
    {
        DeleteCriticalSection(&lock->handle);
        free(lock);
    }
    void LockThreadLock(ThreadLock lock)
    {
        EnterCriticalSection(&lock->handle);
    }
    void UnlockThreadLock(ThreadLock lock)
    {
        LeaveCriticalSection(&lock->handle);
    }
    
    Thread StartThread(ThreadFunc func, void *inputArgs)
    {
        Thread thread = (Thread)malloc(sizeof(ThreadImpl));
        thread->handle = CreateThread(NULL, 0, func, inputArgs, 0, NULL);
        return thread;
    }
    void ShutdownThread(Thread thread)
    {
        TerminateThread(thread->handle, 0);
        free(thread);
    }
}

#endif
#ifdef POSIX
#include "pthreads.h"

namespace threading
{
    typedef struct ConditionVariableImpl
    {
        pthread_cond_t handle;
        pthread_mutex_t mutex;
    } ConditionVariableImpl;
    typedef struct ThreadLockImpl
    {
        pthread_mutex_t handle;
    } ThreadLockImpl;
    typedef struct ThreadImpl
    {
        pthread_t handle;
    } ThreadImpl;

    ConditionVariable CreateConditionVariable()
    {
        ConditionVariableImpl result;
        pthread_cond_init(&result->handle, NULL);
        pthread_mutex_init(&result->mutex, NULL);

        ConditionVariable ptr = (ConditionVariable)malloc(sizeof(ConditionVariableImpl));
        *ptr = result;
        return ptr;
    }
    void DestroyConditionVariable(ConditionVariable variable)
    {
        pthread_mutex_destroy(&variable->mutex);
        pthread_cond_destroy(&variable->handle);
        free(variable);
    }
    void SetSignalled(ConditionVariable variable)
    {
        pthread_mutex_lock(&variable->mutex);
        pthread_cond_signal(&variable->handle);
        pthread_mutex_unlock(&variable->mutex);
    }
    void SetAllSignalled(ConditionVariable variable)
    {
        pthread_mutex_lock(&variable->mutex);
        pthread_cond_broadcast(&variable->handle);
        pthread_mutex_unlock(&variable->mutex);
    }
    void ExitSignalled(ConditionVariable variable)
    {
        pthread_mutex_unlock(&variable->mutex);
    }
    void AwaitSignalled(ConditionVariable variable, u64 timeout)
    {
        pthread_mutex_lock(&variable->mutex);
        if (pthread_cond_wait(&variable->handle, &variable->mutex) != 0)
        {
            pthread_mutex_unlock(&variable->mutex);
        }
    }

    ThreadLock CreateThreadLock()
    {
        ThreadLock result = (ThreadLock)malloc(sizeof(ThreadLockImpl));
        pthread_mutex_init(&result->handle, NULL);
        return result;
    }
    void DestroyThreadLock(ThreadLock lock)
    {
        pthread_mutex_destroy(&lock->handle);
        free(lock);
    }
    void LockThreadLock(ThreadLock lock)
    {
        pthread_mutex_lock(&lock->handle);
    }
    void UnlockThreadLock(ThreadLock lock)
    {
        pthread_mutex_unlock(&lock->handle);
    }

    Thread StartThread(ThreadFunc func, void *inputArgs)
    {
        Thread thread = (Thread)malloc(sizeof(ThreadImpl));
        pthread_create(&thread->handle, NULL, func, inputArgs);
        return thread;
    }
    void ShutdownThread(Thread thread)
    {
        pthread_cancel(&thread->handle);
        free(thread);
    }
}

#endif

#endif