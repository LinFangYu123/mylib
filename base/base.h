#ifndef BASE_H
#define BASE_H

/**
 * @file base.h
 * @brief 基础数据结构和类型参考
 *
 * 提供常用的比特位操作宏和基础数据结构定义，包括：
 * - 比特位操作宏
 * - 链表数据结构 (list.h)
 * - AVL树数据结构 (avl.h)
 *
 * @note C语言完整类型列表（用于参考）：
 *
 * 基本数据类型：
 * - 整数类型：char, unsigned char, short, unsigned short, int, unsigned int,
 *            long, unsigned long, long long, unsigned long long
 * - 浮点类型：float, double, long double
 * - 字符类型：char, wchar_t, char16_t, char32_t
 * - 布尔类型：_Bool, bool
 * - void类型：void
 *
 * 固定宽度整数类型（stdint.h）：
 * - 无符号：uint8_t, uint16_t, uint32_t, uint64_t
 * - 有符号：int8_t, int16_t, int32_t, int64_t
 * - 其他：uint_least8_t, int_least8_t, uint_fast8_t, int_fast8_t,
 *         uintmax_t, intmax_t, uintptr_t, intptr_t
 *
 * 复合类型：
 * - 数组类型：type[], type[n]
 * - 指针类型：type*, void*, function*
 * - 结构体类型：struct name, struct { ... }
 * - 联合体类型：union name, union { ... }
 * - 枚举类型：enum name, enum { ... }
 * - 函数类型：return_type (*)(parameter_types)
 *
 * 特殊类型：
 * - 类型限定符：const, volatile, restrict, _Atomic
 * - 存储类说明符：auto, register, static, extern, typedef
 * - 其他：ptrdiff_t, size_t, ssize_t, wchar_t, wint_t, sig_atomic_t, max_align_t
 */

/* 常量定义 */
#define BIT_0        (1 << 0)    /* 第0位 */
#define BIT_1        (1 << 1)    /* 第1位 */
#define BIT_2        (1 << 2)    /* 第2位 */
#define BIT_3        (1 << 3)    /* 第3位 */
#define BIT_4        (1 << 4)    /* 第4位 */
#define BIT_5        (1 << 5)    /* 第5位 */
#define BIT_6        (1 << 6)    /* 第6位 */
#define BIT_7        (1 << 7)    /* 第7位 */
#define BIT_8        (1 << 8)    /* 第8位 */
#define BIT_9        (1 << 9)    /* 第9位 */
#define BIT_10       (1 << 10)   /* 第10位 */
#define BIT_11       (1 << 11)   /* 第11位 */
#define BIT_12       (1 << 12)   /* 第12位 */
#define BIT_13       (1 << 13)   /* 第13位 */
#define BIT_14       (1 << 14)   /* 第14位 */
#define BIT_15       (1 << 15)   /* 第15位 */
#define BIT_16       (1 << 16)   /* 第16位 */
#define BIT_17       (1 << 17)   /* 第17位 */
#define BIT_18       (1 << 18)   /* 第18位 */
#define BIT_19       (1 << 19)   /* 第19位 */
#define BIT_20       (1 << 20)   /* 第20位 */
#define BIT_21       (1 << 21)   /* 第21位 */
#define BIT_22       (1 << 22)   /* 第22位 */
#define BIT_23       (1 << 23)   /* 第23位 */
#define BIT_24       (1 << 24)   /* 第24位 */
#define BIT_25       (1 << 25)   /* 第25位 */
#define BIT_26       (1 << 26)   /* 第26位 */
#define BIT_27       (1 << 27)   /* 第27位 */
#define BIT_28       (1 << 28)   /* 第28位 */
#define BIT_29       (1 << 29)   /* 第29位 */
#define BIT_30       (1 << 30)   /* 第30位 */
#define BIT_31       (1 << 31)   /* 第31位 */

/* 常用掩码 */
#define MASK_8BIT    0xFF        /* 8位掩码 */
#define MASK_16BIT   0xFFFF      /* 16位掩码 */
#define MASK_24BIT   0xFFFFFF   /* 24位掩码 */
#define MASK_32BIT   0xFFFFFFFF /* 32位掩码 */

/* 状态定义 */
#define STATUS_OFF   0x0000      /* 关闭状态 */
#define STATUS_ON    0x0001      /* 开启状态 */

/* 比特位操作宏 */

/**
 * @brief 设置指定位为1
 * @param var 要操作的变量
 * @param bit 位位置(从0开始)
 */
#define SET_BIT(var, bit) ((var) |= (1 << (bit)))

/**
 * @brief 清除指定位为0
 * @param var 要操作的变量
 * @param bit 位位置(从0开始)
 */
#define CLEAR_BIT(var, bit) ((var) &= ~(1 << (bit)))

/**
 * @brief 测试指定位是否为1
 * @param var 要测试的变量
 * @param bit 位位置(从0开始)
 * @return 1 如果位为1，0 如果位为0
 */
#define TEST_BIT(var, bit) (((var) >> (bit)) & 1)

/**
 * @brief 翻转指定位(0变1，1变0)
 * @param var 要操作的变量
 * @param bit 位位置(从0开始)
 */
#define FLIP_BIT(var, bit) ((var) ^= (1 << (bit)))

/**
 * @brief 获取指定位的值
 * @param var 要获取的变量
 * @param bit 位位置(从0开始)
 * @return 指定位的值(0或1)
 */
#define GET_BIT(var, bit) TEST_BIT(var, bit)

/**
 * @brief 检查多个位是否被设置
 * @param var 要检查的变量
 * @param mask 位掩码
 * @return 1 如果所有指定位都被设置，0 否则
 */
#define ALL_BITS_SET(var, mask) (((var) & (mask)) == (mask))

/**
 * @brief 检查至少一个位被设置
 * @param var 要检查的变量
 * @param mask 位掩码
 * @return 1 如果至少一个指定位被设置，0 否则
 */
#define ANY_BIT_SET(var, mask) ((var) & (mask))

/**
 * @brief 测试多个特定位是否被设置
 * @param var 要检查的变量
 * @param mask 位掩码，包含要测试的位
 * @return 1 如果所有测试位都被设置，0 否则
 */
#define TEST_BITS(var, mask) (((var) & (mask)) == (mask))

/**
 * @brief 清除多个位
 * @param var 要操作的变量
 * @param mask 位掩码
 */
#define CLEAR_BITS(var, mask) ((var) &= ~(mask))

/**
 * @brief 设置多个位
 * @param var 要操作的变量
 * @param mask 位掩码
 */
#define SET_BITS(var, mask) ((var) |= (mask))

/**
 * @brief 翻转多个位
 * @param var 要操作的变量
 * @param mask 位掩码
 */
#define FLIP_BITS(var, mask) ((var) ^= (mask))

/**
 * @brief 创建位掩码
 * @param bit 位位置(从0开始)
 * @return 对应位的掩码
 */
#define BIT_MASK(bit) (1 << (bit))

/**
 * @brief 创建多个位的掩码
 * @param start 起始位位置
 * @param end 结束位位置(包含)
 * @return 对应范围的掩码
 */
#define BIT_RANGE_MASK(start, end) (((1 << ((end) - (start) + 1)) - 1) << (start))

/**
 * @brief 获取字节的特定位域
 * @param var 要操作的变量
 * @param start 起始位位置
 * @param width 位域宽度
 * @return 提取的位域值
 */
#define GET_BITFIELD(var, start, width) (((var) >> (start)) & ((1 << (width)) - 1))

/**
 * @brief 设置字节的特定位域
 * @param var 要操作的变量
 * @param start 起始位位置
 * @param width 位域宽度
 * @param value 要设置的值
 */
#define SET_BITFIELD(var, start, width, value) \
    do { \
        (var) &= ~(((1 << (width)) - 1) << (start)); \
        (var) |= ((value) & ((1 << (width)) - 1)) << (start); \
    } while(0)

/**
 * @brief 检查变量是否为2的幂
 * @param x 要检查的值
 * @return 1 如果是2的幂，0 否则
 */
#define IS_POWER_OF_TWO(x) ((x) != 0 && ((x) & ((x) - 1)) == 0)

/**
 * @brief 获取最低有效位的位置
 * @param x 要检查的值
 * @return 最低有效位的位置(从0开始)，如果x为0则返回-1
 */
#define LSB_POSITION(x) ((x) == 0 ? -1 : __builtin_ctz(x))

/**
 * @brief 获取最高有效位的位置
 * @param x 要检查的值
 * @return 最高有效位的位置(从0开始)，如果x为0则返回-1
 */
#define MSB_POSITION(x) ((x) == 0 ? -1 : (sizeof(x) * 8 - 1 - __builtin_clz(x)))

/* 基础数据结构包含 */
#include "list.h"
#include "avl.h"

#endif /* BASE_H */