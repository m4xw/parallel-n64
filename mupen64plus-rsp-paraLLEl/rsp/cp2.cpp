#include "../rsp.hpp"
#include "../state.hpp"
#include "../rsp_op.hpp"
#include <assert.h>

namespace RSP
{
	extern CPU cpu;
}

extern "C" {
u32 RSP_CFC2(lightrec_state *, u32 OPCODE, u32 rd)
{
	auto *rsp = &RSP::cpu.get_state();
	unsigned src = rd & 3;
	if (src == 3)
		src = 2;

	int16_t res = rsp_get_flags(rsp->cp2.flags[src].e);
	return res;
}

void RSP_CTC2(lightrec_state *, u32 OPCODE, u32 rt, u32 rd)
{
	auto *rsp = &RSP::cpu.get_state();
	rt &= 0xffff;

	unsigned dst = rd & 3;
	if (dst >= 2)
	{
		rt &= 0xff;
		dst = 2;
	}
	rsp_set_flags(rsp->cp2.flags[dst].e, rt);
}

void RSP_MTC2(lightrec_state *, u32 OPCODE, u32 rt, u32 rd, u32 element)
{
	auto *rsp = &RSP::cpu.get_state();
	uint16_t *e = rsp->cp2.regs[rd].e;

	unsigned lo = element >> 1;

	if (element & 1)
	{
		unsigned hi = (element + 1) >> 1;
		e[lo] = (e[lo] & 0xff00) | ((rt >> 8) & 0xff);
		e[hi] = (e[lo] & 0x00ff) | ((rt & 0xff) << 8);
	}
	else
		e[lo] = rt;
}

u32 RSP_MFC2(lightrec_state *, u32 OPCODE, u32 rd, u32 element)
{
	auto *rsp = &RSP::cpu.get_state();

	const uint16_t *e = rsp->cp2.regs[rd].e;
	unsigned lo = element >> 1;

	if (element & 1)
	{
		unsigned hi = ((element + 1) >> 1) & 7;
		uint16_t high = e[lo] << 8;
		uint8_t low = e[hi] >> 8;
		return int16_t(high | low);
	}
	else
		return int16_t(e[lo]);
}

void RSP_LWC2(lightrec_state *, u32 instr, u32 op, u32 rt, u32 imm, s32 simm, u32 rs)
{
#if 1
	unsigned ref_rt = (instr >> 16) & 31;
	int16_t ref_simm = instr;
	// Sign extend.
	ref_simm <<= 9;
	ref_simm >>= 9;
	unsigned ref_rs = (instr >> 21) & 31;
	unsigned ref_rd = (instr >> 11) & 31;
	unsigned ref_imm = (instr >> 7) & 15;
	assert(ref_rt == rt);
	assert(ref_simm == simm);
	assert(ref_rd == op);
	assert(ref_imm == imm);
#endif
	auto *rsp = &RSP::cpu.get_state();

	using LWC2Op = void (*)(RSP::CPUState *, unsigned, unsigned, int, unsigned);

	static const LWC2Op ops[32] = {
		RSP_LBV, RSP_LSV, RSP_LLV, RSP_LDV, RSP_LQV, RSP_LRV, RSP_LPV, RSP_LUV,
		RSP_LHV, nullptr, nullptr, RSP_LTV, nullptr, nullptr, nullptr, nullptr,
	};

	if (ops[op])
		ops[op](rsp, rt, imm, simm, rs);
}

void RSP_SWC2(lightrec_state *, u32 instr, u32 op, u32 rt, u32 imm, s32 simm, u32 rs)
{
#if 1
	unsigned ref_rt = (instr >> 16) & 31;
	int16_t ref_simm = instr;
	// Sign extend.
	ref_simm <<= 9;
	ref_simm >>= 9;
	unsigned ref_rs = (instr >> 21) & 31;
	unsigned ref_rd = (instr >> 11) & 31;
	unsigned ref_imm = (instr >> 7) & 15;
	assert(ref_rt == rt);
	assert(ref_simm == simm);
	assert(ref_rd == op);
	assert(ref_imm == imm);
#endif
	auto *rsp = &RSP::cpu.get_state();

	using SWC2Op = void (*)(RSP::CPUState *, unsigned, unsigned, int, unsigned);
	static const SWC2Op ops[32] = {
		RSP_SBV, RSP_SSV, RSP_SLV, RSP_SDV, RSP_SQV, RSP_SRV, RSP_SPV, RSP_SUV,
		RSP_SHV, RSP_SFV, nullptr, RSP_STV, nullptr, nullptr, nullptr, nullptr,
	};

	if (ops[op])
		ops[op](rsp, rt, imm, simm, rs);
}

void RSP_COP2(lightrec_state *, u32 instr, u32 op, u32 vd, u32 vs, u32 vt, u32 e)
{
	auto *rsp = &RSP::cpu.get_state();

#if 1
	// VU instruction.
	uint32_t ref_op = instr & 63;
	uint32_t ref_vd = (instr >> 6) & 31;
	uint32_t ref_vs = (instr >> 11) & 31;
	uint32_t ref_vt = (instr >> 16) & 31;
	uint32_t ref_e  = (instr >> 21) & 15;
	assert(ref_op == op);
	assert(ref_vd == vd);
	assert(ref_vs == vs);
	assert(ref_vt == vt);
	assert(ref_e == e);
#endif

	using COP2Op = void (*)(RSP::CPUState *, uint32_t, uint32_t, uint32_t, uint32_t);
	static const COP2Op ops[64] = {
		RSP_VMULF, RSP_VMULU, nullptr, nullptr, RSP_VMUDL, RSP_VMUDM, RSP_VMUDN, RSP_VMUDH,
		RSP_VMACF, RSP_VMACU, nullptr, nullptr, RSP_VMADL, RSP_VMADM, RSP_VMADN, RSP_VMADH,
		RSP_VADD, RSP_VSUB, nullptr, RSP_VABS, RSP_VADDC, RSP_VSUBC, nullptr, nullptr,
		nullptr, nullptr, nullptr, nullptr, nullptr, RSP_VSAR, nullptr, nullptr,
		RSP_VLT, RSP_VEQ, RSP_VNE, RSP_VGE, RSP_VCL, RSP_VCH, RSP_VCR, RSP_VMRG,
		RSP_VAND, RSP_VNAND, RSP_VOR, RSP_VNOR, RSP_VXOR, RSP_VNXOR, nullptr, nullptr,
		RSP_VRCP, RSP_VRCPL, RSP_VRCPH, RSP_VMOV, RSP_VRSQ, RSP_VRSQL, RSP_VRSQH, RSP_VNOP,
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	};

	if (ops[op])
		ops[op](rsp, vd, vs, vt, e);
}

}

