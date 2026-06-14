#include "hash.h"
#include <string.h>
#include <stdio.h>

/* ==================== 素数表（用于扩容） ==================== */

static const size_t PRIMES[] = {
    7, 13, 31, 61, 127, 251, 509, 1021, 2039, 4093,
    8191, 16381, 32749, 65521, 131071, 262139, 524287,
    1048573, 2097143, 4194301, 8388593, 16777213,
    33554393, 67108859, 134217689, 268435399, 536870909,
    1073741789, 2147483647
};

static const size_t PRIME_COUNT = sizeof(PRIMES) / sizeof(PRIMES[0]);

/**
 * @brief 查找下一个素数
 */
static size_t hash_next_prime(size_t n)
{
    for (size_t i = 0; i < PRIME_COUNT; i++)
    {
        if (PRIMES[i] >= n)
            return PRIMES[i];
    }
    return PRIMES[PRIME_COUNT - 1];
}

/* ==================== 内置哈希函数 ==================== */

uint32_t hash_djb2(const void *key, size_t key_size)
{
    if (!key || key_size == 0)
        return 0;

    const uint8_t *str = (const uint8_t *)key;
    uint32_t hash = 5381;

    for (size_t i = 0; i < key_size; i++)
    {
        hash = ((hash << 5) + hash) + str[i]; /* hash * 33 + c */
    }

    return hash;
}

uint32_t hash_fnv1a(const void *key, size_t key_size)
{
    if (!key || key_size == 0)
        return 0;

    const uint8_t *data = (const uint8_t *)key;
    uint32_t hash = 2166136261u;

    for (size_t i = 0; i < key_size; i++)
    {
        hash ^= data[i];
        hash *= 16777619u;
    }

    return hash;
}

uint32_t hash_murmur3(const void *key, size_t key_size)
{
    if (!key || key_size == 0)
        return 0;

    const uint8_t *data = (const uint8_t *)key;
    const uint32_t c1 = 0xcc9e2d51;
    const uint32_t c2 = 0x1b873593;
    const uint32_t r1 = 15;
    const uint32_t r2 = 13;
    const uint32_t m = 5;
    const uint32_t n = 0xe6546b64;

    uint32_t h1 = 0;
    uint32_t k1 = 0;
    const int nblocks = key_size / 4;
    const uint32_t *blocks = (const uint32_t *)data;

    for (int i = 0; i < nblocks; i++)
    {
        k1 = blocks[i];
        k1 *= c1;
        k1 = (k1 << r1) | (k1 >> (32 - r1));
        k1 *= c2;

        h1 ^= k1;
        h1 = (h1 << 13) | (h1 >> (32 - 13));
        h1 = h1 * m + n;
    }

    const uint8_t *tail = (const uint8_t *)(data + nblocks * 4);
    k1 = 0;

    switch (key_size & 3)
    {
        case 3: k1 ^= tail[2] << 16;
        case 2: k1 ^= tail[1] << 8;
        case 1:
            k1 ^= tail[0];
            k1 *= c1;
            k1 = (k1 << r1) | (k1 >> (32 - r1));
            k1 *= c2;
            h1 ^= k1;
    }

    h1 ^= key_size;
    h1 ^= h1 >> 16;
    h1 *= 0x85ebca6b;
    h1 ^= h1 >> 13;
    h1 *= 0xc2b2ae35;
    h1 ^= h1 >> 16;

    return h1;
}

uint32_t hash_sdbm(const void *key, size_t key_size)
{
    if (!key || key_size == 0)
        return 0;

    const uint8_t *str = (const uint8_t *)key;
    uint32_t hash = 0;

    for (size_t i = 0; i < key_size; i++)
    {
        hash = str[i] + (hash << 6) + (hash << 16) - hash;
    }

    return hash;
}

uint32_t hash_int(const void *key, size_t key_size)
{
    if (!key || key_size != sizeof(int))
        return 0;

    int val = *(const int *)key;
    return (uint32_t)val;
}

uint32_t hash_general_int(const void *key, size_t key_size)
{
    if (!key || key_size == 0)
        return 0;

    uint64_t val = 0;
    if (key_size == sizeof(int32_t))
        val = *(const int32_t *)key;
    else if (key_size == sizeof(int64_t))
        val = *(const int64_t *)key;
    else if (key_size >= sizeof(uint64_t))
        val = *(const uint64_t *)key;
    else
        val = *(const uint32_t *)key;

    /* 使用 Knuth 的乘法哈希 */
    val = val * 2654435761u;
    return (uint32_t)(val ^ (val >> 32));
}

uint32_t hash_combine(const void *key, size_t key_size)
{
    if (!key || key_size == 0)
        return 0;

    /* 使用 MurmurHash3 的 Mix 函数 */
    const uint8_t *data = (const uint8_t *)key;
    uint32_t hash = 0;

    for (size_t i = 0; i < key_size; i++)
    {
        hash ^= data[i];
        hash *= 0x5bd1e995;
        hash ^= hash >> 15;
    }

    return hash;
}

/* ==================== 内置比较函数 ==================== */

int hash_compare_string(const void *a, const void *b)
{
    return strcmp((const char *)a, (const char *)b);
}

int hash_compare_int(const void *a, const void *b)
{
    int ia = *(const int *)a;
    int ib = *(const int *)b;
    return (ia > ib) - (ia < ib);
}

int hash_compare_pointer(const void *a, const void *b)
{
    const uint8_t *pa = (const uint8_t *)a;
    const uint8_t *pb = (const uint8_t *)b;
    return (pa > pb) - (pa < pb);
}

int hash_compare_memory(const void *a, const void *b)
{
    /* 这个比较函数假设外部提供 key_size */
    return 0;
}

/* ==================== 内置键大小函数 ==================== */

size_t hash_key_size_string(const void *key)
{
    if (!key)
        return 0;
    return strlen((const char *)key) + 1;
}

size_t hash_key_size_fixed(const void *key)
{
    (void)key;
    return 0; /* 表示固定大小，由调用者处理 */
}

/* ==================== 内部辅助函数 ==================== */

/**
 * @brief 计算键的哈希值并缓存
 */
static uint32_t hash_compute_hash(hash_table_s *table, const void *key)
{
    size_t key_size = table->key_size ? table->key_size(key) : 0;
    return table->hash_func(key, key_size);
}

/**
 * @brief 创建哈希节点
 */
static hash_node_s *hash_create_node(hash_table_s *table, void *key, void *data)
{
    hash_node_s *node = (hash_node_s *)malloc(sizeof(hash_node_s));
    if (!node)
        return NULL;

    node->key = key;
    node->data = data;
    node->hash = hash_compute_hash(table, key);
    node->next = NULL;

    return node;
}

/**
 * @brief 释放哈希节点
 */
static void hash_free_node(hash_table_s *table, hash_node_s *node)
{
    if (!node)
        return;

    if (table->key_free && node->key)
        table->key_free(node->key);
    if (table->data_free && node->data)
        table->data_free(node->data);

    free(node);
}

/**
 * @brief 链地址法：插入节点
 */
static hash_status_t hash_insert_chaining(hash_table_s *table, void *key, void *data, int force)
{
    uint32_t hash = hash_compute_hash(table, key);
    size_t index = hash % table->bucket_count;

    hash_node_s *current = table->buckets[index];
    hash_node_s *prev = NULL;

    /* 检查键是否已存在 */
    while (current)
    {
        if (table->compare(key, current->key) == 0)
        {
            if (force)
            {
                /* 替换数据 */
                if (table->data_free && current->data)
                    table->data_free(current->data);
                current->data = data;
                current->hash = hash;
                return HASH_OK;
            }
            else
                return HASH_EXISTS;
        }
        prev = current;
        current = current->next;
    }

    /* 创建新节点 */
    hash_node_s *node = hash_create_node(table, key, data);
    if (!node)
        return HASH_ERROR;

    /* 插入链表头部 */
    node->next = table->buckets[index];
    table->buckets[index] = node;

    table->size++;
    return HASH_OK;
}

/**
 * @brief 开放寻址法：查找插入位置
 */
static hash_status_t hash_insert_open_addressing(hash_table_s *table, void *key, void *data, int force)
{
    uint32_t hash = hash_compute_hash(table, key);
    size_t index = hash % table->bucket_count;
    size_t start_index = index;
    int found = 0;

    /* 探测插入位置 */
    while (table->buckets[index])
    {
        hash_node_s *node = table->buckets[index];

        if (node && table->compare(key, node->key) == 0)
        {
            if (force)
            {
                /* 替换数据 */
                if (table->data_free && node->data)
                    table->data_free(node->data);
                node->data = data;
                node->hash = hash;
                return HASH_OK;
            }
            else
                return HASH_EXISTS;
        }

        found = 1;
        index++;

        if (index >= table->bucket_count)
            index = 0;

        /* 表已满 */
        if (index == start_index)
            return HASH_ERROR;
    }

    /* 创建新节点 */
    hash_node_s *node = hash_create_node(table, key, data);
    if (!node)
        return HASH_ERROR;

    table->buckets[index] = node;
    table->size++;

    return HASH_OK;
}

/* ==================== 公共接口函数 ==================== */

hash_table_s *hash_create(size_t initial_bucket_count,
                          float load_factor,
                          hash_collision_t collision,
                          uint32_t (*hash_func)(const void *key, size_t key_size),
                          int (*compare)(const void *a, const void *b),
                          size_t (*key_size)(const void *key),
                          void (*key_free)(void *key),
                          void (*data_free)(void *data))
{
    if (!hash_func || !compare)
        return NULL;

    if (initial_bucket_count == 0)
        initial_bucket_count = 16;

    if (load_factor <= 0 || load_factor > 1.0f)
        load_factor = 0.75f;

    initial_bucket_count = hash_next_prime(initial_bucket_count);

    hash_table_s *table = (hash_table_s *)malloc(sizeof(hash_table_s));
    if (!table)
        return NULL;

    table->buckets = (hash_node_s **)calloc(initial_bucket_count, sizeof(hash_node_s *));
    if (!table->buckets)
    {
        free(table);
        return NULL;
    }

    table->bucket_count = initial_bucket_count;
    table->size = 0;
    table->capacity = initial_bucket_count;
    table->load_factor = load_factor;
    table->collision = collision;
    table->hash_func = hash_func;
    table->compare = compare;
    table->key_size = key_size;
    table->key_free = key_free;
    table->data_free = data_free;

    return table;
}

void hash_destroy(hash_table_s *table)
{
    if (!table)
        return;

    hash_clear(table);

    free(table->buckets);
    free(table);
}

void hash_clear(hash_table_s *table)
{
    if (!table)
        return;

    for (size_t i = 0; i < table->bucket_count; i++)
    {
        hash_node_s *current = table->buckets[i];
        while (current)
        {
            hash_node_s *next = current->next;
            hash_free_node(table, current);
            current = next;
        }
        table->buckets[i] = NULL;
    }

    table->size = 0;
}

hash_status_t hash_insert(hash_table_s *table, void *key, void *data)
{
    if (!table || !key)
        return HASH_ERROR;

    hash_status_t result;

    if (table->collision == HASH_CHAINING)
        result = hash_insert_chaining(table, key, data, 0);
    else
        result = hash_insert_open_addressing(table, key, data, 0);

    /* 检查是否需要扩容 */
    if (result == HASH_OK && hash_load_factor(table) > table->load_factor)
        hash_auto_resize(table);

    return result;
}

hash_status_t hash_update(hash_table_s *table, void *key, void *data, void **old_data)
{
    if (old_data)
        *old_data = NULL;

    if (!table || !key)
        return HASH_ERROR;

    void *old = hash_search(table, key);
    if (old)
    {
        if (old_data)
            *old_data = old;
        return hash_replace(table, key, data, NULL);
    }

    return hash_insert(table, key, data);
}

hash_status_t hash_replace(hash_table_s *table, void *key, void *data, void **old_data)
{
    if (old_data)
        *old_data = NULL;

    if (!table || !key)
        return HASH_ERROR;

    void *old = hash_search(table, key);
    if (old_data)
        *old_data = old;

    if (table->collision == HASH_CHAINING)
        return hash_insert_chaining(table, key, data, 1);
    else
        return hash_insert_open_addressing(table, key, data, 1);
}

hash_status_t hash_delete(hash_table_s *table, void *key, void **old_data)
{
    if (old_data)
        *old_data = NULL;

    if (!table || !key)
        return HASH_ERROR;

    uint32_t hash = hash_compute_hash(table, key);
    size_t index = hash % table->bucket_count;

    if (table->collision == HASH_CHAINING)
    {
        hash_node_s *current = table->buckets[index];
        hash_node_s *prev = NULL;

        while (current)
        {
            if (table->compare(key, current->key) == 0)
            {
                if (prev)
                    prev->next = current->next;
                else
                    table->buckets[index] = current->next;

                if (old_data)
                    *old_data = current->data;
                else if (table->data_free && current->data)
                    table->data_free(current->data);

                if (table->key_free && current->key)
                    table->key_free(current->key);

                free(current);
                table->size--;
                return HASH_OK;
            }
            prev = current;
            current = current->next;
        }
    }
    else
    {
        /* 开放寻址法：标记为删除（懒惰删除）或移动后续元素 */
        /* 这里简化处理，直接删除并重新哈希 */
        hash_node_s *node = table->buckets[index];
        size_t start_index = index;
        size_t delete_index = (size_t)-1;

        while (node)
        {
            if (table->compare(key, node->key) == 0)
            {
                delete_index = index;
                break;
            }
            index++;
            if (index >= table->bucket_count)
                index = 0;
            if (index == start_index)
                break;
            node = table->buckets[index];
        }

        if (delete_index != (size_t)-1)
        {
            node = table->buckets[delete_index];
            if (old_data)
                *old_data = node->data;
            else if (table->data_free && node->data)
                table->data_free(node->data);

            if (table->key_free && node->key)
                table->key_free(node->key);

            free(node);
            table->buckets[delete_index] = NULL;
            table->size--;

            /* 重新哈希后续元素 */
            index = delete_index + 1;
            if (index >= table->bucket_count)
                index = 0;

            while (table->buckets[index] && index != start_index)
            {
                node = table->buckets[index];
                void *saved_key = node->key;
                void *saved_data = node->data;

                table->buckets[index] = NULL;
                table->size--;

                hash_insert_open_addressing(table, saved_key, saved_data, 1);

                index++;
                if (index >= table->bucket_count)
                    index = 0;
            }

            return HASH_OK;
        }
    }

    return HASH_NOT_FOUND;
}

void *hash_search(hash_table_s *table, void *key)
{
    if (!table || !key)
        return NULL;

    uint32_t hash = hash_compute_hash(table, key);
    size_t index = hash % table->bucket_count;

    if (table->collision == HASH_CHAINING)
    {
        hash_node_s *current = table->buckets[index];
        while (current)
        {
            if (table->compare(key, current->key) == 0)
                return current->data;
            current = current->next;
        }
    }
    else
    {
        /* 开放寻址法探测 */
        size_t start_index = index;
        while (table->buckets[index])
        {
            hash_node_s *node = table->buckets[index];
            if (table->compare(key, node->key) == 0)
                return node->data;

            index++;
            if (index >= table->bucket_count)
                index = 0;

            if (index == start_index)
                break;
        }
    }

    return NULL;
}

int hash_contains(hash_table_s *table, void *key)
{
    return hash_search(table, key) != NULL;
}

size_t hash_size(hash_table_s *table)
{
    if (!table)
        return 0;
    return table->size;
}

int hash_empty(hash_table_s *table)
{
    if (!table)
        return 1;
    return table->size == 0;
}

size_t hash_bucket_count(hash_table_s *table)
{
    if (!table)
        return 0;
    return table->bucket_count;
}

float hash_load_factor(hash_table_s *table)
{
    if (!table || table->bucket_count == 0)
        return 0.0f;
    return (float)table->size / table->bucket_count;
}

int hash_resize(hash_table_s *table, size_t new_bucket_count)
{
    if (!table || new_bucket_count == 0)
        return -1;

    new_bucket_count = hash_next_prime(new_bucket_count);

    /* 保存旧桶 */
    hash_node_s **old_buckets = table->buckets;
    size_t old_bucket_count = table->bucket_count;

    /* 创建新桶 */
    table->buckets = (hash_node_s **)calloc(new_bucket_count, sizeof(hash_node_s *));
    if (!table->buckets)
    {
        table->buckets = old_buckets;
        return -1;
    }

    table->bucket_count = new_bucket_count;
    table->size = 0;

    /* 重新哈希所有节点 */
    for (size_t i = 0; i < old_bucket_count; i++)
    {
        hash_node_s *current = old_buckets[i];
        while (current)
        {
            hash_node_s *next = current->next;
            current->next = NULL;

            /* 重新计算哈希值 */
            current->hash = hash_compute_hash(table, current->key);
            size_t index = current->hash % new_bucket_count;

            /* 插入新桶 */
            current->next = table->buckets[index];
            table->buckets[index] = current;
            table->size++;

            current = next;
        }
    }

    free(old_buckets);
    return 0;
}

int hash_auto_resize(hash_table_s *table)
{
    if (!table)
        return -1;

    /* 扩容到下一个素数 */
    size_t new_size = hash_next_prime(table->bucket_count * 2);
    return hash_resize(table, new_size);
}

void hash_foreach(hash_table_s *table, void (*func)(void *key, void *data, void *userdata),
                  void *userdata)
{
    if (!table || !func)
        return;

    for (size_t i = 0; i < table->bucket_count; i++)
    {
        hash_node_s *current = table->buckets[i];
        while (current)
        {
            func(current->key, current->data, userdata);
            current = current->next;
        }
    }
}

int hash_get_keys(hash_table_s *table, void ***keys, size_t *count)
{
    if (!table || !keys || !count)
        return -1;

    if (table->size == 0)
    {
        *keys = NULL;
        *count = 0;
        return 0;
    }

    void **result = (void **)malloc(table->size * sizeof(void *));
    if (!result)
        return -1;

    size_t index = 0;
    for (size_t i = 0; i < table->bucket_count; i++)
    {
        hash_node_s *current = table->buckets[i];
        while (current)
        {
            result[index++] = current->key;
            current = current->next;
        }
    }

    *keys = result;
    *count = table->size;
    return 0;
}

int hash_get_values(hash_table_s *table, void ***values, size_t *count)
{
    if (!table || !values || !count)
        return -1;

    if (table->size == 0)
    {
        *values = NULL;
        *count = 0;
        return 0;
    }

    void **result = (void **)malloc(table->size * sizeof(void *));
    if (!result)
        return -1;

    size_t index = 0;
    for (size_t i = 0; i < table->bucket_count; i++)
    {
        hash_node_s *current = table->buckets[i];
        while (current)
        {
            result[index++] = current->data;
            current = current->next;
        }
    }

    *values = result;
    *count = table->size;
    return 0;
}

size_t hash_collision_count(hash_table_s *table)
{
    if (!table)
        return 0;

    size_t collisions = 0;

    for (size_t i = 0; i < table->bucket_count; i++)
    {
        if (table->collision == HASH_CHAINING)
        {
            hash_node_s *current = table->buckets[i];
            while (current && current->next)
            {
                collisions++;
                current = current->next;
            }
        }
        else
        {
            /* 开放寻址法统计比较复杂，这里简化处理 */
        }
    }

    return collisions;
}

size_t hash_max_chain_length(hash_table_s *table)
{
    if (!table)
        return 0;

    if (table->collision != HASH_CHAINING)
        return 0;

    size_t max_len = 0;

    for (size_t i = 0; i < table->bucket_count; i++)
    {
        size_t len = 0;
        hash_node_s *current = table->buckets[i];
        while (current)
        {
            len++;
            current = current->next;
        }
        if (len > max_len)
            max_len = len;
    }

    return max_len;
}

size_t hash_empty_bucket_count(hash_table_s *table)
{
    if (!table)
        return 0;

    size_t empty = 0;

    for (size_t i = 0; i < table->bucket_count; i++)
    {
        if (table->collision == HASH_CHAINING)
        {
            if (!table->buckets[i])
                empty++;
        }
        else
        {
            if (!table->buckets[i])
                empty++;
        }
    }

    return empty;
}