#pragma once
#include "Linxc.h"

#ifdef WINDOWS
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif
#ifdef POSIX
#include "pthreads.h" //to test
#endif

namespace threading
{
#ifdef POSIX
    struct ConditionVariable
    {
        pthread_cond_t handle;
        pthread_mutex_t mutex;

        inline ConditionVariable()
        {
            pthread_cond_init(&handle, NULL);
            pthread_mutex_init(&mutex, NULL);
        }
        inline void deinit()
        {
            pthread_mutex_destroy(&mutex);
            pthread_cond_destroy(&handle);
        }

        inline bool AwaitSignalled(i64 timeout)
        {
            pthread_mutex_lock(&mutex);
            if (pthread_cond_wait(&handle, &mutex) != 0)
            {
                return false;
            }
            return true;
        }
        inline bool ExitSignalled()
        {
            pthread_mutex_unlock(&mutex);
        }
        inline void SetSingleSignalled()
        {
            pthread_mutex_lock(&mutex);
            pthread_cond_signal(&handle);
            pthread_mutex_unlock(&mutex);
        }
        inline void SetAllSignalled()
        {
            pthread_mutex_lock(&mutex);
            pthread_cond_broadcast(&handle);
            pthread_mutex_unlock(&mutex);
        }
    };
#endif

    struct ConditionVariable
    {
        CONDITION_VARIABLE handle;
        CRITICAL_SECTION criticalSection;

        inline ConditionVariable()
        {
            InitializeConditionVariable(&handle);
            InitializeCriticalSection(&criticalSection);
        }

        inline bool AwaitSignalled(i64 timeout)
        {
            EnterCriticalSection(&criticalSection);

            if (!SleepConditionVariableCS(&handle, &criticalSection, timeout <= 0 ? INFINITE : timeout))
            {
                return false;
            }
            return true;
            
        }
        inline bool ExitSignalled()
        {
            LeaveCriticalSection(&criticalSection);
        }

        inline void SetSingleSignalled()
        {
            WakeConditionVariable(&handle);
        }
        inline void SetAllSignalled()
        {
            WakeAllConditionVariable(&handle);
        }
        inline void deinit()
        {
            DeleteCriticalSection(&criticalSection);
        }
    };

#ifdef POSIX
    struct Mutex
    {
        pthread_mutex_t handle;

        inline Mutex()
        {
            handle = pthread_mutex_init(&handle, NULL);
        }
        inline bool EnterLock()
        {
            if (pthread_mutex_lock(&handle) == 0)
            {
                return true;
            }
            return false;
        }
        inline bool ExitLock()
        {
            if (pthread_mutex_unlock(&handle) == 0)
            {
                return true;
            }
            return false;
        }
        inline void deinit()
        {
            if (handle != NULL)
                pthread_mutex_destroy(&handle);
        }
    };
#endif

#ifdef WINDOWS
    struct Mutex
    {
        //use a critical section instead of a win32 mutex because they are lighter
        CRITICAL_SECTION handle;

        inline Mutex()
        {
            handle = {};
        }
        inline static Mutex init()
        {
            Mutex result = Mutex();
            InitializeCriticalSection(&result.handle);
            return result;
        }
        inline void deinit()
        {
            DeleteCriticalSection(&handle);
        }

        inline bool EnterLock()
        {
            EnterCriticalSection(&handle);
            return true;
        }
        inline bool ExitLock()
        {
            LeaveCriticalSection(&handle);
            return true;
        }
    };
#endif

#ifdef POSIX
#define THREAD_RESULT void*
    def_delegate(ThreadFunc, THREAD_RESULT, void*);
    typedef pthread_t Thread;
    inline Thread NewThread(ThreadFunc func, void *inputArgs)
    {
        Thread threadID;
        pthread_create(&threadID, NULL, func, inputArgs);
        return threadID;
    }
#endif

#ifdef WINDOWS
#define THREAD_RESULT unsigned long
    def_delegate(ThreadFunc, THREAD_RESULT, void*);
    typedef HANDLE Thread;
    inline Thread NewThread(ThreadFunc func, void *inputArgs)
    {
        return CreateThread(NULL, 0, func, inputArgs, 0, NULL);
    }
#endif
}