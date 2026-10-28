#include "defuser.h"

#include <inttypes.h>
#include <stdio.h>

#define MAX_FIELDS 8

/*
 * 把 values 中的 count 个整数写成一行“空格分隔的无符号十进制整数”，存入 out。
 * 成功返回 0；count 为 0（还没有实现）、snprintf 出错或结果被截断时返回非零。
 */
static int write_answer(char *out, size_t cap, const uint32_t *values, size_t count) {
    size_t used = 0;
    for (size_t i = 0; i < count; ++i) {
        int size = snprintf(out + used, cap - used, "%s%" PRIu32, i ? " " : "", values[i]);
        if (size < 0 || (size_t)size >= cap - used) return 1;
        used += (size_t)size;
    }
    return count == 0;
}


int solve_phase_1(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;

    uint32_t lo  = (uint32_t)challenge;
    uint32_t mid = (uint32_t)(challenge >> 16);
    uint32_t hi  = (uint32_t)(challenge >> 32);

    uint32_t t2 = ((lo + 0x2178u) ^ hi);
    uint32_t t0 = ((3u * lo + mid) ^ 0x4510u);
    uint32_t result = 4u * t0 + (t2 ^ 0x8ec2u);

    values[0] = result;
    count = 1;

    return write_answer(out, cap, values, count);
}



int solve_phase_2(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 6;

    values[0] = (uint32_t)(((challenge >> 35) & 0x7f) ^ 0x1a);

    for (uint32_t i = 1; i < 6; i++) {
        uint32_t part =
            (uint32_t)((challenge >> (16 + 2 * i)) & 0x1f) + 6;

        uint32_t base = 5 * values[i - 1] - i;
        values[i] = part ^ base;
    }

    return write_answer(out, cap, values, count);
}


int solve_phase_3(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;

    // 第一个整数：challenge 的低 3 位
    uint32_t index = (uint32_t)(challenge & 0x7u);
    values[0] = index;

    // 第二个整数：提取 challenge 的相关位
    uint32_t x = (uint32_t)((challenge >> 32) ^ (challenge >> 3));
    uint32_t result;

    // 根据 index 选择对应的计算分支
    switch (index) {
    case 0:
        result = (x ^ 0xfb44u) + (x >> 1); // p3_case_7
        break;
    case 1:
        result = 5u * x + 0x31b05u;        // p3_case_5
        break;
    case 2:
        result = (2u * x) ^ 0xbf4fu;       // p3_case_0
        break;
    case 3:
        result = (2u * x) ^ 0xd649u;       // p3_case_6
        break;
    case 4:
        result = 5u * x + 0x4d76au;        // p3_case_1
        break;
    case 5:
        result = (x ^ 0x1689u) + (x >> 1); // p3_case_3
        break;
    case 6:
        result = 5u * x + 0x70b0u;         // p3_case_2
        break;
    case 7:
        result = 5u * x + 0x516fu;        // p3_case_4
        break;
    default:
        return 0;
}

    values[1] = result;
    count = 2;

    return write_answer(out, cap, values, count);
}


static uint32_t p4_walk_user(uint32_t target,
                             uint32_t low,
                             uint32_t high,
                             uint32_t salt) {
    uint32_t mid = low + (high - low) / 2u;

    if (target == mid) {
        return (mid + 0x80u) ^ salt;
    }

    uint32_t mixed = mid + salt;

    if (target < mid) {
        uint32_t recursive =
            p4_walk_user(target, low, mid - 1u, salt);
        return (5u * recursive) ^ mixed ^ 0x1bu;
    } else {
        uint32_t recursive =
            p4_walk_user(target, mid + 1u, high, salt);
        return (5u * recursive) ^ mixed ^ 1u;
    }
}
int solve_phase_4(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;

    uint32_t target = (uint32_t)challenge & 0x7fu;
    uint32_t salt = (uint32_t)((challenge >> 7) ^
                               (challenge >> 32));

    values[0] = p4_walk_user(target, 0u, 127u, salt);
    count = 1;

    return write_answer(out, cap, values, count);
}

int solve_phase_5(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    // TODO: 根据 phase_05 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    return write_answer(out, cap, values, count);
}

int solve_phase_6(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    // TODO: 根据 phase_06 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    return write_answer(out, cap, values, count);
}

int solve_secret(uint64_t challenge, uint64_t context, char *out, size_t cap) {
    (void)challenge;
    (void)context;
    (void)out;
    (void)cap;
    // TODO（选做）: 根据 secret_gate、secret_copy 和 secret_phase 的汇编构造完整的第七行，写入 out 后返回 0。
    // 第七行不一定只有整数，可以直接用 snprintf 写入；还没完成时保持返回 1，测试会显示 not attempted。
    return 1;
}
