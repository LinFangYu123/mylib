#ifndef AVL_H
#define AVL_H

#include <stdint.h>
#include <stdlib.h>

/**
 * @file avl.h
 * @brief AVL树数据结构实现
 *
 * 提供自平衡二叉搜索树的基本操作，包括插入、删除、查找等功能
 */

/**
 * @brief AVL树节点结构
 */
typedef struct avl_node
{
    void *key;                    // 节点键指针
    void *data;                   // 节点数据指针
    int height;                   // 节点高度
    struct avl_node *left;        // 左子节点
    struct avl_node *right;       // 右子节点
    struct avl_node *parent;      // 父节点
} avl_node_t;

/**
 * @brief AVL树结构
 */
typedef struct avl_tree
{
    avl_node_t *root;             // 根节点
    size_t size;                  // 树的大小
    int (*compare)(const void *a, const void *b); // 键比较函数
    void (*key_free)(void *key);   // 键释放函数
    void (*data_free)(void *data); // 数据释放函数
} avl_tree_t;

/**
 * @brief 创建AVL树
 * @param compare 键比较函数
 * @param key_free 键释放函数，NULL表示不释放
 * @param data_free 数据释放函数，NULL表示不释放
 * @return 成功返回AVL树指针，失败返回NULL
 */
avl_tree_t *avl_create(int (*compare)(const void *a, const void *b),
                      void (*key_free)(void *key),
                      void (*data_free)(void *data));

/**
 * @brief 销毁AVL树
 * @param tree AVL树指针
 */
void avl_destroy(avl_tree_t *tree);

/**
 * @brief 插入键值对
 * @param tree AVL树指针
 * @param key 键指针
 * @param data 数据指针
 * @return 成功返回0，键已存在返回1，失败返回-1
 */
int avl_insert(avl_tree_t *tree, void *key, void *data);

/**
 * @brief 删除指定键的节点
 * @param tree AVL树指针
 * @param key 要删除的键
 * @return 成功返回0，键不存在返回1，失败返回-1
 */
int avl_delete(avl_tree_t *tree, void *key);

/**
 * @brief 查找指定键的节点数据
 * @param tree AVL树指针
 * @param key 要查找的键
 * @return 找到返回数据指针，未找到返回NULL
 */
void *avl_search(avl_tree_t *tree, void *key);

/**
 * @brief 检查键是否存在
 * @param tree AVL树指针
 * @param key 要检查的键
 * @return 存在返回1，不存在返回0
 */
int avl_contains(avl_tree_t *tree, void *key);

/**
 * @brief 获取最小键节点数据
 * @param tree AVL树指针
 * @return 最小键对应的数据指针，空树返回NULL
 */
void *avl_minimum(avl_tree_t *tree);

/**
 * @brief 获取最大键节点数据
 * @param tree AVL树指针
 * @return 最大键对应的数据指针，空树返回NULL
 */
void *avl_maximum(avl_tree_t *tree);

/**
 * @brief 获取前驱节点数据
 * @param tree AVL树指针
 * @param key 指定键
 * @return 前驱节点数据指针，不存在返回NULL
 */
void *avl_predecessor(avl_tree_t *tree, void *key);

/**
 * @brief 获取后继节点数据
 * @param tree AVL树指针
 * @param key 指定键
 * @return 后继节点数据指针，不存在返回NULL
 */
void *avl_successor(avl_tree_t *tree, void *key);

/**
 * @brief 中序遍历
 * @param tree AVL树指针
 * @param func 遍历函数
 * @param userdata 传递给遍历函数的用户数据
 */
void avl_inorder(avl_tree_t *tree, void (*func)(void *key, void *data, void *userdata),
                void *userdata);

/**
 * @brief 前序遍历
 * @param tree AVL树指针
 * @param func 遍历函数
 * @param userdata 传递给遍历函数的用户数据
 */
void avl_preorder(avl_tree_t *tree, void (*func)(void *key, void *data, void *userdata),
                 void *userdata);

/**
 * @brief 后序遍历
 * @param tree AVL树指针
 * @param func 遍历函数
 * @param userdata 传递给遍历函数的用户数据
 */
void avl_postorder(avl_tree_t *tree, void (*func)(void *key, void *data, void *userdata),
                  void *userdata);

/**
 * @brief 层序遍历
 * @param tree AVL树指针
 * @param func 遍历函数
 * @param userdata 传递给遍历函数的用户数据
 */
void avl_levelorder(avl_tree_t *tree, void (*func)(void *key, void *data, void *userdata),
                   void *userdata);

/**
 * @brief 获取树的大小
 * @param tree AVL树指针
 * @return 树的大小
 */
size_t avl_size(avl_tree_t *tree);

/**
 * @brief 检查树是否为空
 * @param tree AVL树指针
 * @return 空返回1，非空返回0
 */
int avl_empty(avl_tree_t *tree);

/**
 * @brief 获取树的高度
 * @param tree AVL树指针
 * @return 树的高度
 */
int avl_height(avl_tree_t *tree);

/**
 * @brief 检查树是否为有效的AVL树
 * @param tree AVL树指针
 * @return 有效返回1，无效返回0
 */
int avl_validate(avl_tree_t *tree);

/* 内部函数声明（用于实现） */
int avl_node_height(avl_node_t *node);
int avl_balance_factor(avl_node_t *node);
avl_node_t *avl_rotate_left(avl_tree_t *tree, avl_node_t *node);
avl_node_t *avl_rotate_right(avl_tree_t *tree, avl_node_t *node);
avl_node_t *avl_rebalance(avl_tree_t *tree, avl_node_t *node);
avl_node_t *avl_minimum_node(avl_node_t *node);
avl_node_t *avl_maximum_node(avl_node_t *node);
void avl_free_nodes(avl_node_t *node, void (*key_free)(void *), void (*data_free)(void *));

#endif /* AVL_H */