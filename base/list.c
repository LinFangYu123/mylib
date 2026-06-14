#include "list.h"
#include <string.h>
#include <stdio.h>

/* ==================== 双向链表实现 ==================== */

/**
 * @brief 创建双向链表
 * @param free_func 数据释放函数，NULL表示不释放
 * @return 成功返回链表指针，失败返回NULL
 */
dlist_s *dlist_create(void (*free_func)(void *data))
{
    dlist_s *list = (dlist_s *)malloc(sizeof(dlist_s));
    if (!list)
        return NULL;

    dlist_init(list, free_func);

    return list;
}

/**
 * @brief 初始化双向链表（非指针版本）
 * @param list 链表指针
 * @param free_func 数据释放函数，NULL表示不释放
 */
void dlist_init(dlist_s *list, void (*free_func)(void *data))
{
    if (!list)
        return;

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    list->free_func = free_func;
}

/**
 * @brief 销毁双向链表
 * @param list 链表指针
 */
void dlist_destroy(dlist_s *list)
{
    if (!list)
        return;

    dlist_clear(list);
    free(list);
}

/**
 * @brief 清空双向链表（非指针版本）
 * @param list 链表指针
 */
void dlist_clear(dlist_s *list)
{
    if (!list)
        return;

    dlist_node_s *current = list->head;
    while (current)
    {
        dlist_node_s *next = current->next;

        if (list->free_func && current->data)
        {
            list->free_func(current->data);
        }

        free(current);
        current = next;
    }

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

/**
 * @brief 在双向链表头部插入节点
 * @param list 链表指针
 * @param data 数据指针
 * @return 成功返回0，失败返回-1
 */
int dlist_push_front(dlist_s *list, void *data)
{
    if (!list)
        return -1;

    dlist_node_s *node = (dlist_node_s *)malloc(sizeof(dlist_node_s));
    if (!node)
        return -1;

    node->data = data;
    node->prev = NULL;
    node->next = list->head;

    if (list->head)
    {
        list->head->prev = node;
    }

    list->head = node;

    if (!list->tail)
    {
        list->tail = node;
    }

    list->size++;
    return 0;
}

/**
 * @brief 在双向链表尾部插入节点
 * @param list 链表指针
 * @param data 数据指针
 * @return 成功返回0，失败返回-1
 */
int dlist_push_back(dlist_s *list, void *data)
{
    if (!list)
        return -1;

    dlist_node_s *node = (dlist_node_s *)malloc(sizeof(dlist_node_s));
    if (!node)
        return -1;

    node->data = data;
    node->next = NULL;
    node->prev = list->tail;

    if (list->tail)
    {
        list->tail->next = node;
    }

    list->tail = node;

    if (!list->head)
    {
        list->head = node;
    }

    list->size++;
    return 0;
}

/**
 * @brief 在指定节点前插入新节点
 * @param list 链表指针
 * @param node 指定节点
 * @param data 数据指针
 * @return 成功返回0，失败返回-1
 */
int dlist_insert_before(dlist_s *list, dlist_node_s *node, void *data)
{
    if (!list || !node)
        return -1;

    dlist_node_s *new_node = (dlist_node_s *)malloc(sizeof(dlist_node_s));
    if (!new_node)
        return -1;

    new_node->data = data;
    new_node->prev = node->prev;
    new_node->next = node;

    if (node->prev)
    {
        node->prev->next = new_node;
    }
    else
    {
        list->head = new_node;
    }

    node->prev = new_node;
    list->size++;
    return 0;
}

/**
 * @brief 在指定节点后插入新节点
 * @param list 链表指针
 * @param node 指定节点
 * @param data 数据指针
 * @return 成功返回0，失败返回-1
 */
int dlist_insert_after(dlist_s *list, dlist_node_s *node, void *data)
{
    if (!list || !node)
        return -1;

    dlist_node_s *new_node = (dlist_node_s *)malloc(sizeof(dlist_node_s));
    if (!new_node)
        return -1;

    new_node->data = data;
    new_node->prev = node;
    new_node->next = node->next;

    if (node->next)
    {
        node->next->prev = new_node;
    }
    else
    {
        list->tail = new_node;
    }

    node->next = new_node;
    list->size++;
    return 0;
}

/**
 * @brief 删除指定节点
 * @param list 链表指针
 * @param node 要删除的节点
 * @return 成功返回0，失败返回-1
 */
int dlist_remove_node(dlist_s *list, dlist_node_s *node)
{
    if (!list || !node)
        return -1;

    if (node->prev)
    {
        node->prev->next = node->next;
    }
    else
    {
        list->head = node->next;
    }

    if (node->next)
    {
        node->next->prev = node->prev;
    }
    else
    {
        list->tail = node->prev;
    }

    if (list->free_func && node->data)
    {
        list->free_func(node->data);
    }

    free(node);
    list->size--;
    return 0;
}

/**
 * @brief 删除并返回链表头节点数据
 * @param list 链表指针
 * @return 成功返回数据指针，失败返回NULL
 */
void *dlist_pop_front(dlist_s *list)
{
    if (!list || !list->head)
        return NULL;

    dlist_node_s *node = list->head;
    void *data = node->data;

    list->head = node->next;

    if (list->head)
    {
        list->head->prev = NULL;
    }
    else
    {
        list->tail = NULL;
    }

    free(node);
    list->size--;

    return data;
}

/**
 * @brief 删除并返回链表尾节点数据
 * @param list 链表指针
 * @return 成功返回数据指针，失败返回NULL
 */
void *dlist_pop_back(dlist_s *list)
{
    if (!list || !list->tail)
        return NULL;

    dlist_node_s *node = list->tail;
    void *data = node->data;

    list->tail = node->prev;

    if (list->tail)
    {
        list->tail->next = NULL;
    }
    else
    {
        list->head = NULL;
    }

    free(node);
    list->size--;

    return data;
}

/**
 * @brief 查找数据
 * @param list 链表指针
 * @param data 要查找的数据
 * @param compare 比较函数
 * @return 找到返回节点指针，未找到返回NULL
 */
dlist_node_s *dlist_find(dlist_s *list, void *data,
                         int (*compare)(const void *a, const void *b))
{
    if (!list || !compare)
        return NULL;

    dlist_node_s *current = list->head;
    while (current)
    {
        if (compare(current->data, data) == 0)
            return current;
        current = current->next;
    }

    return NULL;
}

/**
 * @brief 遍历链表
 * @param list 链表指针
 * @param func 遍历函数
 * @param userdata 传递给遍历函数的用户数据
 */
void dlist_foreach(dlist_s *list, void (*func)(void *data, void *userdata),
                   void *userdata)
{
    if (!list || !func)
        return;

    dlist_node_s *current = list->head;
    while (current)
    {
        func(current->data, userdata);
        current = current->next;
    }
}

/**
 * @brief 获取链表大小
 * @param list 链表指针
 * @return 链表大小
 */
size_t dlist_size(dlist_s *list)
{
    if (!list)
        return 0;
    return list->size;
}

/**
 * @brief 检查链表是否为空
 * @param list 链表指针
 * @return 空返回1，非空返回0
 */
int dlist_empty(dlist_s *list)
{
    if (!list)
        return 1;
    return list->size == 0;
}

/**
 * @brief 获取链表头节点
 * @param list 链表指针
 * @return 头节点指针，空链表返回NULL
 */
dlist_node_s *dlist_head(dlist_s *list)
{
    if (!list)
        return NULL;
    return list->head;
}

/**
 * @brief 获取链表尾节点
 * @param list 链表指针
 * @return 尾节点指针，空链表返回NULL
 */
dlist_node_s *dlist_tail(dlist_s *list)
{
    if (!list)
        return NULL;
    return list->tail;
}

/**
 * @brief 获取节点数据
 * @param node 节点指针
 * @return 节点数据指针
 */
void *dlist_node_data(dlist_node_s *node)
{
    if (!node)
        return NULL;
    return node->data;
}

/**
 * @brief 获取节点前驱
 * @param node 节点指针
 * @return 前驱节点指针
 */
dlist_node_s *dlist_node_prev(dlist_node_s *node)
{
    if (!node)
        return NULL;
    return node->prev;
}

/**
 * @brief 获取节点后继
 * @param node 节点指针
 * @return 后继节点指针
 */
dlist_node_s *dlist_node_next(dlist_node_s *node)
{
    if (!node)
        return NULL;
    return node->next;
}

/* ==================== 单向链表实现 ==================== */

/**
 * @brief 创建单向链表
 * @param free_func 数据释放函数，NULL表示不释放
 * @return 成功返回链表指针，失败返回NULL
 */
slist_s *slist_create(void (*free_func)(void *data))
{
    slist_s *list = (slist_s *)malloc(sizeof(slist_s));
    if (!list)
        return NULL;

    slist_init(list, free_func);

    return list;
}

/**
 * @brief 初始化单向链表（非指针版本）
 * @param list 链表指针
 * @param free_func 数据释放函数，NULL表示不释放
 */
void slist_init(slist_s *list, void (*free_func)(void *data))
{
    if (!list)
        return;

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    list->free_func = free_func;
}

/**
 * @brief 销毁单向链表
 * @param list 链表指针
 */
void slist_destroy(slist_s *list)
{
    if (!list)
        return;

    slist_clear(list);
    free(list);
}

/**
 * @brief 清空单向链表（非指针版本）
 * @param list 链表指针
 */
void slist_clear(slist_s *list)
{
    if (!list)
        return;

    slist_node_s *current = list->head;
    while (current)
    {
        slist_node_s *next = current->next;

        if (list->free_func && current->data)
        {
            list->free_func(current->data);
        }

        free(current);
        current = next;
    }

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

/**
 * @brief 在单向链表头部插入节点
 * @param list 链表指针
 * @param data 数据指针
 * @return 成功返回0，失败返回-1
 */
int slist_push_front(slist_s *list, void *data)
{
    if (!list)
        return -1;

    slist_node_s *node = (slist_node_s *)malloc(sizeof(slist_node_s));
    if (!node)
        return -1;

    node->data = data;
    node->next = list->head;

    list->head = node;

    if (!list->tail)
    {
        list->tail = node;
    }

    list->size++;
    return 0;
}

/**
 * @brief 在单向链表尾部插入节点
 * @param list 链表指针
 * @param data 数据指针
 * @return 成功返回0，失败返回-1
 */
int slist_push_back(slist_s *list, void *data)
{
    if (!list)
        return -1;

    slist_node_s *node = (slist_node_s *)malloc(sizeof(slist_node_s));
    if (!node)
        return -1;

    node->data = data;
    node->next = NULL;

    if (list->tail)
    {
        list->tail->next = node;
    }
    else
    {
        list->head = node;
    }

    list->tail = node;
    list->size++;
    return 0;
}

/**
 * @brief 在指定位置插入节点
 * @param list 链表指针
 * @param index 插入位置（从0开始）
 * @param data 数据指针
 * @return 成功返回0，失败返回-1
 */
int slist_insert_at(slist_s *list, size_t index, void *data)
{
    if (!list)
        return -1;

    if (index == 0)
    {
        return slist_push_front(list, data);
    }

    if (index >= list->size)
    {
        return slist_push_back(list, data);
    }

    slist_node_s *node = (slist_node_s *)malloc(sizeof(slist_node_s));
    if (!node)
        return -1;

    node->data = data;

    slist_node_s *current = list->head;
    for (size_t i = 0; i < index - 1; i++)
    {
        current = current->next;
    }

    node->next = current->next;
    current->next = node;

    list->size++;
    return 0;
}

/**
 * @brief 删除指定节点
 * @param list 链表指针
 * @param node 要删除的节点
 * @return 成功返回0，失败返回-1
 */
int slist_remove_node(slist_s *list, slist_node_s *node)
{
    if (!list || !node)
        return -1;

    if (node == list->head)
    {
        list->head = node->next;
        if (!list->head)
        {
            list->tail = NULL;
        }
    }
    else
    {
        slist_node_s *prev = list->head;
        while (prev && prev->next != node)
        {
            prev = prev->next;
        }

        if (!prev)
            return -1;

        prev->next = node->next;

        if (node == list->tail)
        {
            list->tail = prev;
        }
    }

    if (list->free_func && node->data)
    {
        list->free_func(node->data);
    }

    free(node);
    list->size--;
    return 0;
}

/**
 * @brief 删除指定位置的节点
 * @param list 链表指针
 * @param index 删除位置（从0开始）
 * @return 成功返回数据指针，失败返回NULL
 */
void *slist_remove_at(slist_s *list, size_t index)
{
    if (!list || index >= list->size)
        return NULL;

    if (index == 0)
    {
        return slist_pop_front(list);
    }

    slist_node_s *prev = list->head;
    for (size_t i = 0; i < index - 1; i++)
    {
        prev = prev->next;
    }

    slist_node_s *node = prev->next;
    void *data = node->data;

    prev->next = node->next;

    if (node == list->tail)
    {
        list->tail = prev;
    }

    free(node);
    list->size--;

    return data;
}

/**
 * @brief 删除并返回链表头节点数据
 * @param list 链表指针
 * @return 成功返回数据指针，失败返回NULL
 */
void *slist_pop_front(slist_s *list)
{
    if (!list || !list->head)
        return NULL;

    slist_node_s *node = list->head;
    void *data = node->data;

    list->head = node->next;

    if (!list->head)
    {
        list->tail = NULL;
    }

    free(node);
    list->size--;

    return data;
}

/**
 * @brief 查找数据
 * @param list 链表指针
 * @param data 要查找的数据
 * @param compare 比较函数
 * @return 找到返回节点指针，未找到返回NULL
 */
slist_node_s *slist_find(slist_s *list, void *data,
                         int (*compare)(const void *a, const void *b))
{
    if (!list || !compare)
        return NULL;

    slist_node_s *current = list->head;
    while (current)
    {
        if (compare(current->data, data) == 0)
            return current;
        current = current->next;
    }

    return NULL;
}

/**
 * @brief 遍历链表
 * @param list 链表指针
 * @param func 遍历函数
 * @param userdata 传递给遍历函数的用户数据
 */
void slist_foreach(slist_s *list, void (*func)(void *data, void *userdata),
                   void *userdata)
{
    if (!list || !func)
        return;

    slist_node_s *current = list->head;
    while (current)
    {
        func(current->data, userdata);
        current = current->next;
    }
}

/**
 * @brief 获取链表大小
 * @param list 链表指针
 * @return 链表大小
 */
size_t slist_size(slist_s *list)
{
    if (!list)
        return 0;
    return list->size;
}

/**
 * @brief 检查链表是否为空
 * @param list 链表指针
 * @return 空返回1，非空返回0
 */
int slist_empty(slist_s *list)
{
    if (!list)
        return 1;
    return list->size == 0;
}

/**
 * @brief 获取链表头节点
 * @param list 链表指针
 * @return 头节点指针，空链表返回NULL
 */
slist_node_s *slist_head(slist_s *list)
{
    if (!list)
        return NULL;
    return list->head;
}

/**
 * @brief 获取链表尾节点
 * @param list 链表指针
 * @return 尾节点指针，空链表返回NULL
 */
slist_node_s *slist_tail(slist_s *list)
{
    if (!list)
        return NULL;
    return list->tail;
}

/**
 * @brief 获取指定位置的节点
 * @param list 链表指针
 * @param index 位置索引（从0开始）
 * @return 找到返回节点指针，未找到返回NULL
 */
slist_node_s *slist_at(slist_s *list, size_t index)
{
    if (!list || index >= list->size)
        return NULL;

    slist_node_s *current = list->head;
    for (size_t i = 0; i < index; i++)
    {
        current = current->next;
    }

    return current;
}

/**
 * @brief 获取节点数据
 * @param node 节点指针
 * @return 节点数据指针
 */
void *slist_node_data(slist_node_s *node)
{
    if (!node)
        return NULL;
    return node->data;
}

/**
 * @brief 获取节点后继
 * @param node 节点指针
 * @return 后继节点指针
 */
slist_node_s *slist_node_next(slist_node_s *node)
{
    if (!node)
        return NULL;
    return node->next;
}

/**
 * @brief 反转单向链表
 * @param list 链表指针
 * @return 成功返回0，失败返回-1
 */
int slist_reverse(slist_s *list)
{
    if (!list)
        return -1;

    slist_node_s *prev = NULL;
    slist_node_s *current = list->head;
    slist_node_s *next = NULL;

    while (current)
    {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }

    list->tail = list->head;
    list->head = prev;

    return 0;
}

/**
 * @brief 检测单向链表中是否存在环
 * @param list 链表指针
 * @return 存在环返回1，不存在返回0
 */
int slist_has_cycle(slist_s *list)
{
    if (!list)
        return 0;

    slist_node_s *slow = list->head;
    slist_node_s *fast = list->head;

    while (fast && fast->next)
    {
        slow = slow->next;
        fast = fast->next->next;

        if (slow == fast)
            return 1;
    }

    return 0;
}

/**
 * @brief 合并两个有序单向链表
 * @param list1 第一个链表
 * @param list2 第二个链表
 * @param compare 比较函数
 * @return 成功返回合并后的链表，失败返回NULL
 */
slist_s *slist_merge_sorted(slist_s *list1, slist_s *list2,
                            int (*compare)(const void *a, const void *b))
{
    if (!list1 || !list2 || !compare)
        return NULL;

    slist_s *merged = slist_create(list1->free_func);
    if (!merged)
        return NULL;

    slist_node_s *p1 = list1->head;
    slist_node_s *p2 = list2->head;

    while (p1 && p2)
    {
        if (compare(p1->data, p2->data) <= 0)
        {
            slist_push_back(merged, p1->data);
            p1 = p1->next;
        }
        else
        {
            slist_push_back(merged, p2->data);
            p2 = p2->next;
        }
    }

    while (p1)
    {
        slist_push_back(merged, p1->data);
        p1 = p1->next;
    }

    while (p2)
    {
        slist_push_back(merged, p2->data);
        p2 = p2->next;
    }

    return merged;
}