#include "register.hpp"

namespace ir {

BasicType *const BasicType::I1 = new BasicType(1, "i1");
BasicType *const BasicType::I32 = new BasicType(32, "i32");
BasicType *const BasicType::FLOAT = new BasicType(32, "float");
BasicType *const BasicType::VOID = new BasicType(0, "void");

} // namespace ir

namespace reg {

int Virtual::_counter = 0;

Machine *const Machine::ZERO = new Machine(ir::BasicType::I32, "zero");
Machine *const Machine::RA = new Machine(ir::BasicType::I32, "ra");
Machine *const Machine::SP = new Machine(ir::BasicType::I32, "sp");
Machine *const Machine::GP = new Machine(ir::BasicType::I32, "gp");
Machine *const Machine::TP = new Machine(ir::BasicType::I32, "tp");
Machine *const Machine::T0 = new Machine(ir::BasicType::I32, "t0");
Machine *const Machine::T1 = new Machine(ir::BasicType::I32, "t1");
Machine *const Machine::T2 = new Machine(ir::BasicType::I32, "t2");
Machine *const Machine::S0 = new Machine(ir::BasicType::I32, "s0");
Machine *const Machine::S1 = new Machine(ir::BasicType::I32, "s1");
Machine *const Machine::A0 = new Machine(ir::BasicType::I32, "a0");
Machine *const Machine::A1 = new Machine(ir::BasicType::I32, "a1");
Machine *const Machine::A2 = new Machine(ir::BasicType::I32, "a2");
Machine *const Machine::A3 = new Machine(ir::BasicType::I32, "a3");
Machine *const Machine::A4 = new Machine(ir::BasicType::I32, "a4");
Machine *const Machine::A5 = new Machine(ir::BasicType::I32, "a5");
Machine *const Machine::A6 = new Machine(ir::BasicType::I32, "a6");
Machine *const Machine::A7 = new Machine(ir::BasicType::I32, "a7");
Machine *const Machine::S2 = new Machine(ir::BasicType::I32, "s2");
Machine *const Machine::S3 = new Machine(ir::BasicType::I32, "s3");
Machine *const Machine::S4 = new Machine(ir::BasicType::I32, "s4");
Machine *const Machine::S5 = new Machine(ir::BasicType::I32, "s5");
Machine *const Machine::S6 = new Machine(ir::BasicType::I32, "s6");
Machine *const Machine::S7 = new Machine(ir::BasicType::I32, "s7");
Machine *const Machine::S8 = new Machine(ir::BasicType::I32, "s8");
Machine *const Machine::S9 = new Machine(ir::BasicType::I32, "s9");
Machine *const Machine::S10 = new Machine(ir::BasicType::I32, "s10");
Machine *const Machine::S11 = new Machine(ir::BasicType::I32, "s11");
Machine *const Machine::T3 = new Machine(ir::BasicType::I32, "t3");
Machine *const Machine::T4 = new Machine(ir::BasicType::I32, "t4");
Machine *const Machine::T5 = new Machine(ir::BasicType::I32, "t5");
Machine *const Machine::T6 = new Machine(ir::BasicType::I32, "t6");
Machine *const Machine::FA0 = new Machine(ir::BasicType::FLOAT, "fa0");
Machine *const Machine::FA1 = new Machine(ir::BasicType::FLOAT, "fa1");
Machine *const Machine::FA2 = new Machine(ir::BasicType::FLOAT, "fa2");
Machine *const Machine::FA3 = new Machine(ir::BasicType::FLOAT, "fa3");
Machine *const Machine::FA4 = new Machine(ir::BasicType::FLOAT, "fa4");
Machine *const Machine::FA5 = new Machine(ir::BasicType::FLOAT, "fa5");
Machine *const Machine::FA6 = new Machine(ir::BasicType::FLOAT, "fa6");
Machine *const Machine::FA7 = new Machine(ir::BasicType::FLOAT, "fa7");
Machine *const Machine::FS0 = new Machine(ir::BasicType::FLOAT, "fs0");
Machine *const Machine::FS1 = new Machine(ir::BasicType::FLOAT, "fs1");
Machine *const Machine::FS2 = new Machine(ir::BasicType::FLOAT, "fs2");
Machine *const Machine::FS3 = new Machine(ir::BasicType::FLOAT, "fs3");
Machine *const Machine::FS4 = new Machine(ir::BasicType::FLOAT, "fs4");
Machine *const Machine::FS5 = new Machine(ir::BasicType::FLOAT, "fs5");
Machine *const Machine::FS6 = new Machine(ir::BasicType::FLOAT, "fs6");
Machine *const Machine::FS7 = new Machine(ir::BasicType::FLOAT, "fs7");
Machine *const Machine::FS8 = new Machine(ir::BasicType::FLOAT, "fs8");
Machine *const Machine::FS9 = new Machine(ir::BasicType::FLOAT, "fs9");
Machine *const Machine::FS10 = new Machine(ir::BasicType::FLOAT, "fs10");
Machine *const Machine::FS11 = new Machine(ir::BasicType::FLOAT, "fs11");
Machine *const Machine::FT0 = new Machine(ir::BasicType::FLOAT, "ft0");
Machine *const Machine::FT1 = new Machine(ir::BasicType::FLOAT, "ft1");
Machine *const Machine::FT2 = new Machine(ir::BasicType::FLOAT, "ft2");
Machine *const Machine::FT3 = new Machine(ir::BasicType::FLOAT, "ft3");
Machine *const Machine::FT4 = new Machine(ir::BasicType::FLOAT, "ft4");
Machine *const Machine::FT5 = new Machine(ir::BasicType::FLOAT, "ft5");
Machine *const Machine::FT6 = new Machine(ir::BasicType::FLOAT, "ft6");
Machine *const Machine::FT7 = new Machine(ir::BasicType::FLOAT, "ft7");
Machine *const Machine::FT8 = new Machine(ir::BasicType::FLOAT, "ft8");
Machine *const Machine::FT9 = new Machine(ir::BasicType::FLOAT, "ft9");
Machine *const Machine::FT10 = new Machine(ir::BasicType::FLOAT, "ft10");
Machine *const Machine::FT11 = new Machine(ir::BasicType::FLOAT, "ft11");

const std::vector<Machine *> Machine::I_REGS = {
    Machine::A0, Machine::A1, Machine::A2, Machine::A3, Machine::A4,  Machine::A5, Machine::A6,
    Machine::A7, Machine::S0, Machine::S1, Machine::S2, Machine::S3,  Machine::S4, Machine::S5,
    Machine::S6, Machine::S7, Machine::S8, Machine::S9, Machine::S10, Machine::S11};
const std::vector<Machine *> Machine::F_REGS = {
    Machine::FA0, Machine::FA1, Machine::FA2, Machine::FA3,  Machine::FA4,
    Machine::FA5, Machine::FA6, Machine::FA7, Machine::FS0,  Machine::FS1,
    Machine::FS2, Machine::FS3, Machine::FS4, Machine::FS5,  Machine::FS6,
    Machine::FS7, Machine::FS8, Machine::FS9, Machine::FS10, Machine::FS11};
const std::vector<Machine *> Machine::I_CALLER_REGS = {Machine::A0, Machine::A1, Machine::A2,
                                                 Machine::A3, Machine::A4, Machine::A5,
                                                 Machine::A6, Machine::A7};
const std::vector<Machine *> Machine::F_CALLER_REGS = {
    Machine::FA0, Machine::FA1, Machine::FA2, Machine::FA3,
    Machine::FA4, Machine::FA5, Machine::FA6, Machine::FA7};
const std::vector<Machine *> Machine::I_CALLEE_REGS = {
    Machine::S0, Machine::S1, Machine::S2, Machine::S3, Machine::S4,  Machine::S5,
    Machine::S6, Machine::S7, Machine::S8, Machine::S9, Machine::S10, Machine::S11};
const std::vector<Machine *> Machine::F_CALLEE_REGS = {
    Machine::FS0, Machine::FS1, Machine::FS2, Machine::FS3, Machine::FS4,  Machine::FS5,
    Machine::FS6, Machine::FS7, Machine::FS8, Machine::FS9, Machine::FS10, Machine::FS11};

} // namespace reg