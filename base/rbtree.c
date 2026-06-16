#include "rbtree.h"
#include <stdio.h>
#include <string.h>

/* ==================== 内部辅助函数 ==================== */

/**
 * @brief 获取两个数中的最大值
 */
static inline int max_int(int a, int b)
{
    return (a > b) ? a : b;
}

/**
 * @brief 计算节点高度
 */
static int rb_node_height(rb_node_s *node, rb_node_s *nil)
{
    if (!node || node == nil)
        return 0;
    return 1 + max_int(rb_node_height(node->left, nil),
                       rb_node_height(node->right, nil));
}

/**
 * @brief 左旋转
 */
void rb_left_rotate(rb_tree_s *tree, rb_node_s *x)
{
    if (!tree || !x || x->right == tree->nil)
        return;

    rb_node_s *y = x->right;
    x->right = y->left;

    if (y->left != tree->nil)
        y->left->parent = x;

    y->parent = x->parent;

    if (x->parent == tree->nil)
        tree->root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;

    y->left = x;
    x->parent = y;
}

/**
 * @brief 右旋转
 */
void rb_right_rotate(rb_tree_s *tree, rb_node_s *y)
{
    if (!tree || !y || y->left == tree->nil)
        return;

    rb_node_s *x = y->left;
    y->left = x->right;

    if (x->right != tree->nil)
        x->right->parent = y;

    x->parent = y->parent;

    if (y->parent == tree->nil)
        tree->root = x;
    else if (y == y->parent->right)
        y->parent->right = x;
    else
        y->parent->left = x;

    x->right = y;
    y->parent = x;
}

/**
 * @brief 插入后修复红黑树性质
 */
void rb_insert_fixup(rb_tree_s *tree, rb_node_s *z)
{
    while (z->parent->color == RB_RED)
    {
        if (z->parent == z->parent->parent->left)
        {
            rb_node_s *y = z->parent->parent->right; /* 叔叔节点 */

            if (y->color == RB_RED)
            {
                /* 情况1：叔叔是红色 */
                z->parent->color = RB_BLACK;
                y->color = RB_BLACK;
                z->parent->parent->color = RB_RED;
                z = z->parent->parent;
            }
            else
            {
                if (z == z->parent->right)
                {
                    /* 情况2：叔叔是黑色，z是右子节点 */
                    z = z->parent;
                    rb_left_rotate(tree, z);
                }

                /* 情况3：叔叔是黑色，z是左子节点 */
                z->parent->color = RB_BLACK;
                z->parent->parent->color = RB_RED;
                rb_right_rotate(tree, z->parent->parent);
            }
        }
        else
        {
            /* 对称情况 */
            rb_node_s *y = z->parent->parent->left; /* 叔叔节点 */

            if (y->color == RB_RED)
            {
                /* 情况1：叔叔是红色 */
                z->parent->color = RB_BLACK;
                y->color = RB_BLACK;
                z->parent->parent->color = RB_RED;
                z = z->parent->parent;
            }
            else
            {
                if (z == z->parent->left)
                {
                    /* 情况2：叔叔是黑色，z是左子节点 */
                    z = z->parent;
                    rb_right_rotate(tree, z);
                }

                /* 情况3：叔叔是黑色，z是右子节点 */
                z->parent->color = RB_BLACK;
                z->parent->parent->color = RB_RED;
                rb_left_rotate(tree, z->parent->parent);
            }
        }
    }

    tree->root->color = RB_BLACK;
}

/**
 * @brief 用v替换u
 */
rb_node_s *rb_transplant(rb_tree_s *tree, rb_node_s *u, rb_node_s *v)
{
    rb_node_s *u_parent = u->parent;

    if (u->parent == tree->nil)
        tree->root = v;
    else if (u == u->parent->left)
        u->parent->left = v;
    else
        u->parent->right = v;

    v->parent = u->parent;

    return u_parent;
}

/**
 * @brief 删除后修复红黑树性质
 */
void rb_delete_fixup(rb_tree_s *tree, rb_node_s *x)
{
    while (x != tree->root && x->color == RB_BLACK)
    {
        if (x == x->parent->left)
        {
            rb_node_s *w = x->parent->right; /* 兄弟节点 */

            if (w->color == RB_RED)
            {
                /* 情况1：兄弟是红色 */
                w->color = RB_BLACK;
                x->parent->color = RB_RED;
                rb_left_rotate(tree, x->parent);
                w = x->parent->right;
            }

            if (w->left->color == RB_BLACK && w->right->color == RB_BLACK)
            {
                /* 情况2：兄弟的两个子节点都是黑色 */
                w->color = RB_RED;
                x = x->parent;
            }
            else
            {
                if (w->right->color == RB_BLACK)
                {
                    /* 情况3：兄弟的右子节点是黑色，左子节点是红色 */
                    w->left->color = RB_BLACK;
                    w->color = RB_RED;
                    rb_right_rotate(tree, w);
                    w = x->parent->right;
                }

                /* 情况4：兄弟的右子节点是红色 */
                w->color = x->parent->color;
                x->parent->color = RB_BLACK;
                w->right->color = RB_BLACK;
                rb_left_rotate(tree, x->parent);
                x = tree->root;
            }
        }
        else
        {
            /* 对称情况 */
            rb_node_s *w = x->parent->left; /* 兄弟节点 */

            if (w->color == RB_RED)
            {
                /* 情况1：兄弟是红色 */
                w->color = RB_BLACK;
                x->parent->color = RB_RED;
                rb_right_rotate(tree, x->parent);
                w = x->parent->left;
            }

            if (w->right->color == RB_BLACK && w->left->color == RB_BLACK)
            {
                /* 情况2：兄弟的两个子节点都是黑色 */
                w->color = RB_RED;
                x = x->parent;
            }
            else
            {
                if (w->left->color == RB_BLACK)
                {
                    /* 情况3：兄弟的左子节点是黑色，右子节点是红色 */
                    w->right->color = RB_BLACK;
                    w->color = RB_RED;
                    rb_left_rotate(tree, w);
                    w = x->parent->left;
                }

                /* 情况4：兄弟的左子节点是红色 */
                w->color = x->parent->color;
                x->parent->color = RB_BLACK;
                w->left->color = RB_BLACK;
                rb_right_rotate(tree, x->parent);
                x = tree->root;
            }
        }
    }

    x->color = RB_BLACK;
}

/**
 * @brief 查找最小节点
 */
rb_node_s *rb_minimum_node(rb_node_s *node, rb_node_s *nil)
{
    while (node->left != nil)
        node = node->left;
    return node;
}

/**
 * @brief 查找最大节点
 */
rb_node_s *rb_maximum_node(rb_node_s *node, rb_node_s *nil)
{
    while (node->right != nil)
        node = node->right;
    return node;
}

/**
 * @brief 验证辅助函数
 */
int rb_validate_helper(rb_node_s *node, rb_node_s *nil, int *black_height)
{
    if (node == nil)
    {
        if (*black_height == 0)
            *black_height = 1;
        return 1;
    }

    /* 红色节点的子节点必须是黑色 */
    if (node->color == RB_RED)
    {
        if (node->left->color == RB_RED || node->right->color == RB_RED)
            return 0;
    }

    /* 检查左子树 */
    int left_bh = 0;
    if (!rb_validate_helper(node->left, nil, &left_bh))
        return 0;

    /* 检查右子树 */
    int right_bh = 0;
    if (!rb_validate_helper(node->right, nil, &right_bh))
        return 0;

    /* 黑色高度必须相同 */
    if (left_bh != right_bh)
        return 0;

    /* 累加当前节点的黑色高度 */
    *black_height = left_bh + (node->color == RB_BLACK ? 1 : 0);

    return 1;
}

/**
 * @brief 递归释放节点
 */
static void rb_free_nodes(rb_node_s *node, rb_node_s *nil,
                          void (*key_free)(void *), void (*data_free)(void *))
{
    if (node == nil)
        return;

    rb_free_nodes(node->left, nil, key_free, data_free);
    rb_free_nodes(node->right, nil, key_free, data_free);

    if (key_free && node->key)
        key_free(node->key);
    if (data_free && node->data)
        data_free(node->data);

    free(node);
}

/* ==================== 公共接口函数 ==================== */

/**
 * @brief 创建红黑树
 */
rb_tree_s *rb_create(int (*compare)(const void *a, const void *b),
                     void (*key_free)(void *key),
                     void (*data_free)(void *data))
{
    if (!compare)
        return NULL;

    rb_tree_s *tree = (rb_tree_s *)malloc(sizeof(rb_tree_s));
    if (!tree)
        return NULL;

    /* 创建哨兵节点 */
    rb_node_s *nil = (rb_node_s *)malloc(sizeof(rb_node_s));
    if (!nil)
    {
        free(tree);
        return NULL;
    }

    nil->color = RB_BLACK;
    nil->left = NULL;
    nil->right = NULL;
    nil->parent = NULL;
    nil->key = NULL;
    nil->data = NULL;

    tree->nil = nil;
    tree->root = nil;
    tree->size = 0;
    tree->compare = compare;
    tree->key_free = key_free;
    tree->data_free = data_free;

    return tree;
}

/**
 * @brief 销毁红黑树
 */
void rb_destroy(rb_tree_s *tree)
{
    if (!tree)
        return;

    rb_free_nodes(tree->root, tree->nil, tree->key_free, tree->data_free);
    free(tree->nil);
    free(tree);
}

/**
 * @brief 清空树
 */
void rb_clear(rb_tree_s *tree)
{
    if (!tree)
        return;

    rb_free_nodes(tree->root, tree->nil, tree->key_free, tree->data_free);
    tree->root = tree->nil;
    tree->size = 0;
}

/**
 * @brief 插入键值对
 */
int rb_insert(rb_tree_s *tree, void *key, void *data)
{
    if (!tree || !key)
        return -1;

    rb_node_s *z = (rb_node_s *)malloc(sizeof(rb_node_s));
    if (!z)
        return -1;

    z->key = key;
    z->data = data;
    z->color = RB_RED;
    z->left = tree->nil;
    z->right = tree->nil;

    /* 查找插入位置 */
    rb_node_s *y = tree->nil;
    rb_node_s *x = tree->root;

    while (x != tree->nil)
    {
        y = x;
        int cmp = tree->compare(key, x->key);

        if (cmp < 0)
            x = x->left;
        else if (cmp > 0)
            x = x->right;
        else
        {
            /* 键已存在 */
            free(z);
            return 1;
        }
    }

    z->parent = y;

    if (y == tree->nil)
        tree->root = z;
    else if (tree->compare(key, y->key) < 0)
        y->left = z;
    else
        y->right = z;

    tree->size++;

    /* 修复红黑树性质 */
    rb_insert_fixup(tree, z);

    return 0;
}

/**
 * @brief 更新键值对
 */
int rb_update(rb_tree_s *tree, void *key, void *data, void **old_data)
{
    if (!tree || !key)
        return -1;

    if (old_data)
        *old_data = NULL;

    /* 查找键 */
    rb_node_s *node = tree->root;
    while (node != tree->nil)
    {
        int cmp = tree->compare(key, node->key);

        if (cmp < 0)
            node = node->left;
        else if (cmp > 0)
            node = node->right;
        else
        {
            /* 键存在，更新数据 */
            if (old_data)
                *old_data = node->data;
            node->data = data;
            return 0;
        }
    }

    /* 键不存在，插入新节点 */
    return rb_insert(tree, key, data);
}

/**
 * @brief 删除指定键的节点
 */
int rb_delete(rb_tree_s *tree, void *key)
{
    if (!tree || !key)
        return -1;

    /* 查找节点 */
    rb_node_s *z = tree->root;
    while (z != tree->nil)
    {
        int cmp = tree->compare(key, z->key);

        if (cmp < 0)
            z = z->left;
        else if (cmp > 0)
            z = z->right;
        else
            break;
    }

    if (z == tree->nil)
        return 1; /* 键不存在 */

    rb_node_s *y = z;
    rb_color_t y_original_color = y->color;
    rb_node_s *x;

    if (z->left == tree->nil)
    {
        x = z->right;
        rb_transplant(tree, z, z->right);
    }
    else if (z->right == tree->nil)
    {
        x = z->left;
        rb_transplant(tree, z, z->left);
    }
    else
    {
        y = rb_minimum_node(z->right, tree->nil);
        y_original_color = y->color;
        x = y->right;

        if (y->parent == z)
            x->parent = y;
        else
        {
            rb_transplant(tree, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }

        rb_transplant(tree, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }

    /* 释放键和数据 */
    if (tree->key_free && z->key)
        tree->key_free(z->key);
    if (tree->data_free && z->data)
        tree->data_free(z->data);

    free(z);
    tree->size--;

    /* 如果删除的是黑色节点，需要修复 */
    if (y_original_color == RB_BLACK)
        rb_delete_fixup(tree, x);

    return 0;
}

/**
 * @brief 查找指定键的节点数据
 */
void *rb_search(rb_tree_s *tree, void *key)
{
    if (!tree || !key)
        return NULL;

    rb_node_s *current = tree->root;
    while (current != tree->nil)
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
 */
int rb_contains(rb_tree_s *tree, void *key)
{
    return rb_search(tree, key) != NULL;
}

/**
 * @brief 获取最小键节点数据
 */
void *rb_minimum(rb_tree_s *tree)
{
    if (!tree || tree->root == tree->nil)
        return NULL;

    rb_node_s *node = rb_minimum_node(tree->root, tree->nil);
    return node != tree->nil ? node->data : NULL;
}

/**
 * @brief 获取最大键节点数据
 */
void *rb_maximum(rb_tree_s *tree)
{
    if (!tree || tree->root == tree->nil)
        return NULL;

    rb_node_s *node = rb_maximum_node(tree->root, tree->nil);
    return node != tree->nil ? node->data : NULL;
}

/**
 * @brief 获取前驱节点数据
 */
void *rb_predecessor(rb_tree_s *tree, void *key)
{
    if (!tree || !key || tree->root == tree->nil)
        return NULL;

    /* 查找节点 */
    rb_node_s *node = tree->root;
    rb_node_s *predecessor = tree->nil;

    while (node != tree->nil)
    {
        int cmp = tree->compare(key, node->key);

        if (cmp < 0)
            node = node->left;
        else if (cmp > 0)
        {
            predecessor = node;
            node = node->right;
        }
        else
        {
            /* 找到节点，找前驱 */
            if (node->left != tree->nil)
                predecessor = rb_maximum_node(node->left, tree->nil);
            break;
        }
    }

    return predecessor != tree->nil ? predecessor->data : NULL;
}

/**
 * @brief 获取后继节点数据
 */
void *rb_successor(rb_tree_s *tree, void *key)
{
    if (!tree || !key || tree->root == tree->nil)
        return NULL;

    /* 查找节点 */
    rb_node_s *node = tree->root;
    rb_node_s *successor = tree->nil;

    while (node != tree->nil)
    {
        int cmp = tree->compare(key, node->key);

        if (cmp < 0)
        {
            successor = node;
            node = node->left;
        }
        else if (cmp > 0)
            node = node->right;
        else
        {
            /* 找到节点，找后继 */
            if (node->right != tree->nil)
                successor = rb_minimum_node(node->right, tree->nil);
            break;
        }
    }

    return successor != tree->nil ? successor->data : NULL;
}

/**
 * @brief 中序遍历
 */
void rb_inorder(rb_tree_s *tree, void (*func)(void *key, void *data, void *userdata),
                void *userdata)
{
    if (!tree || !func || tree->root == tree->nil)
        return;

    /* 使用栈进行迭代中序遍历 */
    rb_node_s *stack[128];
    int top = -1;
    rb_node_s *current = tree->root;

    while (current != tree->nil || top >= 0)
    {
        while (current != tree->nil)
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
 */
void rb_preorder(rb_tree_s *tree, void (*func)(void *key, void *data, void *userdata),
                 void *userdata)
{
    if (!tree || !func || tree->root == tree->nil)
        return;

    /* 使用栈进行迭代前序遍历 */
    rb_node_s *stack[128];
    int top = -1;
    rb_node_s *current = tree->root;

    stack[++top] = current;

    while (top >= 0)
    {
        current = stack[top--];
        func(current->key, current->data, userdata);

        if (current->right != tree->nil)
            stack[++top] = current->right;
        if (current->left != tree->nil)
            stack[++top] = current->left;
    }
}

/**
 * @brief 后序遍历
 */
void rb_postorder(rb_tree_s *tree, void (*func)(void *key, void *data, void *userdata),
                  void *userdata)
{
    if (!tree || !func || tree->root == tree->nil)
        return;

    /* 使用栈进行迭代后序遍历 */
    rb_node_s *stack1[128];
    rb_node_s *stack2[128];
    int top1 = -1, top2 = -1;
    rb_node_s *current = tree->root;

    stack1[++top1] = current;

    while (top1 >= 0)
    {
        current = stack1[top1--];
        stack2[++top2] = current;

        if (current->left != tree->nil)
            stack1[++top1] = current->left;
        if (current->right != tree->nil)
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
 */
void rb_levelorder(rb_tree_s *tree, void (*func)(void *key, void *data, void *userdata),
                   void *userdata)
{
    if (!tree || !func || tree->root == tree->nil)
        return;

    /* 使用队列进行层序遍历 */
    rb_node_s *queue[128];
    int front = 0, rear = 0;
    queue[rear++] = tree->root;

    while (front < rear)
    {
        rb_node_s *current = queue[front++];
        func(current->key, current->data, userdata);

        if (current->left != tree->nil)
            queue[rear++] = current->left;
        if (current->right != tree->nil)
            queue[rear++] = current->right;
    }
}

/**
 * @brief 反向中序遍历（降序）
 */
void rb_reverse_inorder(rb_tree_s *tree, void (*func)(void *key, void *data, void *userdata),
                        void *userdata)
{
    if (!tree || !func || tree->root == tree->nil)
        return;

    /* 使用栈进行迭代反向中序遍历 */
    rb_node_s *stack[128];
    int top = -1;
    rb_node_s *current = tree->root;

    while (current != tree->nil || top >= 0)
    {
        while (current != tree->nil)
        {
            stack[++top] = current;
            current = current->right;
        }

        current = stack[top--];
        func(current->key, current->data, userdata);
        current = current->left;
    }
}

/**
 * @brief 范围查询
 */
void rb_range_query(rb_tree_s *tree, void *min_key, void *max_key,
                    void (*func)(void *key, void *data, void *userdata),
                    void *userdata)
{
    if (!tree || !func || tree->root == tree->nil)
        return;

    /* 使用栈进行迭代中序遍历，同时检查范围 */
    rb_node_s *stack[128];
    int top = -1;
    rb_node_s *current = tree->root;

    while (current != tree->nil || top >= 0)
    {
        while (current != tree->nil)
        {
            stack[++top] = current;
            current = current->left;
        }

        current = stack[top--];

        /* 检查是否在范围内 */
        if (tree->compare(current->key, min_key) >= 0 &&
            tree->compare(current->key, max_key) <= 0)
        {
            func(current->key, current->data, userdata);
        }
        else if (tree->compare(current->key, max_key) > 0)
        {
            /* 超出范围，停止遍历 */
            break;
        }

        current = current->right;
    }
}

/**
 * @brief 统计指定范围内的键值对数量
 */
static void rb_count_helper(void *key, void *data, void *userdata)
{
    (void)key;
    (void)data;
    (*(size_t *)userdata)++;
}

size_t rb_range_count(rb_tree_s *tree, void *min_key, void *max_key)
{
    size_t count = 0;
    rb_range_query(tree, min_key, max_key, rb_count_helper, &count);
    return count;
}

/**
 * @brief 获取树的大小
 */
size_t rb_size(rb_tree_s *tree)
{
    if (!tree)
        return 0;
    return tree->size;
}

/**
 * @brief 检查树是否为空
 */
int rb_empty(rb_tree_s *tree)
{
    if (!tree)
        return 1;
    return tree->size == 0;
}

/**
 * @brief 获取树的高度
 */
int rb_height(rb_tree_s *tree)
{
    if (!tree || tree->root == tree->nil)
        return 0;
    return rb_node_height(tree->root, tree->nil);
}

/**
 * @brief 检查树是否为有效的红黑树
 */
int rb_validate(rb_tree_s *tree)
{
    if (!tree || !tree->nil)
        return 0;

    /* 检查根节点是否为黑色 */
    if (tree->root->color != RB_BLACK)
        return 0;

    /* 检查红色节点的子节点和黑色高度 */
    int black_height = 0;
    return rb_validate_helper(tree->root, tree->nil, &black_height);
}

/**
 * @brief 获取树的黑色高度
 */
int rb_black_height(rb_tree_s *tree)
{
    if (!tree || tree->root == tree->nil)
        return 0;

    int black_height = 0;
    rb_node_s *current = tree->root;

    while (current != tree->nil)
    {
        if (current->color == RB_BLACK)
            black_height++;
        current = current->left;
    }

    return black_height;
}