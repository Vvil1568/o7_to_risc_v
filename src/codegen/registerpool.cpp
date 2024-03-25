#include "registerpool.h"

RegisterPool::RegisterPool() {
	for (int i = 0; i < 7; i++) {
		Register reg(RegType::T, "t" + i);
		pool[reg.getType()].insert(reg);
	}
	for (int i = 0; i < 12; i++) {
		Register reg(RegType::S, "s" + i);
		pool[reg.getType()].insert(reg);
	}
	for (int i = 0; i < 8; i++) {
		Register reg(RegType::A, "a" + i);
		pool[reg.getType()].insert(reg);
	}
	for (int i = 0; i < 12; i++) {
		Register reg(RegType::FT, "ft" + i);
		pool[reg.getType()].insert(reg);
	}
	for (int i = 0; i < 12; i++) {
		Register reg(RegType::FS, "fs" + i);
		pool[reg.getType()].insert(reg);
	}
	for (int i = 0; i < 8; i++) {
		Register reg(RegType::FA, "fa" + i);
		pool[reg.getType()].insert(reg);
	}
}

Register RegisterPool::takeFirstFreeReg(RegType type) {
	Register reg = *(pool[type].begin());
	pool[type].erase(pool[type].begin());
	return reg;
}

void RegisterPool::freeRegister(Register reg)
{
	pool[reg.getType()].insert(reg);
}
