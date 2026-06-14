#ifndef LIST_H
#define LIST_H

#include <stdint.h>
#include <stdlib.h>

/**
 * @file list.h
 * @brief 链表数据结构实现
 *
 * 提供双向链表和单向链表的基本操作，两种链表完全独立，不共用任何结构或函数
 */

/* ==================== 双向链表 ==================== */

/**
 * @brief 双向链表节点结构
 */
typedef struct dlist_node
{
    void *data;                    // 节点数据指针
    struct dlist_node *prev;       // 前驱节点
    struct dlist_node *next;       // 后继节点
} dlist_node_s;

/**
 * @brief 双向链表结构
 */
typedef struct dlist
{
    dlist_node_s *head;            // 头节点
    dlist_node_s *tail;            // 尾节点
    size_t size;                   // 链表大小
    void (*free_func)(void *data); // 数据释放函数
} dlist_s;

/**
 * @brief 创建双向链表
 * @param free_func 数据释放函数，NULL表示不释放
 * @return 成功返回链表指针，失败返回NULL
 */
dlist_s *dlist_create(void (*free_func)(void *data));

/**
 * @brief 初始化双向链表（非指针版本）
 * @param list 链表指针
 * @param free_func 数据释放函数，NULL表示不释放
 */
void dlist_init(dlist_s *list, void (*free_func)(void *data));

/**
 * @brief 销毁双向链表
 * @param list 链表指针
 */
void dlist_destroy(dlist_s *list);

/**
 * @brief 清空双向链表（非指针版本）
 * @param list 链表指针
 */
void dlist_clear(dlist_s *list);

/**
 * @brief 在双向链表头部插入节点
 * @param list 链表指针
 * @param data 数据指针
 * @return 成功返回0，失败返回-1
 */
int dlist_push_front(dlist_s *list, void *data);

/**
 * @brief 在双向链表尾部插入节点
 * @param list 链表指针
 * @param data 数据指针
 * @return 成功返回0，失败返回-1
 */
int dlist_push_back(dlist_s *list, void *data);

/**
 * @brief 在指定节点前插入新节点
 * @param list 链表指针
 * @param node 指定节点
 * @param data 数据指针
 * @return 成功返回0，失败返回-1
 */
int dlist_insert_before(dlist_s *list, dlist_node_s *node, void *data);

/**
 * @brief 在指定节点后插入新节点
 * @param list 链表指针
 * @param node 指定节点
 * @param data 数据指针
 * @return 成功返回0，失败返回-1
 */
int dlist_insert_after(dlist_s *list, dlist_node_s *node, void *data);

/**
 * @brief 删除指定节点
 * @param list 链表指针
 * @param node 要删除的节点
 * @return 成功返回0，失败返回-1
 */
int dlist_remove_node(dlist_s *list, dlist_node_s *node);

/**
 * @brief 删除并返回链表头节点数据
 * @param list 链表指针
 * @return 成功返回数据指针，失败返回NULL
 */
void *dlist_pop_front(dlist_s *list);

/**
 * @brief 删除并返回链表尾节点数据
 * @param list 链表指针
 * @return 成功返回数据指针，失败返回NULL
 */
void *dlist_pop_back(dlist_s *list);

/**
 * @brief 查找数据
 * @param list 链表指针
 * @param data 要查找的数据
 * @param compare 比较函数
 * @return 找到返回节点指针，未找到返回NULL
 */
dlist_node_s *dlist_find(dlist_s *list, void *data,
                         int (*compare)(const void *a, const void *b));

/**
 * @brief 遍历链表
 * @param list 链表指针
 * @param func 遍历函数
 * @param userdata 传递给遍历函数的用户数据
 */
void dlist_foreach(dlist_s *list, void (*func)(void *data, void *userdata),
                   void *userdata);

/**
 * @brief 获取链表大小
 * @param list 链表指针
 * @return 链表大小
 */
size_t dlist_size(dlist_s *list);

/**
 * @brief 检查链表是否为空
 * @param list 链表指针
 * @return 空返回1，非空返回0
 */
int dlist_empty(dlist_s *list);

/**
 * @brief 获取链表头节点
 * @param list 链表指针
 * @return 头节点指针，空链表返回NULL
 */
dlist_node_s *dlist_head(dlist_s *list);

/**
 * @brief 获取链表尾节点
 * @param list 链表指针
 * @return 尾节点指针，空链表返回NULL
 */
dlist_node_s *dlist_tail(dlist_s *list);

/**
 * @brief 获取节点数据
 * @param node 节点指针
 * @return 节点数据指针
 */
void *dlist_node_data(dlist_node_s *node);

/**
 * @brief 获取节点前驱
 * @param node 节点指针
 * @return 前驱节点指针
 */
dlist_node_s *dlist_node_prev(dlist_node_s *node);

/**
 * @brief 获取节点后继
 * @param node 节点指针
 * @return 后继节点指针
 */
dlist_node_s *dlist_node_next(dlist_node_s *node);

/* ==================== 单向链表 ==================== */

/**
 * @brief 单向链表节点结构
 */
typedef struct slist_node
{
    void *data;                    // 节点数据指针
    struct slist_node *next;       // 后继节点
} slist_node_s;

/**
 * @brief 单向链表结构
 */
typedef struct slist
{
    slist_node_s *head;            // 头节点
    slist_node_s *tail;            // 尾节点
    size_t size;                   // 链表大小
    void (*free_func)(void *data); // 数据释放函数
} slist_s;

/**
 * @brief 创建单向链表
 * @param free_func 数据释放函数，NULL表示不释放
 * @return 成功返回链表指针，失败返回NULL
 */
slist_s *slist_create(void (*free_func)(void *data));

/**
 * @brief 初始化单向链表（非指针版本）
 * @param list 链表指针
 * @param free_func 数据释放函数，NULL表示不释放
 */
void slist_init(slist_s *list, void (*free_func)(void *data));

/**
 * @brief 销毁单向链表
 * @param list 链表指针
 */
void slist_destroy(slist_s *list);

/**
 * @brief 清空单向链表（非指针版本）
 * @param list 链表指针
 */
void slist_clear(slist_s *list);

/**
 * @brief 在单向链表头部插入节点
 * @param list 链表指针
 * @param data 数据指针
 * @return 成功返回0，失败返回-1
 */
int slist_push_front(slist_s *list, void *data);

/**
 * @brief 在单向链表尾部插入节点
 * @param list 链表指针
 * @param data 数据指针
 * @return 成功返回0，失败返回-1
 */
int slist_push_back(slist_s *list, void *data);

/**
 * @brief 在指定位置插入节点
 * @param list 链表指针
 * @param index 插入位置（从0开始）
 * @param data 数据指针
 * @return 成功返回0，失败返回-1
 */
int slist_insert_at(slist_s *list, size_t index, void *data);

/**
 * @brief 删除指定节点
 * @param list 链表指针
 * @param node 要删除的节点
 * @return 成功返回0，失败返回-1
 */
int slist_remove_node(slist_s *list, slist_node_s *node);

/**
 * @brief 删除指定位置的节点
 * @param list 链表指针
 * @param index 删除位置（从0开始）
 * @return 成功返回数据指针，失败返回NULL
 */
void *slist_remove_at(slist_s *list, size_t index);

/**
 * @brief 删除并返回链表头节点数据
 * @param list 链表指针
 * @return 成功返回数据指针，失败返回NULL
 */
void *slist_pop_front(slist_s *list);

/**
 * @brief 查找数据
 * @param list 链表指针
 * @param data 要查找的数据
 * @param compare 比较函数
 * @return 找到返回节点指针，未找到返回NULL
 */
slist_node_s *slist_find(slist_s *list, void *data,
                         int (*compare)(const void *a, const void *b));

/**
 * @brief 遍历链表
 * @param list 链表指针
 * @param func 遍历函数
 * @param userdata 传递给遍历函数的用户数据
 */
void slist_foreach(slist_s *list, void (*func)(void *data, void *userdata),
                   void *userdata);

/**
 * @brief 获取链表大小
 * @param list 链表指针
 * @return 链表大小
 */
size_t slist_size(slist_s *list);

/**
 * @brief 检查链表是否为空
 * @param list 链表指针
 * @return 空返回1，非空返回0
 */
int slist_empty(slist_s *list);

/**
 * @brief 获取链表头节点
 * @param list 链表指针
 * @return 头节点指针，空链表返回NULL
 */
slist_node_s *slist_head(slist_s *list);

/**
 * @brief 获取链表尾节点
 * @param list 链表指针
 * @return 尾节点指针，空链表返回NULL
 */
slist_node_s *slist_tail(slist_s *list);

/**
 * @brief 获取指定位置的节点
 * @param list 链表指针
 * @param index 位置索引（从0开始）
 * @return 找到返回节点指针，未找到返回NULL
 */
slist_node_s *slist_at(slist_s *list, size_t index);

/**
 * @brief 获取节点数据
 * @param node 节点指针
 * @return 节点数据指针
 */
void *slist_node_data(slist_node_s *node);

/**
 * @brief 获取节点后继
 * @param node 节点指针
 * @return 后继节点指针
 */
slist_node_s *slist_node_next(slist_node_s *node);

/**
 * @brief 反转单向链表
 * @param list 链表指针
 * @return 成功返回0，失败返回-1
 */
int slist_reverse(slist_s *list);

/**
 * @brief 检测单向链表中是否存在环
 * @param list 链表指针
 * @return 存在环返回1，不存在返回0
 */
int slist_has_cycle(slist_s *list);

/**
 * @brief 合并两个有序单向链表
 * @param list1 第一个链表
 * @param list2 第二个链表
 * @param compare 比较函数
 * @return 成功返回合并后的链表，失败返回NULL
 */
slist_s *slist_merge_sorted(slist_s *list1, slist_s *list2,
                            int (*compare)(const void *a, const void *b));

#endif /* LIST_H */