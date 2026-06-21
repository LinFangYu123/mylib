#include "avl.h"
#include <stdio.h>
#include <string.h>

/* ==================== 内部辅助函数 ==================== */

/**
 * @brief 获取节点高度
 * @param node 节点指针
 * @return 节点高度，空节点返回0
 */
int avl_node_height(avl_node_t *node)
{
    if (!node)
        return 0;
    return node->height;
}

/**
 * @brief 获取两个数中的最大值
 * @param a 第一个数
 * @param b 第二个数
 * @return 较大的数
 */
static inline int max_int(int a, int b)
{
    return (a > b) ? a : b;
}

/**
 * @brief 更新节点高度
 * @param node 节点指针
 */
static inline void avl_update_height(avl_node_t *node)
{
    if (node)
    {
        node->height = 1 + max_int(avl_node_height(node->left),
                                   avl_node_height(node->right));
    }
}

/**
 * @brief 计算节点的平衡因子
 * @param node 节点指针
 * @return 平衡因子（左子树高度 - 右子树高度）
 */
int avl_balance_factor(avl_node_t *node)
{
    if (!node)
        return 0;
    return avl_node_height(node->left) - avl_node_height(node->right);
}

/**
 * @brief 左旋转
 * @param tree AVL树指针
 * @param node 要旋转的节点
 * @return 旋转后的新根节点
 */
avl_node_t *avl_rotate_left(avl_tree_t *tree, avl_node_t *node)
{
    if (!node || !node->right)
        return node;

    avl_node_t *right = node->right;
    avl_node_t *right_left = right->left;

    /* 执行旋转 */
    right->left = node;
    node->right = right_left;

    /* 更新父指针 */
    if (right_left)
        right_left->parent = node;
    right->parent = node->parent;
    node->parent = right;

    /* 更新树根 */
    if (node == tree->root)
        tree->root = right;
    else if (right->parent)
    {
        if (right->parent->left == node)
            right->parent->left = right;
        else
            right->parent->right = right;
    }

    /* 更新高度 */
    avl_update_height(node);
    avl_update_height(right);

    return right;
}

/**
 * @brief 右旋转
 * @param tree AVL树指针
 * @param node 要旋转的节点
 * @return 旋转后的新根节点
 */
avl_node_t *avl_rotate_right(avl_tree_t *tree, avl_node_t *node)
{
    if (!node || !node->left)
        return node;

    avl_node_t *left = node->left;
    avl_node_t *left_right = left->right;

    /* 执行旋转 */
    left->right = node;
    node->left = left_right;

    /* 更新父指针 */
    if (left_right)
        left_right->parent = node;
    left->parent = node->parent;
    node->parent = left;

    /* 更新树根 */
    if (node == tree->root)
        tree->root = left;
    else if (left->parent)
    {
        if (left->parent->left == node)
            left->parent->left = left;
        else
            left->parent->right = left;
    }

    /* 更新高度 */
    avl_update_height(node);
    avl_update_height(left);

    return left;
}

/**
 * @brief 重新平衡节点
 * @param tree AVL树指针
 * @param node 需要平衡的节点
 * @return 平衡后的根节点
 */
avl_node_t *avl_rebalance(avl_tree_t *tree, avl_node_t *node)
{
    if (!node)
        return NULL;

    /* 更新高度 */
    avl_update_height(node);

    /* 计算平衡因子 */
    int balance = avl_balance_factor(node);

    /* 左左情况 - 右旋转 */
    if (balance > 1 && avl_balance_factor(node->left) >= 0)
        return avl_rotate_right(tree, node);

    /* 右右情况 - 左旋转 */
    if (balance < -1 && avl_balance_factor(node->right) <= 0)
        return avl_rotate_left(tree, node);

    /* 左右情况 - 先左旋转再右旋转 */
    if (balance > 1 && avl_balance_factor(node->left) < 0)
    {
        node->left = avl_rotate_left(tree, node->left);
        return avl_rotate_right(tree, node);
    }

    /* 右左情况 - 先右旋转再左旋转 */
    if (balance < -1 && avl_balance_factor(node->right) > 0)
    {
        node->right = avl_rotate_right(tree, node->right);
        return avl_rotate_left(tree, node);
    }

    return node;
}

/**
 * @brief 查找最小节点
 * @param node 起始节点
 * @return 最小节点指针
 */
avl_node_t *avl_minimum_node(avl_node_t *node)
{
    if (!node)
        return NULL;

    while (node->left)
        node = node->left;

    return node;
}

/**
 * @brief 查找最大节点
 * @param node 起始节点
 * @return 最大节点指针
 */
avl_node_t *avl_maximum_node(avl_node_t *node)
{
    if (!node)
        return NULL;

    while (node->right)
        node = node->right;

    return node;
}

/**
 * @brief 递归释放节点
 * @param node 当前节点
 * @param key_free 键释放函数
 * @param data_free 数据释放函数
 */
void avl_free_nodes(avl_node_t *node, void (*key_free)(void *), void (*data_free)(void *))
{
    if (!node)
        return;

    avl_free_nodes(node->left, key_free, data_free);
    avl_free_nodes(node->right, key_free, data_free);

    if (key_free && node->key)
        key_free(node->key);
    if (data_free && node->data)
        data_free(node->data);

    free(node);
}

/* ==================== 公共接口函数 ==================== */

/**
 * @brief 创建AVL树
 * @param compare 键比较函数
 * @param key_free 键释放函数，NULL表示不释放
 * @param data_free 数据释放函数，NULL表示不释放
 * @return 成功返回AVL树指针，失败返回NULL
 */
avl_tree_t *avl_create(int (*compare)(const void *a, const void *b),
                       void (*key_free)(void *key),
                       void (*data_free)(void *data))
{
    if (!compare)
        return NULL;

    avl_tree_t *tree = (avl_tree_t *)malloc(sizeof(avl_tree_t));
    if (!tree)
        return NULL;

    tree->root = NULL;
    tree->size = 0;
    tree->compare = compare;
    tree->key_free = key_free;
    tree->data_free = data_free;

    return tree;
}

/**
 * @brief 销毁AVL树
 * @param tree AVL树指针
 */
void avl_destroy(avl_tree_t *tree)
{
    if (!tree)
        return;

    avl_free_nodes(tree->root, tree->key_free, tree->data_free);
    free(tree);
}

/**
 * @brief 插入键值对
 * @param tree AVL树指针
 * @param key 键指针
 * @param data 数据指针
 * @return 成功返回0，键已存在返回1，失败返回-1
 */
int avl_insert(avl_tree_t *tree, void *key, void *data)
{
    if (!tree || !key)
        return -1;

    /* 如果树为空，创建根节点 */
    if (!tree->root)
    {
        avl_node_t *node = (avl_node_t *)malloc(sizeof(avl_node_t));
        if (!node)
            return -1;

        node->key = key;
        node->data = data;
        node->height = 1;
        node->left = NULL;
        node->right = NULL;
        node->parent = NULL;

        tree->root = node;
        tree->size++;
        return 0;
    }

    /* 查找插入位置 */
    avl_node_t *current = tree->root;
    avl_node_t *parent = NULL;

    while (current)
    {
        int cmp = tree->compare(key, current->key);

        if (cmp < 0)
        {
            parent = current;
            current = current->left;
        }
        else if (cmp > 0)
        {
            parent = current;
            current = current->right;
        }
        else
        {
            /* 键已存在 */
            return 1;
        }
    }

    /* 创建新节点 */
    avl_node_t *node = (avl_node_t *)malloc(sizeof(avl_node_t));
    if (!node)
        return -1;

    node->key = key;
    node->data = data;
    node->height = 1;
    node->left = NULL;
    node->right = NULL;
    node->parent = parent;

    /* 插入到父节点 */
    if (tree->compare(key, parent->key) < 0)
        parent->left = node;
    else
        parent->right = node;

    tree->size++;

    /* 从新节点向上平衡 */
    current = parent;
    while (current)
    {
        avl_rebalance(tree, current);
        current = current->parent;
    }

    return 0;
}

/**
 * @brief 删除指定键的节点
 * @param tree AVL树指针
 * @param key 要删除的键
 * @return 成功返回0，键不存在返回1，失败返回-1
 */
int avl_delete(avl_tree_t *tree, void *key)
{
    if (!tree || !key)
        return -1;

    /* 查找要删除的节点 */
    avl_node_t *node = tree->root;
    while (node)
    {
        int cmp = tree->compare(key, node->key);

        if (cmp < 0)
            node = node->left;
        else if (cmp > 0)
            node = node->right;
        else
            break;
    }

    if (!node)
        return 1; /* 键不存在 */

    /* 释放键和数据 */
    if (tree->key_free && node->key)
        tree->key_free(node->key);
    if (tree->data_free && node->data)
        tree->data_free(node->data);

    avl_node_t *rebalance_start = node->parent;

    /* 情况1: 无子节点 */
    if (!node->left && !node->right)
    {
        if (node == tree->root)
            tree->root = NULL;
        else if (node->parent->left == node)
            node->parent->left = NULL;
        else
            node->parent->right = NULL;

        free(node);
    }
    /* 情况2: 有一个子节点 */
    else if (!node->left || !node->right)
    {
        avl_node_t *child = node->left ? node->left : node->right;
        child->parent = node->parent;

        if (node == tree->root)
            tree->root = child;
        else if (node->parent->left == node)
            node->parent->left = child;
        else
            node->parent->right = child;

        free(node);
    }
    /* 情况3: 有两个子节点 */
    else
    {
        /* 找到后继节点（右子树的最小节点） */
        avl_node_t *successor = avl_minimum_node(node->right);

        /* 保存后继节点的键和数据 */
        void *suc_key = successor->key;
        void *suc_data = successor->data;

        /* 记录重新平衡的起点 */
        rebalance_start = successor->parent == node ? node : successor->parent;

        /* 后继节点最多只有一个右子节点 */
        avl_node_t *suc_child = successor->right;
        if (suc_child)
            suc_child->parent = successor->parent;

        if (successor->parent->left == successor)
            successor->parent->left = suc_child;
        else
            successor->parent->right = suc_child;

        /* 用后继节点的键和数据替换当前节点 */
        node->key = suc_key;
        node->data = suc_data;

        free(successor);
    }

    tree->size--;

    /* 从平衡起点向上重新平衡 */
    while (rebalance_start)
    {
        avl_rebalance(tree, rebalance_start);
        rebalance_start = rebalance_start->parent;
    }

    return 0;
}

/**
 * @brief 查找指定键的节点数据
 * @param tree AVL树指针
 * @param key 要查找的键
 * @return 找到返回数据指针，未找到返回NULL
 */
void *avl_search(avl_tree_t *tree, void *key)
{
    if (!tree || !key)
        return NULL;

    avl_node_t *current = tree->root;
    while (current)
    {
        int cmp = tree->compare(key, current->key);

        if (cmp < 0)
            current = current->left;
        else if (cmp > 0)
            current = current->right;
        else
            return current->data;
    }

    return NULL;
}

/**
 * @brief 检查键是否存在
 * @param tree AVL树指针
 * @param key 要检查的键
 * @return 存在返回1，不存在返回0
 */
int avl_contains(avl_tree_t *tree, void *key)
{
    return avl_search(tree, key) != NULL;
}

/**
 * @brief 获取最小键节点数据
 * @param tree AVL树指针
 * @return 最小键对应的数据指针，空树返回NULL
 */
void *avl_minimum(avl_tree_t *tree)
{
    if (!tree || !tree->root)
        return NULL;

    avl_node_t *node = avl_minimum_node(tree->root);
    return node ? node->data : NULL;
}

/**
 * @brief 获取最大键节点数据
 * @param tree AVL树指针
 * @return 最大键对应的数据指针，空树返回NULL
 */
void *avl_maximum(avl_tree_t *tree)
{
    if (!tree || !tree->root)
        return NULL;

    avl_node_t *node = avl_maximum_node(tree->root);
    return node ? node->data : NULL;
}

/**
 * @brief 获取前驱节点数据
 * @param tree AVL树指针
 * @param key 指定键
 * @return 前驱节点数据指针，不存在返回NULL
 */
void *avl_predecessor(avl_tree_t *tree, void *key)
{
    if (!tree || !key || !tree->root)
        return NULL;

    /* 查找节点 */
    avl_node_t *node = tree->root;
    avl_node_t *predecessor = NULL;

    while (node)
    {
        int cmp = tree->compare(key, node->key);

        if (cmp < 0)
        {
            node = node->left;
        }
        else if (cmp > 0)
        {
            predecessor = node;
            node = node->right;
        }
        else
        {
            /* 找到节点，找前驱 */
            if (node->left)
            {
                predecessor = avl_maximum_node(node->left);
            }
            break;
        }
    }

    return predecessor ? predecessor->data : NULL;
}

/**
 * @brief 获取后继节点数据
 * @param tree AVL树指针
 * @param key 指定键
 * @return 后继节点数据指针，不存在返回NULL
 */
void *avl_successor(avl_tree_t *tree, void *key)
{
    if (!tree || !key || !tree->root)
        return NULL;

    /* 查找节点 */
    avl_node_t *node = tree->root;
    avl_node_t *successor = NULL;

    while (node)
    {
        int cmp = tree->compare(key, node->key);

        if (cmp < 0)
        {
            successor = node;
            node = node->left;
        }
        else if (cmp > 0)
        {
            node = node->right;
        }
        else
        {
            /* 找到节点，找后继 */
            if (node->right)
            {
                successor = avl_minimum_node(node->right);
            }
            break;
        }
    }

    return successor ? successor->data : NULL;
}

/**
 * @brief 中序遍历
 * @param tree AVL树指针
 * @param func 遍历函数
 * @param userdata 传递给遍历函数的用户数据
 */
void avl_inorder(avl_tree_t *tree, void (*func)(void *key, void *data, void *userdata),
                 void *userdata)
{
    if (!tree || !func || !tree->root)
        return;

    /* 使用栈进行迭代中序遍历 */
    avl_node_t *stack[128];
    int top = -1;
    avl_node_t *current = tree->root;

    while (current != NULL || top >= 0)
    {
        while (current != NULL)
        {
            stack[++top] = current;
            current = current->left;
        }

        current = stack[top--];
        func(current->key, current->data, userdata);
        current = current->right;
    }
}

/**
 * @brief 前序遍历
 * @param tree AVL树指针
 * @param func 遍历函数
 * @param userdata 传递给遍历函数的用户数据
 */
void avl_preorder(avl_tree_t *tree, void (*func)(void *key, void *data, void *userdata),
                  void *userdata)
{
    if (!tree || !func || !tree->root)
        return;

    /* 使用栈进行迭代前序遍历 */
    avl_node_t *stack[128];
    int top = -1;
    avl_node_t *current = tree->root;

    stack[++top] = current;

    while (top >= 0)
    {
        current = stack[top--];
        func(current->key, current->data, userdata);

        if (current->right)
            stack[++top] = current->right;
        if (current->left)
            stack[++top] = current->left;
    }
}

/**
 * @brief 后序遍历
 * @param tree AVL树指针
 * @param func 遍历函数
 * @param userdata 传递给遍历函数的用户数据
 */
void avl_postorder(avl_tree_t *tree, void (*func)(void *key, void *data, void *userdata),
                   void *userdata)
{
    if (!tree || !func || !tree->root)
        return;

    /* 使用栈进行迭代后序遍历 */
    avl_node_t *stack1[128];
    avl_node_t *stack2[128];
    int top1 = -1, top2 = -1;
    avl_node_t *current = tree->root;

    stack1[++top1] = current;

    while (top1 >= 0)
    {
        current = stack1[top1--];
        stack2[++top2] = current;

        if (current->left)
            stack1[++top1] = current->left;
        if (current->right)
            stack1[++top1] = current->right;
    }

    while (top2 >= 0)
    {
        current = stack2[top2--];
        func(current->key, current->data, userdata);
    }
}

/**
 * @brief 层序遍历
 * @param tree AVL树指针
 * @param func 遍历函数
 * @param userdata 传递给遍历函数的用户数据
 */
void avl_levelorder(avl_tree_t *tree, void (*func)(void *key, void *data, void *userdata),
                    void *userdata)
{
    if (!tree || !func || !tree->root)
        return;

    /* 使用队列进行层序遍历 */
    avl_node_t *queue[128];
    int front = 0, rear = 0;
    queue[rear++] = tree->root;

    while (front < rear)
    {
        avl_node_t *current = queue[front++];
        func(current->key, current->data, userdata);

        if (current->left)
            queue[rear++] = current->left;
        if (current->right)
            queue[rear++] = current->right;
    }
}

/**
 * @brief 获取树的大小
 * @param tree AVL树指针
 * @return 树的大小
 */
size_t avl_size(avl_tree_t *tree)
{
    if (!tree)
        return 0;
    return tree->size;
}

/**
 * @brief 检查树是否为空
 * @param tree AVL树指针
 * @return 空返回1，非空返回0
 */
int avl_empty(avl_tree_t *tree)
{
    if (!tree)
        return 1;
    return tree->size == 0;
}

/**
 * @brief 获取树的高度
 * @param tree AVL树指针
 * @return 树的高度
 */
int avl_height(avl_tree_t *tree)
{
    if (!tree || !tree->root)
        return 0;
    return tree->root->height;
}

/**
 * @brief 检查树是否为有效的AVL树
 * @param tree AVL树指针
 * @return 有效返回1，无效返回0
 */
int avl_validate(avl_tree_t *tree)
{
    if (!tree)
        return 0;

    /* 检查每个节点的平衡因子和 BST 排序性质 */
    avl_node_t *stack[128];
    int top = -1;
    avl_node_t *current = tree->root;
    avl_node_t *prev = NULL;

    while (current != NULL || top >= 0)
    {
        while (current != NULL)
        {
            int bf = avl_balance_factor(current);
            if (bf < -1 || bf > 1)
                return 0; /* 不平衡 */

            /* 检查高度是否正确 */
            int expected_height = 1 + max_int(avl_node_height(current->left),
                                              avl_node_height(current->right));
            if (current->height != expected_height)
                return 0; /* 高度错误 */

            stack[++top] = current;
            current = current->left;
        }

        current = stack[top--];

        /* 检查 BST 性质：prev->key < current->key */
        if (prev && tree->compare(prev->key, current->key) >= 0)
            return 0; /* BST 排序错误 */

        prev = current;
        current = current->right;
    }

    return 1;
}