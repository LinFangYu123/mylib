#ifndef RBTREE_H
#define RBTREE_H

#include <stdint.h>
#include <stdlib.h>

/**
 * @file rbtree.h
 * @brief 红黑树数据结构实现
 *
 * 提供自平衡二叉搜索树的基本操作，包括插入、删除、查找等功能
 * 红黑树特性：
 * 1. 每个节点是红色或黑色
 * 2. 根节点是黑色
 * 3. 所有叶子节点（NULL节点）是黑色
 * 4. 红色节点的两个子节点都是黑色（不能有连续红色节点）
 * 5. 从任一节点到其每个叶子的所有路径都包含相同数目的黑色节点
 */

/**
 * @brief 红黑树颜色枚举
 */
typedef enum
{
    RB_RED,     // 红色
    RB_BLACK    // 黑色
} rb_color_t;

/**
 * @brief 红黑树节点结构
 */
typedef struct rb_node
{
    void *key;                    // 节点键指针
    void *data;                   // 节点数据指针
    rb_color_t color;             // 节点颜色
    struct rb_node *left;         // 左子节点
    struct rb_node *right;        // 右子节点
    struct rb_node *parent;       // 父节点
} rb_node_s;

/**
 * @brief 红黑树结构
 */
typedef struct rb_tree
{
    rb_node_s *root;              // 根节点
    rb_node_s *nil;               // 哨兵节点（NULL的替代）
    size_t size;                  // 树的大小
    int (*compare)(const void *a, const void *b); // 键比较函数
    void (*key_free)(void *key);   // 键释放函数
    void (*data_free)(void *data); // 数据释放函数
} rb_tree_s;

/**
 * @brief 创建红黑树
 * @param compare 键比较函数
 * @param key_free 键释放函数，NULL表示不释放
 * @param data_free 数据释放函数，NULL表示不释放
 * @return 成功返回红黑树指针，失败返回NULL
 */
rb_tree_s *rb_create(int (*compare)(const void *a, const void *b),
                     void (*key_free)(void *key),
                     void (*data_free)(void *data));

/**
 * @brief 销毁红黑树
 * @param tree 红黑树指针
 */
void rb_destroy(rb_tree_s *tree);

/**
 * @brief 插入键值对
 * @param tree 红黑树指针
 * @param key 键指针
 * @param data 数据指针
 * @return 成功返回0，键已存在返回1，失败返回-1
 */
int rb_insert(rb_tree_s *tree, void *key, void *data);

/**
 * @brief 更新键值对（如果键不存在则插入）
 * @param tree 红黑树指针
 * @param key 键指针
 * @param data 数据指针
 * @param old_data 输出：旧的数据指针（如果键已存在）
 * @return 成功返回0，失败返回-1
 */
int rb_update(rb_tree_s *tree, void *key, void *data, void **old_data);

/**
 * @brief 删除指定键的节点
 * @param tree 红黑树指针
 * @param key 要删除的键
 * @return 成功返回0，键不存在返回1，失败返回-1
 */
int rb_delete(rb_tree_s *tree, void *key);

/**
 * @brief 查找指定键的节点数据
 * @param tree 红黑树指针
 * @param key 要查找的键
 * @return 找到返回数据指针，未找到返回NULL
 */
void *rb_search(rb_tree_s *tree, void *key);

/**
 * @brief 检查键是否存在
 * @param tree 红黑树指针
 * @param key 要检查的键
 * @return 存在返回1，不存在返回0
 */
int rb_contains(rb_tree_s *tree, void *key);

/**
 * @brief 获取最小键节点数据
 * @param tree 红黑树指针
 * @return 最小键对应的数据指针，空树返回NULL
 */
void *rb_minimum(rb_tree_s *tree);

/**
 * @brief 获取最大键节点数据
 * @param tree 红黑树指针
 * @return 最大键对应的数据指针，空树返回NULL
 */
void *rb_maximum(rb_tree_s *tree);

/**
 * @brief 获取前驱节点数据
 * @param tree 红黑树指针
 * @param key 指定键
 * @return 前驱节点数据指针，不存在返回NULL
 */
void *rb_predecessor(rb_tree_s *tree, void *key);

/**
 * @brief 获取后继节点数据
 * @param tree 红黑树指针
 * @param key 指定键
 * @return 后继节点数据指针，不存在返回NULL
 */
void *rb_successor(rb_tree_s *tree, void *key);

/**
 * @brief 中序遍历
 * @param tree 红黑树指针
 * @param func 遍历函数
 * @param userdata 传递给遍历函数的用户数据
 */
void rb_inorder(rb_tree_s *tree, void (*func)(void *key, void *data, void *userdata),
                void *userdata);

/**
 * @brief 前序遍历
 * @param tree 红黑树指针
 * @param func 遍历函数
 * @param userdata 传递给遍历函数的用户数据
 */
void rb_preorder(rb_tree_s *tree, void (*func)(void *key, void *data, void *userdata),
                 void *userdata);

/**
 * @brief 后序遍历
 * @param tree 红黑树指针
 * @param func 遍历函数
 * @param userdata 传递给遍历函数的用户数据
 */
void rb_postorder(rb_tree_s *tree, void (*func)(void *key, void *data, void *userdata),
                  void *userdata);

/**
 * @brief 层序遍历
 * @param tree 红黑树指针
 * @param func 遍历函数
 * @param userdata 传递给遍历函数的用户数据
 */
void rb_levelorder(rb_tree_s *tree, void (*func)(void *key, void *data, void *userdata),
                   void *userdata);

/**
 * @brief 反向中序遍历（降序）
 * @param tree 红黑树指针
 * @param func 遍历函数
 * @param userdata 传递给遍历函数的用户数据
 */
void rb_reverse_inorder(rb_tree_s *tree, void (*func)(void *key, void *data, void *userdata),
                        void *userdata);

/**
 * @brief 范围查询
 * @param tree 红黑树指针
 * @param min_key 最小键
 * @param max_key 最大键
 * @param func 遍历函数
 * @param userdata 传递给遍历函数的用户数据
 */
void rb_range_query(rb_tree_s *tree, void *min_key, void *max_key,
                    void (*func)(void *key, void *data, void *userdata),
                    void *userdata);

/**
 * @brief 统计指定范围内的键值对数量
 * @param tree 红黑树指针
 * @param min_key 最小键
 * @param max_key 最大键
 * @return 范围内的数量
 */
size_t rb_range_count(rb_tree_s *tree, void *min_key, void *max_key);

/**
 * @brief 获取树的大小
 * @param tree 红黑树指针
 * @return 树的大小
 */
size_t rb_size(rb_tree_s *tree);

/**
 * @brief 检查树是否为空
 * @param tree 红黑树指针
 * @return 空返回1，非空返回0
 */
int rb_empty(rb_tree_s *tree);

/**
 * @brief 获取树的高度
 * @param tree 红黑树指针
 * @return 树的高度
 */
int rb_height(rb_tree_s *tree);

/**
 * @brief 检查树是否为有效的红黑树
 * @param tree 红黑树指针
 * @return 有效返回1，无效返回0
 */
int rb_validate(rb_tree_s *tree);

/**
 * @brief 获取树的黑色高度
 * @param tree 红黑树指针
 * @return 黑色高度
 */
int rb_black_height(rb_tree_s *tree);

/**
 * @brief 清空树
 * @param tree 红黑树指针
 */
void rb_clear(rb_tree_s *tree);

/* 内部函数声明（用于实现） */
void rb_left_rotate(rb_tree_s *tree, rb_node_s *x);
void rb_right_rotate(rb_tree_s *tree, rb_node_s *y);
void rb_insert_fixup(rb_tree_s *tree, rb_node_s *z);
void rb_delete_fixup(rb_tree_s *tree, rb_node_s *x);
rb_node_s *rb_minimum_node(rb_node_s *node);
rb_node_s *rb_maximum_node(rb_node_s *node);
rb_node_s *rb_transplant(rb_tree_s *tree, rb_node_s *u, rb_node_s *v);
int rb_validate_helper(rb_node_s *node, rb_node_s *nil, int *black_height);

#endif /* RBTREE_H */