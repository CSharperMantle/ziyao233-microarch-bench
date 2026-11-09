// SPDX-License-Identifier: AGPL-3.0-or-later

#if !defined(__riscv) || __riscv_xlen != 64
#error "riscv64/mulseq.S targets RV64 only"
#endif

/*	Latency: CPI is cycles per multiply	*/

	.macro	seq_lat_2s	name,	shiftop,	shift,	op2

	.global	seq_lat_\name
seq_lat_\name:
	.rept	BATCHSIZE
	\shiftop	t0,		a0,		\shift
	\op2		a0,		t0,		a0
	.endr
	ret
	.endm

	.macro	seq_lat_3s	name,	shiftop,	shift,	op2,	finalshift

	.global	seq_lat_\name
seq_lat_\name:
	.rept	BATCHSIZE
	\shiftop	t0,		a0,		\shift
	\op2		a0,		t0,		a0
	\shiftop	a0,		a0,		\finalshift
	.endr
	ret
	.endm

	.macro	seq_lat_zba_sext	name,	addop

	.global	seq_lat_\name
seq_lat_\name:
	.rept	BATCHSIZE
	\addop		a0,		a0,		a0
	sext.w		a0,		a0
	.endr
	ret
	.endm

	.macro	seq_lat_zba_shift	name,	addop,	shiftop,	imm

	.global	seq_lat_\name
seq_lat_\name:
	.rept	BATCHSIZE
	\addop		a0,		a0,		a0
	\shiftop	a0,		a0,		\imm
	.endr
	ret
	.endm

	.macro	seq_lat_ovf	name,	shiftop,	shift,	op2

	.global	seq_lat_\name
seq_lat_\name:
	.rept	BATCHSIZE
	\shiftop	t0,		a0,		\shift
	\op2		a0,		t0,		a0
	sext.w		t0,		a0
	.endr
	ret
	.endm

	.option		push
	.option		arch, +zba
	seq_lat_zba_sext	32_3_zba,	sh1add
	seq_lat_zba_sext	32_5_zba,	sh2add
	seq_lat_zba_sext	32_9_zba,	sh3add
	seq_lat_zba_shift	32_6_zba,	sh1add,	slliw,	1
	seq_lat_zba_shift	64_6_zba,	sh1add,	slli,	1

	.global		seq_lat_64_li_long
seq_lat_64_li_long:
	.rept		BATCHSIZE
	lui		t0,		0xfff91
	addiw		t0,		t0,		965
	slli.uw		t0,		t0,		13
	addi		t0,		t0,		-1347
	slli		t0,		t0,		12
	addi		t0,		t0,		-529
	mul		a0,		a0,		t0
	.endr
	ret
	.option		pop

	seq_lat_2s	32_3,	slliw,	2,	subw
	seq_lat_2s	32_5,	slliw,	2,	addw
	seq_lat_2s	32_9,	slliw,	3,	addw
	seq_lat_2s	32_7,	slliw,	3,	subw
	seq_lat_2s	32_17,	slliw,	4,	addw
	seq_lat_2s	32_33,	slliw,	5,	addw
	seq_lat_3s	32_6,	slliw,	1,	addw,	1
	seq_lat_2s	64_3,	slli,	2,	sub
	seq_lat_2s	64_5,	slli,	2,	add
	seq_lat_2s	64_9,	slli,	3,	add
	seq_lat_2s	64_7,	slli,	3,	sub
	seq_lat_2s	64_17,	slli,	4,	add
	seq_lat_2s	64_33,	slli,	5,	add
	seq_lat_3s	64_6,	slli,	1,	add,	1

	seq_lat_ovf	32_ovf7,	slliw,	3,	subw

	.global		seq_lat_32_li_addi
seq_lat_32_li_addi:
	.rept		BATCHSIZE
	addi		t0,		zero,		11
	mulw		a0,		a0,		t0
	.endr
	ret

	.global		seq_lat_32_li_lui
seq_lat_32_li_lui:
	.rept		BATCHSIZE
	lui		t0,		0xfff80
	mulw		a0,		a0,		t0
	.endr
	ret

	.global		seq_lat_32_li_lui_addiw
seq_lat_32_li_lui_addiw:
	.rept		BATCHSIZE
	lui		t0,		0x12345
	addiw		t0,		t0,		1656
	mulw		a0,		a0,		t0
	.endr
	ret

	.option		push
	.option		arch, +zbs
	.global		seq_lat_64_li_bseti
seq_lat_64_li_bseti:
	.rept		BATCHSIZE
	bseti		t0,		zero,		63
	mul		a0,		a0,		t0
	.endr
	ret
	.option		pop

	.global		seq_lat_64_li_addi_slli
seq_lat_64_li_addi_slli:
	.rept		BATCHSIZE
	addi		t0,		zero,		-1
	slli		t0,		t0,		63
	mul		a0,		a0,		t0
	.endr
	ret

	.global		seq_lat_64_li_lui_addiw
seq_lat_64_li_lui_addiw:
	.rept		BATCHSIZE
	lui		t0,		0x12345
	addiw		t0,		t0,		1656
	mul		a0,		a0,		t0
	.endr
	ret

/*	Throughput: CPI is cycles per multiply	*/

	.macro	seq_thr_2s	name,	shiftop,	shift,	op2

	.global	seq_thr_\name
seq_thr_\name:
	.rept	BATCHSIZE / 4
	\shiftop	t0,		a0,		\shift
	\op2		a0,		t0,		a0
	\shiftop	t1,		a1,		\shift
	\op2		a1,		t1,		a1
	\shiftop	t2,		a2,		\shift
	\op2		a2,		t2,		a2
	\shiftop	t3,		a3,		\shift
	\op2		a3,		t3,		a3
	.endr
	ret
	.endm

	.macro	seq_thr_3s	name,	shiftop,	shift,	op2,	finalshift

	.global	seq_thr_\name
seq_thr_\name:
	.rept	BATCHSIZE / 4
	\shiftop	t0,		a0,		\shift
	\op2		a0,		t0,		a0
	\shiftop	a0,		a0,		\finalshift
	\shiftop	t1,		a1,		\shift
	\op2		a1,		t1,		a1
	\shiftop	a1,		a1,		\finalshift
	\shiftop	t2,		a2,		\shift
	\op2		a2,		t2,		a2
	\shiftop	a2,		a2,		\finalshift
	\shiftop	t3,		a3,		\shift
	\op2		a3,		t3,		a3
	\shiftop	a3,		a3,		\finalshift
	.endr
	ret
	.endm

	.macro	seq_thr_zba_sext	name,	addop

	.global	seq_thr_\name
seq_thr_\name:
	.rept	BATCHSIZE / 4
	\addop		a0,		a0,		a0
	sext.w		a0,		a0
	\addop		a1,		a1,		a1
	sext.w		a1,		a1
	\addop		a2,		a2,		a2
	sext.w		a2,		a2
	\addop		a3,		a3,		a3
	sext.w		a3,		a3
	.endr
	ret
	.endm

	.macro	seq_thr_zba_shift	name,	addop,	shiftop,	imm

	.global	seq_thr_\name
seq_thr_\name:
	.rept	BATCHSIZE / 4
	\addop		a0,		a0,		a0
	\shiftop	a0,		a0,		\imm
	\addop		a1,		a1,		a1
	\shiftop	a1,		a1,		\imm
	\addop		a2,		a2,		a2
	\shiftop	a2,		a2,		\imm
	\addop		a3,		a3,		a3
	\shiftop	a3,		a3,		\imm
	.endr
	ret
	.endm

	.option		push
	.option		arch, +zba
	seq_thr_zba_sext	32_3_zba,	sh1add
	seq_thr_zba_sext	32_5_zba,	sh2add
	seq_thr_zba_sext	32_9_zba,	sh3add
	seq_thr_zba_shift	32_6_zba,	sh1add,	slliw,	1
	seq_thr_zba_shift	64_6_zba,	sh1add,	slli,	1
	.option		pop

	seq_thr_2s	32_3,	slliw,	2,	subw
	seq_thr_2s	32_5,	slliw,	2,	addw
	seq_thr_2s	32_9,	slliw,	3,	addw
	seq_thr_2s	32_7,	slliw,	3,	subw
	seq_thr_2s	32_17,	slliw,	4,	addw
	seq_thr_2s	32_33,	slliw,	5,	addw
	seq_thr_3s	32_6,	slliw,	1,	addw,	1
	seq_thr_2s	64_3,	slli,	2,	sub
	seq_thr_2s	64_5,	slli,	2,	add
	seq_thr_2s	64_9,	slli,	3,	add
	seq_thr_2s	64_7,	slli,	3,	sub
	seq_thr_2s	64_17,	slli,	4,	add
	seq_thr_2s	64_33,	slli,	5,	add
	seq_thr_3s	64_6,	slli,	1,	add,	1

	.global		seq_thr_32_li_addi
seq_thr_32_li_addi:
	.rept		BATCHSIZE / 4
	addi		t0,		zero,		11
	mulw		a0,		a0,		t0
	addi		t1,		zero,		11
	mulw		a1,		a1,		t1
	addi		t2,		zero,		11
	mulw		a2,		a2,		t2
	addi		t3,		zero,		11
	mulw		a3,		a3,		t3
	.endr
	ret

	.global		seq_thr_32_li_lui
seq_thr_32_li_lui:
	.rept		BATCHSIZE / 4
	lui		t0,		0xfff80
	mulw		a0,		a0,		t0
	lui		t1,		0xfff80
	mulw		a1,		a1,		t1
	lui		t2,		0xfff80
	mulw		a2,		a2,		t2
	lui		t3,		0xfff80
	mulw		a3,		a3,		t3
	.endr
	ret

	.global		seq_thr_32_li_lui_addiw
seq_thr_32_li_lui_addiw:
	.rept		BATCHSIZE / 4
	lui		t0,		0x12345
	addiw		t0,		t0,		1656
	mulw		a0,		a0,		t0
	lui		t1,		0x12345
	addiw		t1,		t1,		1656
	mulw		a1,		a1,		t1
	lui		t2,		0x12345
	addiw		t2,		t2,		1656
	mulw		a2,		a2,		t2
	lui		t3,		0x12345
	addiw		t3,		t3,		1656
	mulw		a3,		a3,		t3
	.endr
	ret

	.option		push
	.option		arch, +zbs
	.global		seq_thr_64_li_bseti
seq_thr_64_li_bseti:
	.rept		BATCHSIZE / 4
	bseti		t0,		zero,		63
	mul		a0,		a0,		t0
	bseti		t1,		zero,		63
	mul		a1,		a1,		t1
	bseti		t2,		zero,		63
	mul		a2,		a2,		t2
	bseti		t3,		zero,		63
	mul		a3,		a3,		t3
	.endr
	ret
	.option		pop

	.global		seq_thr_64_li_addi_slli
seq_thr_64_li_addi_slli:
	.rept		BATCHSIZE / 4
	addi		t0,		zero,		-1
	slli		t0,		t0,		63
	mul		a0,		a0,		t0
	addi		t1,		zero,		-1
	slli		t1,		t1,		63
	mul		a1,		a1,		t1
	addi		t2,		zero,		-1
	slli		t2,		t2,		63
	mul		a2,		a2,		t2
	addi		t3,		zero,		-1
	slli		t3,		t3,		63
	mul		a3,		a3,		t3
	.endr
	ret

	.global		seq_thr_64_li_lui_addiw
seq_thr_64_li_lui_addiw:
	.rept		BATCHSIZE / 4
	lui		t0,		0x12345
	addiw		t0,		t0,		1656
	mul		a0,		a0,		t0
	lui		t1,		0x12345
	addiw		t1,		t1,		1656
	mul		a1,		a1,		t1
	lui		t2,		0x12345
	addiw		t2,		t2,		1656
	mul		a2,		a2,		t2
	lui		t3,		0x12345
	addiw		t3,		t3,		1656
	mul		a3,		a3,		t3
	.endr
	ret
