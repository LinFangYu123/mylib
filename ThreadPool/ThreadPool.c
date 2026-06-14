#include "ThreadPool.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

void *workercallback(void *arg)
{
    thread_pool_worker *worker = (thread_pool_worker *)arg;

    while (1)
    {
        pthread_mutex_lock(&worker->pool->mutex);

        while ((NULL == worker->pool->tasks) && (0 == worker->pool->shutdown))
        {
            pthread_cond_wait(&worker->pool->cond, &worker->pool->mutex);
        }

        if (1 == worker->pool->shutdown)
        {
            pthread_mutex_unlock(&worker->pool->mutex);
            pthread_exit(NULL);
        }

        thread_pool_task *task = worker->pool->tasks;

        if (task == NULL)
        {
            pthread_mutex_unlock(&worker->pool->mutex);
            continue;
        }

        worker->pool->tasks = worker->pool->tasks->next;

        worker->status = 1;
        worker->pool->current_thread_task_num++;
        worker->pool->current_thread_free_num--;
        worker->pool->current_wait_task_num--;

        pthread_mutex_unlock(&worker->pool->mutex);

        task->func(task->arg);

        pthread_mutex_lock(&worker->pool->mutex);
        free(task);
        worker->pool->current_thread_task_num--;
        worker->pool->current_thread_free_num++;
        worker->status = 0;

        if (worker->pool->current_thread_free_num > worker->pool->max_free_thread_num)
        {
            worker->prev->next = worker->next;
            worker->next->prev = worker->prev;
            pthread_mutex_unlock(&worker->pool->mutex);
            free(worker);
            worker = NULL;
            break;
        }

        pthread_mutex_unlock(&worker->pool->mutex);
    }
}

int32_t create_thread_pool(void **thread_pool, int32_t max_thread_num, int32_t worker_num, int32_t allow_free_num)
{
    pthread_pool **pool = (pthread_pool **)thread_pool;

    if (pool == NULL)
    {
        perror("pool malloc");
        return -1;
    }

    (*pool)->shutdown = 0;
    (*pool)->max_thread_num = max_thread_num;
    (*pool)->current_thread_task_num = 0;
    (*pool)->current_wait_task_num = 0;
    (*pool)->max_free_thread_num = allow_free_num;

    pthread_mutex_init(&(*pool)->mutex, NULL);
    pthread_cond_init(&(*pool)->cond, NULL);

    int32_t i;

    for (i = 0; i < worker_num; i++)
    {
        (*pool)->current_thread_num++;

        thread_pool_worker *worker = (thread_pool_worker *)malloc(sizeof(thread_pool_worker));

        if (worker == NULL)
        {
            perror("worker malloc");
            return -2;
        }

        memset(worker, 0, sizeof(worker));
        worker->next = NULL;
        worker->prev = NULL;
        worker->pool = *pool;

        int32_t ret = pthread_create(&worker->pthread_id, NULL, workercallback, (void *)worker);

        if (ret)
        {
            perror("pthread_create");
            free(worker);
            worker = NULL;
            return -3;
        }

        if (pthread_detach(worker->pthread_id) != 0)
        {
            free(worker);
            worker = NULL;
            break;
        }

        if ((*pool)->workers == NULL)
        {
            (*pool)->workers = worker;
            (*pool)->workers_end = worker;
        }
        else
        {
            worker->prev = (*pool)->workers_end;
            (*pool)->workers_end->next = worker;
            (*pool)->workers_end = worker;
        }
    }

    (*pool)->current_thread_free_num = ((*pool)->current_thread_num - (*pool)->current_thread_task_num);

    return 0;
}

int32_t add_task(void *thread_pool, void *(*newtask)(void *arg), void *arg)
{
    pthread_pool *pool = (pthread_pool *)thread_pool;

    if (pool == NULL)
    {
        return -1;
    }

    thread_pool_task *task = (thread_pool_task *)malloc(sizeof(thread_pool_task));

    if (task == NULL)
    {
        perror("malloc");
        return -2;
    }

    task->func = newtask;
    task->arg = arg;
    task->next = NULL;

    pthread_mutex_lock(&pool->mutex);

    if (pool->tasks == NULL)
    {
        pool->tasks = task;
        pool->tasks_end = task;
    }
    else
    {
        pool->tasks_end->next = task;
        pool->tasks_end = pool->tasks_end->next;
    }

    pool->current_wait_task_num++;

    assert(pool->tasks != NULL);

    do
    {
        if (((pool->current_thread_free_num == 0) && (pool->max_thread_num > pool->current_thread_num)))
        {
            thread_pool_worker *worker = (thread_pool_worker *)malloc(sizeof(thread_pool_worker));

            if (worker == NULL)
            {
                break;
            }

            worker->pool = pool;
            worker->next = NULL;
            worker->prev = NULL;

            int32_t ret = pthread_create(&worker->pthread_id, NULL, workercallback, (void *)worker);

            if (0 != ret)
            {
                free(worker);
                worker = NULL;
                break;
            }

            if (pthread_detach(worker->pthread_id) != 0)
            {
                free(worker);
                worker = NULL;
                break;
            }

            pool->workers_end->next = worker;
            worker->prev = pool->workers_end;
            pool->workers_end = pool->workers_end->next;

            pool->current_thread_num++;
            pool->current_thread_free_num++;
        }
    } while (0);

    pthread_mutex_unlock(&pool->mutex);
    pthread_cond_signal(&pool->cond);

    return 0;
}

int32_t thread_pool_destroy(void *thread_pool)
{
    int32_t i;
    pthread_pool *pool = (pthread_pool *)thread_pool;

    if (pool == NULL)
    {
        return -1;
    }

    if (1 == pool->shutdown)
    {
        return -1;
    }

    pool->shutdown = 1;

    pthread_cond_broadcast(&pool->cond);

    thread_pool_worker *worker = NULL;

    while (NULL != pool->workers)
    {
        worker = pool->workers;
        pool->workers = pool->workers->next;

        while (1 == worker->status)
        {
            usleep(200);
        }

        free(worker);
        worker = NULL;
    }

    thread_pool_task *task = NULL;

    while (NULL != pool->tasks)
    {
        task = pool->tasks;
        pool->tasks = pool->tasks->next;
        free(task);
        task = NULL;
    }

    pthread_mutex_destroy(&pool->mutex);
    pthread_cond_destroy(&pool->cond);
    free(pool);
    pool = NULL;

    return 0;
}

int32_t get_max_thread_num(void *thread_pool)
{
    int32_t num;
    pthread_pool *pool = (pthread_pool *)thread_pool;

    pthread_mutex_lock(&pool->mutex);
    num = pool->max_thread_num;
    pthread_mutex_unlock(&pool->mutex);

    return num;
}

int32_t get_current_thread_num(void *thread_pool)
{
    int32_t num;
    pthread_pool *pool = (pthread_pool *)thread_pool;

    pthread_mutex_lock(&pool->mutex);
    num = pool->current_thread_num;
    pthread_mutex_unlock(&pool->mutex);

    return num;
}

int32_t get_current_thread_task_num(void *thread_pool)
{
    int32_t num;
    pthread_pool *pool = (pthread_pool *)thread_pool;

    pthread_mutex_lock(&pool->mutex);
    num = pool->current_thread_task_num;
    pthread_mutex_unlock(&pool->mutex);

    return num;
}

int32_t get_current_thread_free_num(void *thread_pool)
{
    int32_t num;
    pthread_pool *pool = (pthread_pool *)thread_pool;

    pthread_mutex_lock(&pool->mutex);
    num = pool->current_thread_free_num;
    pthread_mutex_unlock(&pool->mutex);

    return num;
}

int32_t get_current_wait_task_num(void *thread_pool)
{
    int32_t num;
    pthread_pool *pool = (pthread_pool *)thread_pool;

    pthread_mutex_lock(&pool->mutex);
    num = pool->current_wait_task_num;
    pthread_mutex_unlock(&pool->mutex);

    return num;
}

int32_t get_max_free_thread_num(void *thread_pool)
{
    int32_t num;
    pthread_pool *pool = (pthread_pool *)thread_pool;

    pthread_mutex_lock(&pool->mutex);
    num = pool->max_free_thread_num;
    pthread_mutex_unlock(&pool->mutex);

    return num;
}