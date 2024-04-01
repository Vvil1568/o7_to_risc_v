#include "registerpool.h"

Register* RegisterPool::a0 = nullptr;
Register* RegisterPool::fa0 = nullptr;

RegisterPool::RegisterPool() {
	for (int i = 0; i < 7; i++) {
		Register* reg = new Register(RegType::T, "t" + std::to_string(i));
		pool[reg->getType()].insert(reg);
	}
	for (int i = 0; i < 12; i++) {
		Register* reg = new Register(RegType::S, "s" + std::to_string(i));
		pool[reg->getType()].insert(reg);
	}
	for (int i = 0; i < 8; i++) {
		Register* reg = new Register(RegType::A, "a" + std::to_string(i));
		if (i == 0) {
			a0 = reg;
		}
		pool[reg->getType()].insert(reg);
	}
	for (int i = 0; i < 12; i++) {
		Register* reg = new Register(RegType::FT, "ft" + std::to_string(i));
		pool[reg->getType()].insert(reg);
	}
	for (int i = 0; i < 12; i++) {
		Register* reg = new Register(RegType::FS, "fs" + std::to_string(i));
		pool[reg->getType()].insert(reg);
	}
	for (int i = 0; i < 8; i++) {
		Register* reg = new Register(RegType::FA, "fa" + std::to_string(i));
		if (i == 0) {
			fa0 = reg;
		}
		pool[reg->getType()].insert(reg);
	}
}

Register* RegisterPool::takeFirstFreeReg(RegType type) {
	if (pool[type].empty()) return nullptr;
	Register* reg = *(pool[type].begin());
	pool[type].erase(pool[type].begin());
	return reg;
}

void RegisterPool::freeRegister(Register* reg)
{
	pool[reg->getType()].insert(reg);
}
