#ifndef RSP_HPP__
#define RSP_HPP__

#include <stdint.h>
#include <string.h>
#include <memory>

#include "state.hpp"
#include "lightrec.h"

namespace RSP
{
enum ReturnMode
{
	MODE_ENTER = 0,
	MODE_CONTINUE = 1,
	MODE_BREAK = 2,
	MODE_DMA_READ = 3,
	MODE_CHECK_FLAGS = 4
};

class alignas(64) CPU
{
public:
	CPU();
	~CPU();

	CPU(CPU&&) = delete;
	void operator=(CPU&&) = delete;

	void set_dmem(uint32_t *dmem)
	{
		state.dmem = dmem;
		memory_map[0].address = dmem;
	}

	void set_imem(uint32_t *imem)
	{
		state.imem = imem;
		memory_map[1].address = imem;
	}

	void set_rdram(uint32_t *rdram)
	{
		state.rdram = rdram;
	}

	CPUState &get_state()
	{
		return state;
	}

	void invalidate_imem();

	ReturnMode run();

private:
	lightrec_state *lightrec = nullptr;
	lightrec_mem_map memory_map[2] = {};
	lightrec_ops ops = {};
	CPUState state;
};
}

#endif
