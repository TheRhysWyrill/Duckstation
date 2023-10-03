// SPDX-FileCopyrightText: 2019-2023 Connor McLaughlin <stenzek@gmail.com>
// SPDX-License-Identifier: (GPL-3.0 OR CC-BY-NC-ND-4.0)

// Shared code between recompiler backends.

#pragma once
#include "cpu_types.h"

#if defined(CPU_ARCH_X64)

// We need to include windows.h before xbyak does..
#ifdef _WIN32
#include "common/windows_headers.h"
#endif

#define XBYAK_NO_OP_NAMES 1
#include "xbyak.h"

namespace CPU::Recompiler {

// A reasonable "maximum" number of bytes per instruction.
constexpr u32 MAX_NEAR_HOST_BYTES_PER_INSTRUCTION = 64;
constexpr u32 MAX_FAR_HOST_BYTES_PER_INSTRUCTION = 128;

// ABI selection
#if defined(_WIN32)
#define ABI_WIN64 1

#define RWRET Xbyak::Reg32(Xbyak::Operand::EAX)
#define RWARG1 Xbyak::Reg32(Xbyak::Operand::RCX)
#define RWARG2 Xbyak::Reg32(Xbyak::Operand::RDX)
#define RWARG3 Xbyak::Reg32(Xbyak::Operand::R8D)
#define RWARG4 Xbyak::Reg32(Xbyak::Operand::R9D)
#define RXRET Xbyak::Reg64(Xbyak::Operand::RAX)
#define RXARG1 Xbyak::Reg64(Xbyak::Operand::RCX)
#define RXARG2 Xbyak::Reg64(Xbyak::Operand::RDX)
#define RXARG3 Xbyak::Reg64(Xbyak::Operand::R8)
#define RXARG4 Xbyak::Reg64(Xbyak::Operand::R9)

static constexpr u32 FUNCTION_CALL_SHADOW_SPACE = 32;

#elif defined(__linux__) || defined(__ANDROID__) || defined(__APPLE__) || defined(__FreeBSD__)
#define ABI_SYSV 1

#define RWRET Xbyak::Reg32(Xbyak::Operand::EAX)
#define RWARG1 Xbyak::Reg32(Xbyak::Operand::EDI)
#define RWARG2 Xbyak::Reg32(Xbyak::Operand::ESI)
#define RWARG3 Xbyak::Reg32(Xbyak::Operand::EDX)
#define RWARG4 Xbyak::Reg32(Xbyak::Operand::ECX)
#define RXRET Xbyak::Reg64(Xbyak::Operand::RAX)
#define RXARG1 Xbyak::Reg64(Xbyak::Operand::RDI)
#define RXARG2 Xbyak::Reg64(Xbyak::Operand::RSI)
#define RXARG3 Xbyak::Reg64(Xbyak::Operand::RDX)
#define RXARG4 Xbyak::Reg64(Xbyak::Operand::RCX)

static constexpr u32 FUNCTION_CALL_SHADOW_SPACE = 0;

#else
#error Unknown ABI.
#endif

bool IsCallerSavedRegister(u32 id);

} // namespace CPU::Recompiler

#elif defined(CPU_ARCH_ARM32)

#include "vixl/aarch32/assembler-aarch32.h"
#include "vixl/aarch32/constants-aarch32.h"
#include "vixl/aarch32/instructions-aarch32.h"

namespace CPU::Recompiler {

// A reasonable "maximum" number of bytes per instruction.
constexpr u32 MAX_NEAR_HOST_BYTES_PER_INSTRUCTION = 64;
constexpr u32 MAX_FAR_HOST_BYTES_PER_INSTRUCTION = 128;

#define RRET vixl::aarch32::r0
#define RARG1 vixl::aarch32::r0
#define RARG2 vixl::aarch32::r1
#define RARG3 vixl::aarch32::r2
#define RARG4 vixl::aarch32::r3
#define RSCRATCH vixl::aarch32::r12
#define RSTATE vixl::aarch32::r4
#define RMEMBASE vixl::aarch32::r5

s32 armGetPCDisplacement(const void* current, const void* target);
bool armIsPCDisplacementInImmediateRange(s32 displacement);
void armMoveAddressToReg(vixl::aarch32::Assembler* armAsm, const vixl::aarch32::Register& reg, const void* addr);
void armEmitMov(vixl::aarch32::Assembler* armAsm, const vixl::aarch32::Register& rd, u32 imm);
void armEmitJmp(vixl::aarch32::Assembler* armAsm, const void* ptr, bool force_inline);
void armEmitCall(vixl::aarch32::Assembler* armAsm, const void* ptr, bool force_inline);

} // namespace CPU::Recompiler

#elif defined(CPU_ARCH_ARM64)

#include "vixl/aarch64/assembler-aarch64.h"
#include "vixl/aarch64/constants-aarch64.h"

namespace CPU::Recompiler {

// A reasonable "maximum" number of bytes per instruction.
constexpr u32 MAX_NEAR_HOST_BYTES_PER_INSTRUCTION = 64;
constexpr u32 MAX_FAR_HOST_BYTES_PER_INSTRUCTION = 128;

#define RWRET vixl::aarch64::w0
#define RXRET vixl::aarch64::x0
#define RWARG1 vixl::aarch64::w0
#define RXARG1 vixl::aarch64::x0
#define RWARG2 vixl::aarch64::w1
#define RXARG2 vixl::aarch64::x1
#define RWARG3 vixl::aarch64::w2
#define RXARG3 vixl::aarch64::x2
#define RWARG4 vixl::aarch64::w3
#define RXARG4 vixl::aarch64::x3
#define RWSCRATCH vixl::aarch64::w16
#define RXSCRATCH vixl::aarch64::x16
#define RSTATE vixl::aarch64::x19
#define RMEMBASE vixl::aarch64::x20

bool armIsCallerSavedRegister(u32 id);
s64 armGetPCDisplacement(const void* current, const void* target);
void armMoveAddressToReg(vixl::aarch64::Assembler* armAsm, const vixl::aarch64::XRegister& reg, const void* addr);
void armEmitMov(vixl::aarch64::Assembler* armAsm, const vixl::aarch64::Register& rd, u64 imm);
void armEmitJmp(vixl::aarch64::Assembler* armAsm, const void* ptr, bool force_inline);
void armEmitCall(vixl::aarch64::Assembler* armAsm, const void* ptr, bool force_inline);
void armEmitCondBranch(vixl::aarch64::Assembler* armAsm, vixl::aarch64::Condition cond, const void* ptr);
u8* armGetJumpTrampoline(const void* target);

} // namespace CPU::Recompiler

#endif
