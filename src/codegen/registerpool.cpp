#include "registerpool.h"

Register* RegisterPool::a0 = nullptr;
Register* RegisterPool::fa0 = nullptr;
Register* RegisterPool::a1 = nullptr;
Register* RegisterPool::fa1 = nullptr;

RegisterPool::RegisterPool() {
	for (int i = 0; i < 7; i++) {
		Register* reg = new Register(RegType::T, "t" + std::to_string(i));
		pool[reg->getType()].insert(reg);
	}
	for (int i = 0; i < 12; i++) {
		Register* reg = new Register(RegType::S, "s" + std::to_string(i));
		pool[reg->getType()].insert(reg);
	}
	for (int i = 0; i < 2; i++) {
		Register* reg = new Register(RegType::AR, "a" + std::to_string(i));
		if (i == 0) {
			a0 = reg;
		}
		if (i == 1) {
			a1 = reg;
		}
		pool[RegType::AR].insert(reg);
	}
	freePools[RegType::AR] = pool[RegType::AR];
	
	for (int i = 2; i < 8; i++) {
		Register* reg = new Register(RegType::A, "a" + std::to_string(i));
		pool[reg->getType()].insert(reg);
	}
	freePools[RegType::A] = pool[RegType::A];
	for (int i = 0; i < 12; i++) {
		Register* reg = new Register(RegType::FT, "ft" + std::to_string(i));
		pool[reg->getType()].insert(reg);
	}
	for (int i = 0; i < 12; i++) {
		Register* reg = new Register(RegType::FS, "fs" + std::to_string(i));
		pool[reg->getType()].insert(reg);
	}
	for (int i = 0; i < 2; i++) {
		Register* reg = new Register(RegType::FAR, "fa" + std::to_string(i));
		if (i == 0) {
			fa0 = reg;
		}
		if (i == 1) {
			fa1 = reg;
		}
		pool[RegType::FAR].insert(reg);
	}
	freePools[RegType::FAR] = pool[RegType::FAR];
	
	for (int i = 2; i < 8; i++) {
		Register* reg = new Register(RegType::FA, "fa" + std::to_string(i));
		pool[reg->getType()].insert(reg);
	}
	freePools[RegType::FA] = pool[RegType::FA];
}

Register* RegisterPool::takeFirstFreeReg(RegType type) {
	if (pool[type].empty()) return nullptr;
	Register* reg = *(pool[type].begin());
	pool[type].erase(pool[type].begin());
	reversedPool[type].insert(reg);
	return reg;
}

void RegisterPool::freeRegister(Register* reg)
{
	reversedPool[reg->getType()].erase(reg);
	pool[reg->getType()].insert(reg);
}

int RegisterPool::getRegCount(RegType type)
{
	return pool[type].size();
}
