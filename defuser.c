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

typedef struct {
    int next;
    uint32_t value;
    uint32_t aux;
} P5Node;

static const P5Node p5_nodes[] = {
    {1,  1131u, 52279u},  /* 0: 0x405ba0 */
    {11,   99u, 44260u},  /* 1: 0x405bb0 */
    {4,   608u, 18488u},  /* 2: 0x405bc0 */
    {0,  2924u,  2307u},  /* 3: 0x405bd0 */
    {8,   862u, 59352u},  /* 4: 0x405be0 */
    {2,  2126u,  9682u},  /* 5: 0x405bf0 */
    {7,  1827u,  6773u},  /* 6: 0x405c00 */
    {10, 2597u, 21116u},  /* 7: 0x405c10 */
    {6,  1574u, 48068u},  /* 8: 0x405c20 */
    {-1, 2351u, 20151u},  /* 9: 0x405c30 */
    {9,   331u, 10665u},  /* 10: 0x405c40 */
    {5,  1323u,  9601u}   /* 11: 0x405c50 */
};

static const int p5_start[] = {
    3, 0, 1, 11, 5, 2, 4, 8
};

int solve_phase_5(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;

    uint32_t index = (uint32_t)(challenge & 7u);
    uint32_t hops = (uint32_t)((challenge >> 3) & 3u);

    int node_index = p5_start[index];
    const P5Node *node = &p5_nodes[node_index];

    uint32_t result =
        ((uint32_t)(challenge >> 6)
         ^ (uint32_t)(challenge >> 32)
         ^ node->aux
         ^ 0x306bu);

    result += node->value;
    uint32_t last_value = node->value;

    uint32_t node_count = hops == 0 ? 1u : hops + 1u;

for (uint32_t i = 1; i < node_count; i++) {
    node_index = node->next;
    node = &p5_nodes[node_index];

    result ^= node->aux;
    result += node->value;
    last_value = node->value;
}

    values[0] = result;
    values[1] = last_value;
    count = 2;

    return write_answer(out, cap, values, count);
}


static uint32_t p6_apply(unsigned int op, uint32_t x)
{
    switch (op) {
    case 0:
        return (x ^ 0xe683u) + 4u * x;
    case 1:
        return ((x << 7) + 0xf945u) ^ (7u * x);
    case 2:
        return (x + 0xa164u) ^ (x >> 2);
    case 3:
        return ((x << 5) | (x >> 27)) ^ 0xb8cdu;
    case 4:
        return (x >> 5) | (x << 27);
    case 5:
        return ((x >> 1) ^ x) + 0xa56du;
    default:
        return 0;
    }
}

int solve_phase_6(uint64_t challenge, char *out, size_t cap)
{
    static const unsigned int ops[8] = {
        1, 0, 1, 3, 2, 4, 5, 2
    };

    uint32_t x = (uint32_t)(challenge >> 6)
               ^ (uint32_t)(challenge >> 32);

    unsigned int first_op = ops[challenge & 7u];
    unsigned int second_op = ops[(challenge >> 3) & 7u];

    uint32_t result = p6_apply(first_op, x);
    result ^= 0xa857u;
    result = p6_apply(second_op, result);
    result += 0x8364u;

    uint32_t values[MAX_FIELDS] = {0};
    values[0] = result;

    return write_answer(out, cap, values, 1);
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
