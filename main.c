// SPDX-License-Identifier: AGPL-3.0-or-later

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef __riscv
#include <sys/syscall.h>
#include <unistd.h>

#if __has_include(<asm/hwprobe.h>)
#include <asm/hwprobe.h>
#endif

#ifndef RISCV_HWPROBE_KEY_IMA_EXT_0
struct riscv_hwprobe {
	long long key;
	unsigned long long value;
};
#define RISCV_HWPROBE_KEY_IMA_EXT_0 4
#define RISCV_HWPROBE_EXT_ZBA       (1ULL << 3)
#define RISCV_HWPROBE_EXT_ZBB       (1ULL << 4)
#define RISCV_HWPROBE_EXT_ZBS       (1ULL << 5)
#define RISCV_HWPROBE_EXT_ZBKB      (1ULL << 8)
#define RISCV_HWPROBE_EXT_ZICOND    (1ULL << 35)
#endif

#ifndef __NR_riscv_hwprobe
#define __NR_riscv_hwprobe 258
#endif

static int have_zba, have_zbb, have_zbs, have_zbkb, have_zicond;

static void
probe_extensions(void)
{
	struct riscv_hwprobe pair = {RISCV_HWPROBE_KEY_IMA_EXT_0, 0};

	if (syscall(__NR_riscv_hwprobe, &pair, 1, 0, NULL, 0) != 0) {
		fprintf(stderr, "riscv_hwprobe unavailable, "
				"extension-gated tests will be skipped\n");
		return;
	}

	have_zba = pair.value & RISCV_HWPROBE_EXT_ZBA;
	have_zbb = pair.value & RISCV_HWPROBE_EXT_ZBB;
	have_zbs = pair.value & RISCV_HWPROBE_EXT_ZBS;
	have_zbkb = pair.value & RISCV_HWPROBE_EXT_ZBKB;
	have_zicond = pair.value & RISCV_HWPROBE_EXT_ZICOND;
}
#endif

#define S_TO_NS(x)    ((x) * 1000 * 1000 * 1000)
#define NS_TO_S(x)    (((x) + .0) / 1000 / 1000 / 1000)
#define _DO_PRAGMA(x) _Pragma(#x)
#define DO_PRAGMA(x)  _DO_PRAGMA(x)

unsigned long batches, frequency, warmupBatches;

static void
item_begin(struct timespec *ts1)
{
	clock_gettime(CLOCK_MONOTONIC, ts1);
}

static void
item_end(const char *name, struct timespec *ts1)
{
	struct timespec ts2;

	clock_gettime(CLOCK_MONOTONIC, &ts2);

	unsigned long long ns = S_TO_NS(ts2.tv_sec) + ts2.tv_nsec;
	ns -= S_TO_NS(ts1->tv_sec) + ts1->tv_nsec;

	unsigned long instructions = BATCHSIZE * batches;
	double cycles = frequency * NS_TO_S(ns);
	printf("%s: IPC/CPI = %.2f/%.2f\n", name, instructions / cycles,
	       cycles / instructions);
}

#define dotest(name, func, ...)                                                \
	do {                                                                   \
		for (unsigned long int i = 0; i < warmupBatches; i++)          \
			func(__VA_ARGS__);                                     \
                                                                               \
		struct timespec ts;                                            \
		item_begin(&ts);                                               \
                                                                               \
		for (unsigned long int i = 0; i < batches; i++)                \
			func(__VA_ARGS__);                                     \
                                                                               \
		item_end(name, &ts);                                           \
	} while (0)

#ifdef __riscv
#define TEST_EXT(cond, name, func, ...)                                        \
	do {                                                                   \
		if (cond)                                                      \
			dotest(name, func, ##__VA_ARGS__);                     \
		else                                                           \
			printf("%s: skipped, extension unavailable\n", name);  \
	} while (0)
#endif

void latency_add_d(void);
void latency_add_w(void);
void latency_mul_d(void);
void latency_mul_w(void);
void latency_mulh_d(void);
void latency_mulh_w(void);
void latency_and(void);
void latency_sll_d(void);
void latency_sll_w(void);
void latency_rotr_d(void);
void latency_rotr_w(void);
void latency_clo_d(void);
void latency_clo_w(void);
void latency_cto_d(void);
void latency_cto_w(void);
void latency_bytepick_d_0(void);
void latency_bytepick_d_4(void);
void latency_bytepick_w_0(void);
void latency_bytepick_w_2(void);
void latency_revb_d(void);
void latency_revh_d(void);
void latency_bitrev_8b(void);
void latency_bitrev_d(void);
void latency_bstrins_d_31_0(void);
void latency_bstrins_d_47_16(void);
void latency_bstrins_w_15_0(void);
void latency_bstrins_w_23_8(void);
void latency_bstrpick_d_31_0(void);
void latency_bstrpick_d_47_16(void);
void latency_bstrpick_w_15_0(void);
void latency_bstrpick_w_23_8(void);
void latency_maskeqz(unsigned long a);
void latency_crc_wbw(void);
void latency_crc_whw(void);
void latency_crc_www(void);
void latency_crc_wdw(void);
void latency_div_d_add(long int dividend, long int divisor, long int diff);

void thrput_add_d(void);
void thrput_add_w(void);
void thrput_mul_d(void);
void thrput_mul_w(void);
void thrput_mulh_d(void);
void thrput_mulh_w(void);
void thrput_and(void);
void thrput_sll_d(void);
void thrput_sll_w(void);
void thrput_rotr_d(void);
void thrput_rotr_w(void);
void thrput_clo_d(void);
void thrput_clo_w(void);
void thrput_cto_d(void);
void thrput_cto_w(void);
void thrput_bytepick_d(void);
void thrput_bytepick_w(void);
void thrput_revb_d(void);
void thrput_revh_d(void);
void thrput_bitrev_8b(void);
void thrput_bitrev_d(void);
void thrput_bstrins_d_31_0(void);
void thrput_bstrins_d_47_16(void);
void thrput_bstrins_w_15_0(void);
void thrput_bstrins_w_23_8(void);
void thrput_bstrpick_d_31_0(void);
void thrput_bstrpick_d_47_16(void);
void thrput_bstrpick_w_15_0(void);
void thrput_bstrpick_w_23_8(void);
void thrput_maskeqz(unsigned long a1, unsigned long a2, unsigned long a3,
		    unsigned long a4, unsigned long a5, unsigned long a6,
		    unsigned long a7, unsigned long a8);
void thrput_crc_wbw(void);
void thrput_crc_whw(void);
void thrput_crc_www(void);
void thrput_crc_wdw(void);
void thrput_div_d(long a0, long a1);

void thrput_nop(void);

#ifdef __riscv
void latency_add(void);
void latency_addw(void);
void latency_and(void);
void latency_sub(void);
void latency_subw(void);
void latency_sll(void);
void latency_sllw(void);
void latency_srl(void);
void latency_sra(void);
void latency_mul(void);
void latency_mulw(void);
void latency_mulh(void);
void latency_mulhu(void);
void latency_mulhsu(void);
void latency_rem(void);
void latency_remu(void);
void latency_sh1add(void);
void latency_sh2add(void);
void latency_sh3add(void);
void latency_add_uw(void);
void latency_sh1add_uw(void);
void latency_sh2add_uw(void);
void latency_sh3add_uw(void);
void latency_andn(void);
void latency_ror(void);
void latency_rorw(void);
void latency_rol(void);
void latency_rolw(void);
void latency_bset(void);
void latency_bclr(void);
void latency_binv(void);
void latency_bext(void);
void latency_sext_w(void);
void latency_clz(void);
void latency_clzw(void);
void latency_ctz(void);
void latency_ctzw(void);
void latency_cpop(void);
void latency_cpopw(void);
void latency_sext_b(void);
void latency_sext_h(void);
void latency_rev8(void);
void latency_brev8(void);
void latency_slli(void);
void latency_slliw(void);
void latency_slli_uw(void);
void latency_rori(void);
void latency_bseti(void);
void latency_bclri(void);
void latency_binvi(void);
void latency_bexti(void);
void latency_czero_eqz(unsigned long a);
void latency_czero_nez(unsigned long a);
void latency_div_add(long int dividend, long int divisor, long int diff);
void latency_divu_add(unsigned long dividend, unsigned long divisor,
		      unsigned long diff);

void thrput_add(void);
void thrput_addw(void);
void thrput_and(void);
void thrput_sub(void);
void thrput_subw(void);
void thrput_sll(void);
void thrput_sllw(void);
void thrput_srl(void);
void thrput_sra(void);
void thrput_mul(void);
void thrput_mulw(void);
void thrput_mulh(void);
void thrput_mulhu(void);
void thrput_mulhsu(void);
void thrput_rem(void);
void thrput_remu(void);
void thrput_sh1add(void);
void thrput_sh2add(void);
void thrput_sh3add(void);
void thrput_add_uw(void);
void thrput_sh1add_uw(void);
void thrput_sh2add_uw(void);
void thrput_sh3add_uw(void);
void thrput_andn(void);
void thrput_ror(void);
void thrput_rorw(void);
void thrput_rol(void);
void thrput_rolw(void);
void thrput_bset(void);
void thrput_bclr(void);
void thrput_binv(void);
void thrput_bext(void);
void thrput_sext_w(void);
void thrput_clz(void);
void thrput_clzw(void);
void thrput_ctz(void);
void thrput_ctzw(void);
void thrput_cpop(void);
void thrput_cpopw(void);
void thrput_sext_b(void);
void thrput_sext_h(void);
void thrput_rev8(void);
void thrput_brev8(void);
void thrput_slli(void);
void thrput_slliw(void);
void thrput_slli_uw(void);
void thrput_rori(void);
void thrput_bseti(void);
void thrput_bclri(void);
void thrput_binvi(void);
void thrput_bexti(void);
void thrput_czero_eqz(unsigned long a);
void thrput_czero_nez(unsigned long a);
void thrput_div(void);
void thrput_divu(void);
#endif

int
main(int argc, const char *argv[])
{
	if (argc != 4) {
		fprintf(stderr, "Usage: %s <BATCHES> <FREQ> <WARMUP-BATCHES>\n",
			argv[0]);
		return 0;
	}

	batches = atol(argv[1]);
	frequency = atol(argv[2]);
	warmupBatches = atol(argv[3]);

	printf("batches %lu, frequency = %lu Hz, warmup for %lu batches\n",
	       batches, frequency, warmupBatches);

#if defined(__loongarch__) && __loongarch_grlen == 64
	dotest("add.d latency", latency_add_d);
	dotest("add.w latency", latency_add_w);
	dotest("mul.d latency", latency_mul_d);
	dotest("mul.w latency", latency_mul_w);
	dotest("mulh.d latency", latency_mulh_d);
	dotest("mulh.w latency", latency_mulh_w);
	dotest("and latency", latency_and);
	dotest("sll.d latency", latency_sll_d);
	dotest("sll.w latency", latency_sll_w);
	dotest("rotr.d latency", latency_rotr_d);
	dotest("rotr.w latency", latency_rotr_w);
	dotest("clo.d latency", latency_clo_d);
	dotest("clo.w latency", latency_clo_w);
	dotest("cto.d latency", latency_cto_d);
	dotest("cto.w latency", latency_cto_w);
	dotest("bytepick.d 0 latency", latency_bytepick_d_0);
	dotest("bytepick.d 4 latency", latency_bytepick_d_4);
	dotest("bytepick.w 0 latency", latency_bytepick_w_0);
	dotest("bytepick.w 2 latency", latency_bytepick_w_2);
	dotest("revb.d latency", latency_revb_d);
	dotest("revh.d latency", latency_revh_d);
	dotest("bitrev.8b latency", latency_bitrev_8b);
	dotest("bitrev.d latency", latency_bitrev_d);
	dotest("bstrins.d (31, 0) latency", latency_bstrins_d_31_0);
	dotest("bstrins.d (47, 16) latency", latency_bstrins_d_47_16);
	dotest("bstrins.w (15, 0) latency", latency_bstrins_w_15_0);
	dotest("bstrins.w (23, 8) latency", latency_bstrins_w_23_8);
	dotest("bstrpick.d (31, 0) latency", latency_bstrpick_d_31_0);
	dotest("bstrpick.d (47, 16) latency", latency_bstrpick_d_47_16);
	dotest("bstrpick.w (15, 0) latency", latency_bstrpick_w_15_0);
	dotest("bstrpick.w (23, 8) latency", latency_bstrpick_w_23_8);
	dotest("maskeqz masked/untaken latency", latency_maskeqz, 0);
	dotest("maskeqz unmasked/taken latency", latency_maskeqz, 1);
	dotest("crc.w.b.w latency", latency_crc_wbw);
	dotest("crc.w.h.w latency", latency_crc_whw);
	dotest("crc.w.w.w latency", latency_crc_www);
	dotest("crc.w.d.w latency", latency_crc_wdw);

	for (int i = 0; i < 64; i += 8) {
		long dividend = 1ULL << i;

		/*
		 * 16 > 3 * 3 so we could always verify against the most common
		 * cases of early termination, where dividend is less than
		 * divisor.
		 */
		for (long divisor = 1; divisor < dividend * 16; divisor *= 3) {
			char msg[256];
			snprintf(msg, 256, "div.d %ld %ld & add latency",
				 dividend, divisor);

			dotest(msg, latency_div_d_add, dividend, divisor,
			       dividend - dividend / divisor);
		}
	}

	dotest("add.d throughput", thrput_add_d);
	dotest("add.w throughput", thrput_add_w);
	dotest("mul.d throughput", thrput_mul_d);
	dotest("mul.w throughput", thrput_mul_w);
	dotest("mulh.d throughput", thrput_mulh_d);
	dotest("mulh.w throughput", thrput_mulh_w);
	dotest("and throughput", thrput_and);
	dotest("sll.d throughput", thrput_sll_d);
	dotest("sll.w throughput", thrput_sll_w);
	dotest("rotr.d throughput", thrput_rotr_d);
	dotest("rotr.w throughput", thrput_rotr_w);
	dotest("clo.d throughput", thrput_clo_d);
	dotest("clo.w throughput", thrput_clo_w);
	dotest("cto.d throughput", thrput_cto_d);
	dotest("cto.w throughput", thrput_cto_w);
	dotest("bytepick.d throughput", thrput_bytepick_d);
	dotest("bytepick.w throughput", thrput_bytepick_w);
	dotest("revb.d throughput", thrput_revb_d);
	dotest("revh.d throughput", thrput_revh_d);
	dotest("bitrev.8b throughput", thrput_bitrev_8b);
	dotest("bitrev.d throughput", thrput_bitrev_d);
	dotest("bstrins.d (31, 0) throughput", thrput_bstrins_d_31_0);
	dotest("bstrins.d (47, 16) throughput", thrput_bstrins_d_47_16);
	dotest("bstrins.w (15, 0) throughput", thrput_bstrins_w_15_0);
	dotest("bstrins.w (23, 8) throughput", thrput_bstrins_w_23_8);
	dotest("bstrpick.d (31, 0) throughput", thrput_bstrpick_d_31_0);
	dotest("bstrpick.d (47, 16) throughput", thrput_bstrpick_d_47_16);
	dotest("bstrpick.w (15, 0) throughput", thrput_bstrpick_w_15_0);
	dotest("bstrpick.w (23, 8) throughput", thrput_bstrpick_w_23_8);
	dotest("maskeqz masked/untaken throughput", thrput_maskeqz, 0, 0, 0, 0,
	       0, 0, 0, 0);
	dotest("maskeqz unmasked/taken throughput", thrput_maskeqz, 1, 1, 1, 1,
	       1, 1, 1, 1);
	dotest("crc.w.b.w throughput", thrput_crc_wbw);
	dotest("crc.w.h.w throughput", thrput_crc_whw);
	dotest("crc.w.w.w throughput", thrput_crc_www);
	dotest("crc.w.d.w throughput", thrput_crc_wdw);

	for (int i = 0; i < 64; i += 8) {
		long dividend = 1ULL << i;

		for (long divisor = 1; divisor < dividend * 16; divisor *= 3) {
			char msg[256];
			snprintf(msg, 256, "div.d %ld %ld throughput", dividend,
				 divisor);

			dotest(msg, thrput_div_d, dividend, divisor);
		}
	}

	dotest("nop throughput", thrput_nop);
#elif defined(__riscv) && __riscv_xlen == 64
	probe_extensions();

	dotest("add latency", latency_add);
	dotest("addw latency", latency_addw);
	dotest("and latency", latency_and);
	dotest("sub latency", latency_sub);
	dotest("subw latency", latency_subw);
	dotest("sll latency", latency_sll);
	dotest("sllw latency", latency_sllw);
	dotest("srl latency", latency_srl);
	dotest("sra latency", latency_sra);
	dotest("mul latency", latency_mul);
	dotest("mulw latency", latency_mulw);
	dotest("mulh latency", latency_mulh);
	dotest("mulhu latency", latency_mulhu);
	dotest("mulhsu latency", latency_mulhsu);
	dotest("rem latency", latency_rem);
	dotest("remu latency", latency_remu);
	TEST_EXT(have_zba, "sh1add latency", latency_sh1add);
	TEST_EXT(have_zba, "sh2add latency", latency_sh2add);
	TEST_EXT(have_zba, "sh3add latency", latency_sh3add);
	TEST_EXT(have_zba, "add.uw latency", latency_add_uw);
	TEST_EXT(have_zba, "sh1add.uw latency", latency_sh1add_uw);
	TEST_EXT(have_zba, "sh2add.uw latency", latency_sh2add_uw);
	TEST_EXT(have_zba, "sh3add.uw latency", latency_sh3add_uw);
	TEST_EXT(have_zbb, "andn latency", latency_andn);
	TEST_EXT(have_zbb, "ror latency", latency_ror);
	TEST_EXT(have_zbb, "rorw latency", latency_rorw);
	TEST_EXT(have_zbb, "rol latency", latency_rol);
	TEST_EXT(have_zbb, "rolw latency", latency_rolw);
	TEST_EXT(have_zbs, "bset latency", latency_bset);
	TEST_EXT(have_zbs, "bclr latency", latency_bclr);
	TEST_EXT(have_zbs, "binv latency", latency_binv);
	TEST_EXT(have_zbs, "bext latency", latency_bext);
	dotest("sext.w latency", latency_sext_w);
	TEST_EXT(have_zbb, "clz latency", latency_clz);
	TEST_EXT(have_zbb, "clzw latency", latency_clzw);
	TEST_EXT(have_zbb, "ctz latency", latency_ctz);
	TEST_EXT(have_zbb, "ctzw latency", latency_ctzw);
	TEST_EXT(have_zbb, "cpop latency", latency_cpop);
	TEST_EXT(have_zbb, "cpopw latency", latency_cpopw);
	TEST_EXT(have_zbb, "sext.b latency", latency_sext_b);
	TEST_EXT(have_zbb, "sext.h latency", latency_sext_h);
	TEST_EXT(have_zbb, "rev8 latency", latency_rev8);
	TEST_EXT(have_zbkb, "brev8 latency", latency_brev8);
	dotest("slli latency", latency_slli);
	dotest("slliw latency", latency_slliw);
	TEST_EXT(have_zba, "slli.uw latency", latency_slli_uw);
	TEST_EXT(have_zbb, "rori latency", latency_rori);
	TEST_EXT(have_zbs, "bseti latency", latency_bseti);
	TEST_EXT(have_zbs, "bclri latency", latency_bclri);
	TEST_EXT(have_zbs, "binvi latency", latency_binvi);
	TEST_EXT(have_zbs, "bexti latency", latency_bexti);
	TEST_EXT(have_zicond, "czero.eqz taken latency", latency_czero_eqz, 0);
	TEST_EXT(have_zicond, "czero.eqz masked latency", latency_czero_eqz, 1);
	TEST_EXT(have_zicond, "czero.nez masked latency", latency_czero_nez, 0);
	TEST_EXT(have_zicond, "czero.nez taken latency", latency_czero_nez, 1);

	for (int i = 0; i < 64; i += 8) {
		long dividend = 1UL << i;

		for (long divisor = 1; divisor < dividend * 16; divisor *= 3) {
			char msg[256];

			snprintf(msg, 256, "div %ld %ld & add latency",
				 dividend, divisor);
			dotest(msg, latency_div_add, dividend, divisor,
			       dividend - dividend / divisor);
		}
	}

	for (int i = 0; i < 64; i += 8) {
		unsigned long dividend = 1UL << i;

		for (unsigned long divisor = 1; divisor < dividend * 16;
		     divisor *= 3) {
			char msg[256];

			snprintf(msg, 256, "divu %lu %lu & add latency",
				 dividend, divisor);
			dotest(msg, latency_divu_add, dividend, divisor,
			       dividend - dividend / divisor);
		}
	}

	dotest("add throughput", thrput_add);
	dotest("addw throughput", thrput_addw);
	dotest("and throughput", thrput_and);
	dotest("sub throughput", thrput_sub);
	dotest("subw throughput", thrput_subw);
	dotest("sll throughput", thrput_sll);
	dotest("sllw throughput", thrput_sllw);
	dotest("srl throughput", thrput_srl);
	dotest("sra throughput", thrput_sra);
	dotest("mul throughput", thrput_mul);
	dotest("mulw throughput", thrput_mulw);
	dotest("mulh throughput", thrput_mulh);
	dotest("mulhu throughput", thrput_mulhu);
	dotest("mulhsu throughput", thrput_mulhsu);
	dotest("rem throughput", thrput_rem);
	dotest("remu throughput", thrput_remu);
	TEST_EXT(have_zba, "sh1add throughput", thrput_sh1add);
	TEST_EXT(have_zba, "sh2add throughput", thrput_sh2add);
	TEST_EXT(have_zba, "sh3add throughput", thrput_sh3add);
	TEST_EXT(have_zba, "add.uw throughput", thrput_add_uw);
	TEST_EXT(have_zba, "sh1add.uw throughput", thrput_sh1add_uw);
	TEST_EXT(have_zba, "sh2add.uw throughput", thrput_sh2add_uw);
	TEST_EXT(have_zba, "sh3add.uw throughput", thrput_sh3add_uw);
	TEST_EXT(have_zbb, "andn throughput", thrput_andn);
	TEST_EXT(have_zbb, "ror throughput", thrput_ror);
	TEST_EXT(have_zbb, "rorw throughput", thrput_rorw);
	TEST_EXT(have_zbb, "rol throughput", thrput_rol);
	TEST_EXT(have_zbb, "rolw throughput", thrput_rolw);
	TEST_EXT(have_zbs, "bset throughput", thrput_bset);
	TEST_EXT(have_zbs, "bclr throughput", thrput_bclr);
	TEST_EXT(have_zbs, "binv throughput", thrput_binv);
	TEST_EXT(have_zbs, "bext throughput", thrput_bext);
	dotest("sext.w throughput", thrput_sext_w);
	TEST_EXT(have_zbb, "clz throughput", thrput_clz);
	TEST_EXT(have_zbb, "clzw throughput", thrput_clzw);
	TEST_EXT(have_zbb, "ctz throughput", thrput_ctz);
	TEST_EXT(have_zbb, "ctzw throughput", thrput_ctzw);
	TEST_EXT(have_zbb, "cpop throughput", thrput_cpop);
	TEST_EXT(have_zbb, "cpopw throughput", thrput_cpopw);
	TEST_EXT(have_zbb, "sext.b throughput", thrput_sext_b);
	TEST_EXT(have_zbb, "sext.h throughput", thrput_sext_h);
	TEST_EXT(have_zbb, "rev8 throughput", thrput_rev8);
	TEST_EXT(have_zbkb, "brev8 throughput", thrput_brev8);
	dotest("slli throughput", thrput_slli);
	dotest("slliw throughput", thrput_slliw);
	TEST_EXT(have_zba, "slli.uw throughput", thrput_slli_uw);
	TEST_EXT(have_zbb, "rori throughput", thrput_rori);
	TEST_EXT(have_zbs, "bseti throughput", thrput_bseti);
	TEST_EXT(have_zbs, "bclri throughput", thrput_bclri);
	TEST_EXT(have_zbs, "binvi throughput", thrput_binvi);
	TEST_EXT(have_zbs, "bexti throughput", thrput_bexti);
	TEST_EXT(have_zicond, "czero.eqz taken throughput", thrput_czero_eqz,
		 0);
	TEST_EXT(have_zicond, "czero.eqz masked throughput", thrput_czero_eqz,
		 1);
	TEST_EXT(have_zicond, "czero.nez masked throughput", thrput_czero_nez,
		 0);
	TEST_EXT(have_zicond, "czero.nez taken throughput", thrput_czero_nez,
		 1);
	dotest("div throughput", thrput_div);
	dotest("divu throughput", thrput_divu);
	dotest("nop throughput", thrput_nop);
#endif

	return 0;
}
