#include "rsp.hpp"
#include "rsp_op.hpp"

namespace RSP
{
CPU::CPU()
{
	memory_map[0].length = 0x1000;
	memory_map[1].length = 0x1000;

	ops.cop0_mfc = RSP_MFC0;
	ops.cop0_mtc = RSP_MTC0;

	ops.cop2_mtc = RSP_MTC2;
	ops.cop2_mfc = RSP_MFC2;
	ops.cop2_ctc = RSP_CTC2;
	ops.cop2_cfc = RSP_CFC2;

	ops.cop2_lwc = RSP_LWC2;
	ops.cop2_swc = RSP_SWC2;
	ops.cop2_vecop = RSP_COP2;

	char tmp[] = "RSP";
	lightrec = lightrec_init(tmp, memory_map, 2, &ops);
	lightrec_set_invalidate_mode(lightrec, true);
}

CPU::~CPU()
{
	lightrec_destroy(lightrec);
}

void CPU::invalidate_imem()
{
	lightrec_invalidate_all(lightrec);
}

ReturnMode CPU::run()
{
	lightrec_reset_cycle_count(lightrec, 0);
	//state.pc = lightrec_execute(lightrec, state.pc, ~0u);
	fprintf(stderr, "Running interpreter, PC: 0x%08x\n", state.pc);
	state.pc = lightrec_run_interpreter(lightrec, state.pc);
	fprintf(stderr, "Ran interpreter, PC: 0x%08x\n", state.pc);
	auto exit_flags = lightrec_exit_flags(lightrec);

	if (exit_flags & LIGHTREC_EXIT_BREAK)
	{
		*state.cp0.cr[CP0_REGISTER_SP_STATUS] |= SP_STATUS_BROKE | SP_STATUS_HALT;
		if (*state.cp0.cr[CP0_REGISTER_SP_STATUS] & SP_STATUS_INTR_BREAK)
			*state.cp0.irq |= 1;
		return MODE_BREAK;
	}
	else if (exit_flags & LIGHTREC_EXIT_MISC)
		return MODE_CHECK_FLAGS;
	else
		return MODE_ENTER;
}
}
