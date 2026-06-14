#ifndef HASH_H
#define HASH_H

#include <stdint.h>
#include <stdlib.h>

/**
 * @file hash.h
 * @brief 哈希表数据结构实现
 *
 * 提供高效的键值对存储和检索功能，支持多种冲突解决策略
 */

/**
 * @brief 哈希冲突解决策略
 */
typedef enum
{
    HASH_CHAINING,      // 链地址法
    HASH_LINEAR_PROBE,  // 线性探测
    HASH_QUADRATIC_PROBE // 二次探测
} hash_collision_t;

/**
 * @brief 哈希表节点结构
 */
typedef struct hash_node
{
    void *key;                      // 键指针
    void *data;                     // 数据指针
    uint32_t hash;                  // 哈希值（缓存）
    struct hash_node *next;         // 链地址法的下一个节点
} hash_node_s;

/**
 * @brief 哈希表状态枚举
 */
typedef enum
{
    HASH_OK = 0,        // 成功
    HASH_EXISTS = 1,    // 键已存在
    HASH_NOT_FOUND = 2, // 键不存在
    HASH_ERROR = -1     // 错误
} hash_status_t;

/**
 * @brief 哈希表结构
 */
typedef struct hash_table
{
    hash_node_s **buckets;          // 桶数组
    size_t bucket_count;            // 桶数量
    size_t size;                    // 元素数量
    size_t capacity;                // 容量（针对开放寻址法）
    float load_factor;              // 负载因子阈值
    hash_collision_t collision;     // 冲突解决策略
    uint32_t (*hash_func)(const void *key, size_t key_size); // 哈希函数
    int (*compare)(const void *a, const void *b); // 键比较函数
    size_t (*key_size)(const void *key); // 获取键大小的函数
    void (*key_free)(void *key);   // 键释放函数
    void (*data_free)(void *data); // 数据释放函数
} hash_table_s;

/**
 * @brief 创建哈希表
 * @param initial_bucket_count 初始桶数量（建议为素数）
 * @param load_factor 负载因子阈值（0.0-1.0）
 * @param collision 冲突解决策略
 * @param hash_func 哈希函数
 * @param compare 键比较函数
 * @param key_size 获取键大小的函数（NULL表示固定大小或不需要）
 * @param key_free 键释放函数（NULL表示不释放）
 * @param data_free 数据释放函数（NULL表示不释放）
 * @return 成功返回哈希表指针，失败返回NULL
 */
hash_table_s *hash_create(size_t initial_bucket_count,
                          float load_factor,
                          hash_collision_t collision,
                          uint32_t (*hash_func)(const void *key, size_t key_size),
                          int (*compare)(const void *a, const void *b),
                          size_t (*key_size)(const void *key),
                          void (*key_free)(void *key),
                          void (*data_free)(void *data));

/**
 * @brief 销毁哈希表
 * @param table 哈希表指针
 */
void hash_destroy(hash_table_s *table);

/**
 * @brief 清空哈希表
 * @param table 哈希表指针
 */
void hash_clear(hash_table_s *table);

/**
 * @brief 插入键值对
 * @param table 哈希表指针
 * @param key 键指针
 * @param data 数据指针
 * @return HASH_OK成功，HASH_EXISTS键已存在，HASH_ERROR错误
 */
hash_status_t hash_insert(hash_table_s *table, void *key, void *data);

/**
 * @brief 更新键值对（如果键不存在则插入）
 * @param table 哈希表指针
 * @param key 键指针
 * @param data 数据指针
 * @param old_data 输出：旧的数据指针（如果键已存在）
 * @return HASH_OK成功，HASH_ERROR错误
 */
hash_status_t hash_update(hash_table_s *table, void *key, void *data, void **old_data);

/**
 * @brief 替换键值对（强制覆盖）
 * @param table 哈希表指针
 * @param key 键指针
 * @param data 数据指针
 * @param old_data 输出：旧的数据指针（如果键已存在）
 * @return HASH_OK成功，HASH_ERROR错误
 */
hash_status_t hash_replace(hash_table_s *table, void *key, void *data, void **old_data);

/**
 * @brief 删除指定键的节点
 * @param table 哈希表指针
 * @param key 要删除的键
 * @param old_data 输出：被删除的数据指针（可选）
 * @return HASH_OK成功，HASH_NOT_FOUND键不存在，HASH_ERROR错误
 */
hash_status_t hash_delete(hash_table_s *table, void *key, void **old_data);

/**
 * @brief 查找指定键的节点数据
 * @param table 哈希表指针
 * @param key 要查找的键
 * @return 找到返回数据指针，未找到返回NULL
 */
void *hash_search(hash_table_s *table, void *key);

/**
 * @brief 检查键是否存在
 * @param table 哈希表指针
 * @param key 要检查的键
 * @return 存在返回1，不存在返回0
 */
int hash_contains(hash_table_s *table, void *key);

/**
 * @brief 获取表的大小
 * @param table 哈希表指针
 * @return 表的大小
 */
size_t hash_size(hash_table_s *table);

/**
 * @brief 检查表是否为空
 * @param table 哈希表指针
 * @return 空返回1，非空返回0
 */
int hash_empty(hash_table_s *table);

/**
 * @brief 获取桶数量
 * @param table 哈希表指针
 * @return 桶数量
 */
size_t hash_bucket_count(hash_table_s *table);

/**
 * @brief 获取负载因子
 * @param table 哈希表指针
 * @return 负载因子
 */
float hash_load_factor(hash_table_s *table);

/**
 * @brief 调整哈希表大小（扩容/缩容）
 * @param table 哈希表指针
 * @param new_bucket_count 新的桶数量
 * @return 成功返回0，失败返回-1
 */
int hash_resize(hash_table_s *table, size_t new_bucket_count);

/**
 * @brief 自动扩容（当负载因子超过阈值时）
 * @param table 哈希表指针
 * @return 成功返回0，失败返回-1
 */
int hash_auto_resize(hash_table_s *table);

/**
 * @brief 遍历哈希表
 * @param table 哈希表指针
 * @param func 遍历函数
 * @param userdata 传递给遍历函数的用户数据
 */
void hash_foreach(hash_table_s *table, void (*func)(void *key, void *data, void *userdata),
                  void *userdata);

/**
 * @brief 获取所有键
 * @param table 哈希表指针
 * @param keys 输出数组指针（需要调用者释放）
 * @param count 输出：键的数量
 * @return 成功返回0，失败返回-1
 */
int hash_get_keys(hash_table_s *table, void ***keys, size_t *count);

/**
 * @brief 获取所有值
 * @param table 哈希表指针
 * @param values 输出数组指针（需要调用者释放）
 * @param count 输出：值的数量
 * @return 成功返回0，失败返回-1
 */
int hash_get_values(hash_table_s *table, void ***values, size_t *count);

/**
 * @brief 统计哈希冲突次数
 * @param table 哈希表指针
 * @return 冲突次数
 */
size_t hash_collision_count(hash_table_s *table);

/**
 * @brief 获取最长链长度（链地址法）
 * @param table 哈希表指针
 * @return 最长链长度
 */
size_t hash_max_chain_length(hash_table_s *table);

/**
 * @brief 获取空桶数量
 * @param table 哈希表指针
 * @return 空桶数量
 */
size_t hash_empty_bucket_count(hash_table_s *table);

/* ==================== 内置哈希函数 ==================== */

/**
 * @brief DJB2 哈希函数（适用于字符串）
 * @param key 键指针
 * @param key_size 键大小
 * @return 哈希值
 */
uint32_t hash_djb2(const void *key, size_t key_size);

/**
 * @brief FNV-1a 哈希函数
 * @param key 键指针
 * @param key_size 键大小
 * @return 哈希值
 */
uint32_t hash_fnv1a(const void *key, size_t key_size);

/**
 * @brief MurmurHash3 哈希函数
 * @param key 键指针
 * @param key_size 键大小
 * @return 哈希值
 */
uint32_t hash_murmur3(const void *key, size_t key_size);

/**
 * @brief SDBM 哈希函数
 * @param key 键指针
 * @param key_size 键大小
 * @return 哈希值
 */
uint32_t hash_sdbm(const void *key, size_t key_size);

/**
 * @brief 整数哈希函数
 * @param key 键指针（整数地址）
 * @param key_size 键大小（应等于sizeof(int)）
 * @return 哈希值
 */
uint32_t hash_int(const void *key, size_t key_size);

/**
 * @brief 通用整数哈希函数（支持64位）
 * @param key 键指针（整数地址）
 * @param key_size 键大小
 * @return 哈希值
 */
uint32_t hash_general_int(const void *key, size_t key_size);

/**
 * @brief 混合哈希函数（Combine multiple hash values）
 * @param key 键指针
 * @param key_size 键大小
 * @return 哈希值
 */
uint32_t hash_combine(const void *key, size_t key_size);

/* ==================== 内置比较函数 ==================== */

/**
 * @brief 字符串比较函数
 */
int hash_compare_string(const void *a, const void *b);

/**
 * @brief 整数比较函数
 */
int hash_compare_int(const void *a, const void *b);

/**
 * @brief 指针比较函数
 */
int hash_compare_pointer(const void *a, const void *b);

/**
 * @brief 内存比较函数
 */
int hash_compare_memory(const void *a, const void *b);

/* ==================== 内置键大小函数 ==================== */

/**
 * @brief 字符串键大小函数
 */
size_t hash_key_size_string(const void *key);

/**
 * @brief 固定大小键函数
 */
size_t hash_key_size_fixed(const void *key);

#endif /* HASH_H */