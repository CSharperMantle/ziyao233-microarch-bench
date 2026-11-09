// SPDX-License-Identifier: AGPL-3.0-or-later

#if !defined(__riscv) || __riscv_xlen != 64
#error "riscv64/arithmetic.S targets RV64 only"
#endif

/*	Latency	*/

	.macro		latency_3reg	name,		opcode

	.global		latency_\name
latency_\name:
	.rept		BATCHSIZE
	\opcode		a0,		a0,		a0
	.endr
	ret
	.endm

	latency_3reg	add,		add
	latency_3reg	addw,		addw
	latency_3reg	and,		and
	latency_3reg	sub,		sub
	latency_3reg	subw,		subw
	latency_3reg	sll,		sll
	latency_3reg	sllw,		sllw
	latency_3reg	srl,		srl
	latency_3reg	sra,		sra
	latency_3reg	mul,		mul
	latency_3reg	mulw,		mulw
	latency_3reg	mulh,		mulh
	latency_3reg	mulhu,		mulhu
	latency_3reg	mulhsu,		mulhsu
	latency_3reg	rem,		rem
	latency_3reg	remu,		remu

	.option		push
	.option		arch, +zba
	latency_3reg	sh1add,		sh1add
	latency_3reg	sh2add,		sh2add
	latency_3reg	sh3add,		sh3add
	latency_3reg	add_uw,		add.uw
	latency_3reg	sh1add_uw,	sh1add.uw
	latency_3reg	sh2add_uw,	sh2add.uw
	latency_3reg	sh3add_uw,	sh3add.uw
	.option		pop

	.option		push
	.option		arch, +zbb
	latency_3reg	andn,		andn
	latency_3reg	ror,		ror
	latency_3reg	rorw,		rorw
	latency_3reg	rol,		rol
	latency_3reg	rolw,		rolw
	.option		pop

	.option		push
	.option		arch, +zbs
	latency_3reg	bset,		bset
	latency_3reg	bclr,		bclr
	latency_3reg	binv,		binv
	latency_3reg	bext,		bext
	.option		pop

	.macro		latency_2reg	name,		opcode

	.global		latency_\name
latency_\name:
	.rept		BATCHSIZE
	\opcode		a0,		a0
	.endr
	ret
	.endm

	latency_2reg	sext_w,		sext.w

	.option		push
	.option		arch, +zbb
	latency_2reg	clz,		clz
	latency_2reg	clzw,		clzw
	latency_2reg	ctz,		ctz
	latency_2reg	ctzw,		ctzw
	latency_2reg	cpop,		cpop
	latency_2reg	cpopw,		cpopw
	latency_2reg	sext_b,		sext.b
	latency_2reg	sext_h,		sext.h
	latency_2reg	rev8,		rev8
	.option		pop

	.option		push
	.option		arch, +zbkb
	latency_2reg	brev8,		brev8
	.option		pop

	.macro		latency_imm	name,		opcode,		imm

	.global		latency_\name
latency_\name:
	.rept		BATCHSIZE
	\opcode		a0,		a0,		\imm
	.endr
	ret
	.endm

	latency_imm	slli,		slli,		1
	latency_imm	slliw,		slliw,		1

	.option		push
	.option		arch, +zba
	latency_imm	slli_uw,	slli.uw,	1
	.option		pop

	.option		push
	.option		arch, +zbb
	latency_imm	rori,		rori,		1
	.option		pop

	.option		push
	.option		arch, +zbs
	latency_imm	bseti,		bseti,		1
	latency_imm	bclri,		bclri,		1
	latency_imm	binvi,		binvi,		1
	latency_imm	bexti,		bexti,		1
	.option		pop

	.macro		latency_cond	name,		opcode

	.global		latency_\name
latency_\name:
	.rept		BATCHSIZE
	\opcode		t0,		t0,		a0
	.endr
	ret
	.endm

	.option		push
	.option		arch, +zicond
	latency_cond	czero_eqz,	czero.eqz
	latency_cond	czero_nez,	czero.nez
	.option		pop

	.macro		latency_div_add	name,		opcode

	.global		latency_\name
latency_\name:
	.rept		BATCHSIZE
	\opcode		a0,		a0,		a1
	add		a0,		a0,		a2
	.endr
	ret
	.endm

	latency_div_add	div_add,	div
	latency_div_add	divu_add,	divu

/*	Throughput	*/

	.macro		thrput_3reg	name,		opcode

	.global		thrput_\name
thrput_\name:
	.rept		BATCHSIZE / 32
	\opcode		t0,		s1,		a0
	\opcode		t1,		a6,		a1
	\opcode		t2,		a5,		a2
	\opcode		t3,		a4,		a3
	\opcode		t4,		a3,		a4
	\opcode		t5,		a2,		a5
	\opcode		t6,		a1,		a6
	\opcode		a7,		a0,		s1

	\opcode		t0,		a6,		a0
	\opcode		t1,		a5,		a1
	\opcode		t2,		a4,		a2
	\opcode		t3,		a3,		a3
	\opcode		t4,		a2,		a4
	\opcode		t5,		a1,		a5
	\opcode		t6,		a0,		a6
	\opcode		a7,		s1,		s1

	\opcode		t0,		a5,		a0
	\opcode		t1,		a4,		a1
	\opcode		t2,		a3,		a2
	\opcode		t3,		a2,		a3
	\opcode		t4,		a1,		a4
	\opcode		t5,		a0,		a5
	\opcode		t6,		s1,		a6
	\opcode		a7,		a6,		s1

	\opcode		t0,		a4,		a0
	\opcode		t1,		a3,		a1
	\opcode		t2,		a2,		a2
	\opcode		t3,		a1,		a3
	\opcode		t4,		a0,		a4
	\opcode		t5,		s1,		a5
	\opcode		t6,		a6,		a6
	\opcode		a7,		s1,		s1
	.endr
	ret
	.endm

	thrput_3reg	add,		add
	thrput_3reg	addw,		addw
	thrput_3reg	and,		and
	thrput_3reg	sub,		sub
	thrput_3reg	subw,		subw
	thrput_3reg	sll,		sll
	thrput_3reg	sllw,		sllw
	thrput_3reg	srl,		srl
	thrput_3reg	sra,		sra
	thrput_3reg	mul,		mul
	thrput_3reg	mulw,		mulw
	thrput_3reg	mulh,		mulh
	thrput_3reg	mulhu,		mulhu
	thrput_3reg	mulhsu,		mulhsu
	thrput_3reg	rem,		rem
	thrput_3reg	remu,		remu

	.option		push
	.option		arch, +zba
	thrput_3reg	sh1add,		sh1add
	thrput_3reg	sh2add,		sh2add
	thrput_3reg	sh3add,		sh3add
	thrput_3reg	add_uw,		add.uw
	thrput_3reg	sh1add_uw,	sh1add.uw
	thrput_3reg	sh2add_uw,	sh2add.uw
	thrput_3reg	sh3add_uw,	sh3add.uw
	.option		pop

	.option		push
	.option		arch, +zbb
	thrput_3reg	andn,		andn
	thrput_3reg	ror,		ror
	thrput_3reg	rorw,		rorw
	thrput_3reg	rol,		rol
	thrput_3reg	rolw,		rolw
	.option		pop

	.option		push
	.option		arch, +zbs
	thrput_3reg	bset,		bset
	thrput_3reg	bclr,		bclr
	thrput_3reg	binv,		binv
	thrput_3reg	bext,		bext
	.option		pop

	.macro		thrput_2reg	name,		opcode

	.global		thrput_\name
thrput_\name:
	.rept		BATCHSIZE / 16
	\opcode		t0,		a0
	\opcode		t1,		a1
	\opcode		t2,		a2
	\opcode		t3,		a3
	\opcode		t4,		a4
	\opcode		t5,		a5
	\opcode		t6,		a6
	\opcode		a7,		s1

	\opcode		t0,		s2
	\opcode		t1,		s3
	\opcode		t2,		s4
	\opcode		t3,		s5
	\opcode		t4,		s6
	\opcode		t5,		s7
	\opcode		t6,		s8
	\opcode		a7,		s9
	.endr
	ret
	.endm

	thrput_2reg	sext_w,		sext.w

	.option		push
	.option		arch, +zbb
	thrput_2reg	clz,		clz
	thrput_2reg	clzw,		clzw
	thrput_2reg	ctz,		ctz
	thrput_2reg	ctzw,		ctzw
	thrput_2reg	cpop,		cpop
	thrput_2reg	cpopw,		cpopw
	thrput_2reg	sext_b,		sext.b
	thrput_2reg	sext_h,		sext.h
	thrput_2reg	rev8,		rev8
	.option		pop

	.option		push
	.option		arch, +zbkb
	thrput_2reg	brev8,		brev8
	.option		pop

	.macro		thrput_imm	name,		opcode,		imm

	.global		thrput_\name
thrput_\name:
	.rept		BATCHSIZE / 16
	\opcode		t0,		a0,		\imm
	\opcode		t1,		a1,		\imm
	\opcode		t2,		a2,		\imm
	\opcode		t3,		a3,		\imm
	\opcode		t4,		a4,		\imm
	\opcode		t5,		a5,		\imm
	\opcode		t6,		a6,		\imm
	\opcode		a7,		s1,		\imm

	\opcode		t0,		s2,		\imm
	\opcode		t1,		s3,		\imm
	\opcode		t2,		s4,		\imm
	\opcode		t3,		s5,		\imm
	\opcode		t4,		s6,		\imm
	\opcode		t5,		s7,		\imm
	\opcode		t6,		s8,		\imm
	\opcode		a7,		s9,		\imm
	.endr
	ret
	.endm

	thrput_imm	slli,		slli,		1
	thrput_imm	slliw,		slliw,		1

	.option		push
	.option		arch, +zba
	thrput_imm	slli_uw,	slli.uw,	1
	.option		pop

	.option		push
	.option		arch, +zbb
	thrput_imm	rori,		rori,		1
	.option		pop

	.option		push
	.option		arch, +zbs
	thrput_imm	bseti,		bseti,		1
	thrput_imm	bclri,		bclri,		1
	thrput_imm	binvi,		binvi,		1
	thrput_imm	bexti,		bexti,		1
	.option		pop

	.macro		thrput_cond	name,		opcode

	.global		thrput_\name
thrput_\name:
	.rept		BATCHSIZE / 8
	\opcode		t0,		s2,		a0
	\opcode		t1,		s3,		a0
	\opcode		t2,		s4,		a0
	\opcode		t3,		s5,		a0
	\opcode		t4,		s6,		a0
	\opcode		t5,		s7,		a0
	\opcode		t6,		s8,		a0
	\opcode		a7,		s9,		a0
	.endr
	ret
	.endm

	.option		push
	.option		arch, +zicond
	thrput_cond	czero_eqz,	czero.eqz
	thrput_cond	czero_nez,	czero.nez
	.option		pop

	.macro		thrput_div	name,		opcode

	.global		thrput_\name
thrput_\name:
	.rept		BATCHSIZE / 8
	\opcode		t0,		a0,		a1
	\opcode		t1,		a0,		a1
	\opcode		t2,		a0,		a1
	\opcode		t3,		a0,		a1
	\opcode		t4,		a0,		a1
	\opcode		t5,		a0,		a1
	\opcode		t6,		a0,		a1
	\opcode		a7,		a0,		a1
	.endr
	ret
	.endm

	thrput_div	div,		div
	thrput_div	divu,		divu

	.global		thrput_nop
thrput_nop:
	.rept		BATCHSIZE
	nop
	.endr
	ret
