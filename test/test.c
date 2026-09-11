/*
 * mylib 综合功能测试
 * 编译: gcc -Wall -I.. -I../base -I../Tcp -I../ThreadPool -I../epoll_reactor -I../log \
 *          test.c ../base/list.c ../base/hash.c ../base/avl.c ../base/rbtree.c \
 *          ../Tcp/Socket.c ../ThreadPool/ThreadPool.c \
 *          ../epoll_reactor/epoll_reactor.c ../log/logc.c \
 *          -lpthread -o test_mylib
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "list.h"
#include "hash.h"
#include "avl.h"
#include "rbtree.h"
#include "Socket.h"
#include "ThreadPool.h"
#include "epoll_reactor.h"
#include "logc.h"

/* ==================== 辅助函数 ==================== */

static int pass = 0, fail = 0;

#define TEST(name, expr) do { \
    printf("  [TEST] %-50s ", name); \
    if (expr) { printf("\xe2\x9c\x93 PASS\n"); pass++; } \
    else { printf("\xe2\x9c\x97 FAIL\n"); fail++; } \
} while(0)

static int cmp_int(const void *a, const void *b)
{
    int ia = *(const int *)a;
    int ib = *(const int *)b;
    if (ia < ib) return -1;
    if (ia > ib) return 1;
    return 0;
}

/* tree/hash 遍历回调: func(key, data, userdata) */
typedef struct { int *buf; int idx; } collect_ctx_t;

static void collect_int_key(void *key, void *data, void *userdata)
{
    (void)data;
    collect_ctx_t *ctx = (collect_ctx_t *)userdata;
    ctx->buf[ctx->idx++] = *(int *)key;
}

/* list 遍历回调: func(data, userdata) */
static void collect_int_data(void *data, void *userdata)
{
    collect_ctx_t *ctx = (collect_ctx_t *)userdata;
    ctx->buf[ctx->idx++] = *(int *)data;
}

/* ==================== 双向链表测试 ==================== */

static void test_dlist(void)
{
    printf("\n─── 双向链表 (dlist) ───\n");

    dlist_s *list = dlist_create(free);
    TEST("dlist_create returns non-NULL", list != NULL);
    TEST("dlist_empty true initially", dlist_empty(list));

    int *a = malloc(sizeof(int)); *a = 10;
    int *b = malloc(sizeof(int)); *b = 20;
    int *c = malloc(sizeof(int)); *c = 30;
    TEST("push_back x3", dlist_push_back(list, a) == 0 &&
                         dlist_push_back(list, b) == 0 &&
                         dlist_push_back(list, c) == 0);
    TEST("dlist_size == 3", dlist_size(list) == 3);

    int order[10];
    collect_ctx_t ctx = { .buf = order, .idx = 0 };
    dlist_foreach(list, collect_int_data, &ctx);
    TEST("顺序 10->20->30", order[0] == 10 && order[1] == 20 && order[2] == 30);

    /* push_front */
    int *z = malloc(sizeof(int)); *z = 0;
    TEST("push_front 0", dlist_push_front(list, z) == 0);
    TEST("size == 4", dlist_size(list) == 4);

    /* pop_front */
    int *pop = (int *)dlist_pop_front(list);
    TEST("pop_front == 0", pop && *pop == 0);
    free(pop);

    /* pop_back */
    pop = (int *)dlist_pop_back(list);
    TEST("pop_back == 30", pop && *pop == 30);
    free(pop);

    /* find */
    dlist_node_s *node = dlist_find(list, &(int){20}, cmp_int);
    TEST("find(20) found", node != NULL);
    TEST("find(99) NULL", dlist_find(list, &(int){99}, cmp_int) == NULL);

    dlist_destroy(list);
}

/* ==================== 单向链表测试 ==================== */

static void test_slist(void)
{
    printf("\n─── 单向链表 (slist) ───\n");

    slist_s *list = slist_create(free);
    TEST("slist_create non-NULL", list != NULL);

    int *a = malloc(sizeof(int)); *a = 1;
    int *b = malloc(sizeof(int)); *b = 2;
    int *c = malloc(sizeof(int)); *c = 3;
    slist_push_back(list, a);
    slist_push_back(list, b);
    slist_push_back(list, c);
    TEST("size == 3", slist_size(list) == 3);

    /* insert_at + remove_at */
    int *x = malloc(sizeof(int)); *x = 99;
    TEST("insert_at(1)", slist_insert_at(list, 1, x) == 0);
    int *rm = (int *)slist_remove_at(list, 0);
    TEST("remove_at(0) == 1", rm && *rm == 1);
    free(rm);

    /* reverse */
    slist_reverse(list);
    int order[10];
    collect_ctx_t ctx = { .buf = order, .idx = 0 };
    slist_foreach(list, collect_int_data, &ctx);
    TEST("reverse => 3->2->99", order[0] == 3 && order[1] == 2 && order[2] == 99);

    slist_destroy(list);
}

/* ==================== 哈希表测试 ==================== */

static void test_hash(void)
{
    printf("\n─── 哈希表 (hash) ───\n");

    hash_table_s *ht = hash_create(17, 0.75, HASH_CHAINING,
        hash_djb2, (int (*)(const void *, const void *))strcmp,
        hash_key_size_string, free, free);
    TEST("hash_create non-NULL", ht != NULL);

    char *k1 = strdup("apple");  int *v1 = malloc(sizeof(int)); *v1 = 100;
    char *k2 = strdup("banana"); int *v2 = malloc(sizeof(int)); *v2 = 200;

    TEST("insert apple",  hash_insert(ht, k1, v1) == HASH_OK);
    TEST("insert banana", hash_insert(ht, k2, v2) == HASH_OK);
    TEST("size == 2", hash_size(ht) == 2);
    TEST("insert duplicate", hash_insert(ht, strdup("apple"), NULL) == HASH_EXISTS);

    int *r = (int *)hash_search(ht, "apple");
    TEST("search apple == 100", r && *r == 100);
    TEST("contains apple", hash_contains(ht, "apple"));
    TEST("!contains ghost", !hash_contains(ht, "ghost"));

    void *old_data = NULL;
    TEST("delete banana", hash_delete(ht, "banana", &old_data) == HASH_OK);
    if (old_data) { TEST("deleted val==200", *(int *)old_data == 200); free(old_data); }
    TEST("size == 1", hash_size(ht) == 1);

    hash_destroy(ht);

    /* 线性探测 */
    hash_table_s *ht2 = hash_create(11, 0.6, HASH_LINEAR_PROBE,
        hash_djb2, (int (*)(const void *, const void *))strcmp,
        hash_key_size_string, free, free);
    TEST("linear probe ok", ht2 != NULL);
    hash_destroy(ht2);

    TEST("hash tests done", 1);
}

/* ==================== AVL树测试 ==================== */

static void test_avl(void)
{
    printf("\n─── AVL树 ───\n");

    avl_tree_t *tree = avl_create(cmp_int, free, free);
    TEST("avl_create non-NULL", tree != NULL);

    int vals[] = {50, 30, 70, 20, 40, 60, 80, 10, 25, 35, 45};
    int i;
    for (i = 0; i < 11; i++) {
        int *k = malloc(sizeof(int)); *k = vals[i];
        int *v = malloc(sizeof(int)); *v = vals[i] * 10;
        avl_insert(tree, k, v);
    }
    TEST("size == 11", avl_size(tree) == 11);

    int key = 30;
    int *val = (int *)avl_search(tree, &key);
    TEST("search(30)==300", val && *val == 300);
    key = 99;
    TEST("search(99) NULL", avl_search(tree, &key) == NULL);

    key = 70;
    TEST("contains(70)", avl_contains(tree, &key));
    key = 999;
    TEST("!contains(999)", !avl_contains(tree, &key));

    int *min = (int *)avl_minimum(tree);
    TEST("min data == 100", min && *min == 100);
    int *max = (int *)avl_maximum(tree);
    TEST("max data == 800", max && *max == 800);

    key = 40;
    int *pred = (int *)avl_predecessor(tree, &key);
    TEST("pred(40) data == 350", pred && *pred == 350);
    int *succ = (int *)avl_successor(tree, &key);
    TEST("succ(40) data == 450", succ && *succ == 450);

    TEST("validate", avl_validate(tree));
    TEST("height > 0", avl_height(tree) > 0);

    key = 20;
    TEST("delete(20)", avl_delete(tree, &key) == 0);
    TEST("size == 10 after delete", avl_size(tree) == 10);
    key = 20;
    TEST("search(20) NULL after delete", avl_search(tree, &key) == NULL);
    TEST("validate after delete", avl_validate(tree));

    int collected[16];
    collect_ctx_t ctx = { .buf = collected, .idx = 0 };
    avl_inorder(tree, collect_int_key, &ctx);
    int sorted = 1;
    for (i = 1; i < ctx.idx; i++)
        if (collected[i-1] >= collected[i]) { sorted = 0; break; }
    TEST("inorder sorted", sorted);

    avl_destroy(tree);
}

/* ==================== 红黑树测试 ==================== */

static void test_rbtree(void)
{
    printf("\n─── 红黑树 (rbtree) ───\n");

    rb_tree_s *tree = rb_create(cmp_int, free, free);
    TEST("rb_create non-NULL", tree != NULL);

    int vals[] = {50, 30, 70, 20, 40, 60, 80, 15, 25, 35, 45, 55, 65, 75, 90};
    int i;
    for (i = 0; i < 15; i++) {
        int *k = malloc(sizeof(int)); *k = vals[i];
        int *v = malloc(sizeof(int)); *v = vals[i] * 2;
        rb_insert(tree, k, v);
    }
    TEST("size == 15", rb_size(tree) == 15);

    int key = 40;
    int *val = (int *)rb_search(tree, &key);
    TEST("search(40)==80", val && *val == 80);
    key = 999;
    TEST("search(999) NULL", rb_search(tree, &key) == NULL);

    key = 60;
    TEST("contains(60)", rb_contains(tree, &key));

    int *min = (int *)rb_minimum(tree);
    TEST("min data == 30", min && *min == 30);
    int *max = (int *)rb_maximum(tree);
    TEST("max data == 180", max && *max == 180);

    key = 50;
    int *succ = (int *)rb_successor(tree, &key);
    TEST("succ(50) data == 110", succ && *succ == 110);

    /* 查找不存在的键 */
    key = 61;
    int *r_search = (int *)rb_search(tree, &key);
    TEST("search(61) NULL", r_search == NULL);

    TEST("validate", rb_validate(tree));
    TEST("black height > 0", rb_black_height(tree) > 0);

    key = 30;
    TEST("delete(30)", rb_delete(tree, &key) == 0);
    TEST("size == 14", rb_size(tree) == 14);
    key = 30;
    TEST("search(30) NULL", rb_search(tree, &key) == NULL);
    TEST("validate after delete", rb_validate(tree));

    int collected[20];
    collect_ctx_t ctx = { .buf = collected, .idx = 0 };
    rb_inorder(tree, collect_int_key, &ctx);
    int sorted = 1;
    for (i = 1; i < ctx.idx; i++)
        if (collected[i-1] >= collected[i]) { sorted = 0; break; }
    TEST("inorder sorted", sorted);

    int keys[8] = {0};
    collect_ctx_t rctx = { .buf = keys, .idx = 0 };
    int mk1 = 40, mk2 = 70;
    rb_range_query(tree, &mk1, &mk2, collect_int_key, &rctx);
    TEST("range [40,70] results > 0", rctx.idx > 0);

    rb_destroy(tree);
}

/* ==================== 线程池测试 ==================== */

static volatile int task_counter = 0;

static void *count_task(void *arg)
{
    (void)arg;
    __sync_fetch_and_add(&task_counter, 1);
    return NULL;
}

static void test_threadpool(void)
{
    printf("\n─── 线程池 (ThreadPool) ───\n");

    pthread_pool *pool = NULL;
    int ret = create_thread_pool((void **)&pool, 4, 2, 2);
    TEST("create_thread_pool OK", ret == 0 && pool != NULL);
    if (ret != 0 || pool == NULL)
        return;

    TEST("max_thread_num == 4", get_max_thread_num(pool) == 4);

    task_counter = 0;
    int i;
    for (i = 0; i < 10; i++)
        add_task(pool, count_task, NULL);

    struct timespec ts = { .tv_sec = 2, .tv_nsec = 0 };
    nanosleep(&ts, NULL);

    TEST("10 tasks completed", task_counter == 10);
    TEST("destroy", thread_pool_destroy(pool) == 0);
}

/* ==================== 日志测试 ==================== */

static void test_logger(void)
{
    printf("\n─── 日志模块 (log) ───\n");

    log_create("/tmp/mylib_test.log");
    log_write("Log test: %s = %d\n", "answer", 42);
    log_write("Second entry\n");

    FILE *f = fopen("/tmp/mylib_test.log", "r");
    TEST("log file exists", f != NULL);
    if (f) {
        char buf[256];
        int lines = 0;
        while (fgets(buf, sizeof(buf), f)) lines++;
        TEST("2 lines written", lines == 2);
        fclose(f);
        remove("/tmp/mylib_test.log");
    }
}

/* ==================== main ==================== */

int main(void)
{
    printf("\xe2\x95\x94\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x97\n");
    printf("\xe2\x95\x91      mylib \xe7\xbb\xbc\xe5\x90\x88\xe5\x8a\x9f\xe8\x83\xbd\xe6\xb5\x8b\xe8\xaf\x95                      \xe2\x95\x91\n");
    printf("\xe2\x95\x9a\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x9d\n");

    test_dlist();
    test_slist();
    test_hash();
    test_avl();
    test_rbtree();
    test_threadpool();
    test_logger();

    printf("\n\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\n");
    printf("  \xe6\x80\xbb\xe8\xae\xa1: %d \xe9\x80\x9a\xe8\xbf\x87, %d \xe5\xa4\xb1\xe8\xb4\xa5", pass, fail);
    if (fail == 0) printf("  \xf0\x9f\x8e\x89 \xe5\x85\xa8\xe9\x83\xa8\xe9\x80\x9a\xe8\xbf\x87!\n");
    else           printf("  \xe2\x9d\x8c\n");
    printf("\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\n");

    return fail > 0 ? 1 : 0;
}
