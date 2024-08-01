#include "register.hpp"

namespace ir {

BasicType *const BasicType::I1 = new BasicType(1, "i1");
BasicType *const BasicType::I32 = new BasicType(32, "i32");
BasicType *const BasicType::FLOAT = new BasicType(32, "float");
BasicType *const BasicType::VOID = new BasicType(0, "void");

} // namespace ir

namespace reg {

int VReg::_counter = 0;

MReg *const MReg::ZERO = new MReg(ir::BasicType::I32, "zero");
MReg *const MReg::RA = new MReg(ir::BasicType::I32, "ra");
MReg *const MReg::SP = new MReg(ir::BasicType::I32, "sp");
MReg *const MReg::GP = new MReg(ir::BasicType::I32, "gp");
MReg *const MReg::TP = new MReg(ir::BasicType::I32, "tp");
MReg *const MReg::T0 = new MReg(ir::BasicType::I32, "t0");
MReg *const MReg::T1 = new MReg(ir::BasicType::I32, "t1");
MReg *const MReg::T2 = new MReg(ir::BasicType::I32, "t2");
MReg *const MReg::S0 = new MReg(ir::BasicType::I32, "s0");
MReg *const MReg::S1 = new MReg(ir::BasicType::I32, "s1");
MReg *const MReg::A0 = new MReg(ir::BasicType::I32, "a0");
MReg *const MReg::A1 = new MReg(ir::BasicType::I32, "a1");
MReg *const MReg::A2 = new MReg(ir::BasicType::I32, "a2");
MReg *const MReg::A3 = new MReg(ir::BasicType::I32, "a3");
MReg *const MReg::A4 = new MReg(ir::BasicType::I32, "a4");
MReg *const MReg::A5 = new MReg(ir::BasicType::I32, "a5");
MReg *const MReg::A6 = new MReg(ir::BasicType::I32, "a6");
MReg *const MReg::A7 = new MReg(ir::BasicType::I32, "a7");
MReg *const MReg::S2 = new MReg(ir::BasicType::I32, "s2");
MReg *const MReg::S3 = new MReg(ir::BasicType::I32, "s3");
MReg *const MReg::S4 = new MReg(ir::BasicType::I32, "s4");
MReg *const MReg::S5 = new MReg(ir::BasicType::I32, "s5");
MReg *const MReg::S6 = new MReg(ir::BasicType::I32, "s6");
MReg *const MReg::S7 = new MReg(ir::BasicType::I32, "s7");
MReg *const MReg::S8 = new MReg(ir::BasicType::I32, "s8");
MReg *const MReg::S9 = new MReg(ir::BasicType::I32, "s9");
MReg *const MReg::S10 = new MReg(ir::BasicType::I32, "s10");
MReg *const MReg::S11 = new MReg(ir::BasicType::I32, "s11");
MReg *const MReg::T3 = new MReg(ir::BasicType::I32, "t3");
MReg *const MReg::T4 = new MReg(ir::BasicType::I32, "t4");
MReg *const MReg::T5 = new MReg(ir::BasicType::I32, "t5");
MReg *const MReg::T6 = new MReg(ir::BasicType::I32, "t6");
MReg *const MReg::FA0 = new MReg(ir::BasicType::FLOAT, "fa0");
MReg *const MReg::FA1 = new MReg(ir::BasicType::FLOAT, "fa1");
MReg *const MReg::FA2 = new MReg(ir::BasicType::FLOAT, "fa2");
MReg *const MReg::FA3 = new MReg(ir::BasicType::FLOAT, "fa3");
MReg *const MReg::FA4 = new MReg(ir::BasicType::FLOAT, "fa4");
MReg *const MReg::FA5 = new MReg(ir::BasicType::FLOAT, "fa5");
MReg *const MReg::FA6 = new MReg(ir::BasicType::FLOAT, "fa6");
MReg *const MReg::FA7 = new MReg(ir::BasicType::FLOAT, "fa7");
MReg *const MReg::FS0 = new MReg(ir::BasicType::FLOAT, "fs0");
MReg *const MReg::FS1 = new MReg(ir::BasicType::FLOAT, "fs1");
MReg *const MReg::FS2 = new MReg(ir::BasicType::FLOAT, "fs2");
MReg *const MReg::FS3 = new MReg(ir::BasicType::FLOAT, "fs3");
MReg *const MReg::FS4 = new MReg(ir::BasicType::FLOAT, "fs4");
MReg *const MReg::FS5 = new MReg(ir::BasicType::FLOAT, "fs5");
MReg *const MReg::FS6 = new MReg(ir::BasicType::FLOAT, "fs6");
MReg *const MReg::FS7 = new MReg(ir::BasicType::FLOAT, "fs7");
MReg *const MReg::FS8 = new MReg(ir::BasicType::FLOAT, "fs8");
MReg *const MReg::FS9 = new MReg(ir::BasicType::FLOAT, "fs9");
MReg *const MReg::FS10 = new MReg(ir::BasicType::FLOAT, "fs10");
MReg *const MReg::FS11 = new MReg(ir::BasicType::FLOAT, "fs11");
MReg *const MReg::FT0 = new MReg(ir::BasicType::FLOAT, "ft0");
MReg *const MReg::FT1 = new MReg(ir::BasicType::FLOAT, "ft1");
MReg *const MReg::FT2 = new MReg(ir::BasicType::FLOAT, "ft2");
MReg *const MReg::FT3 = new MReg(ir::BasicType::FLOAT, "ft3");
MReg *const MReg::FT4 = new MReg(ir::BasicType::FLOAT, "ft4");
MReg *const MReg::FT5 = new MReg(ir::BasicType::FLOAT, "ft5");
MReg *const MReg::FT6 = new MReg(ir::BasicType::FLOAT, "ft6");
MReg *const MReg::FT7 = new MReg(ir::BasicType::FLOAT, "ft7");
MReg *const MReg::FT8 = new MReg(ir::BasicType::FLOAT, "ft8");
MReg *const MReg::FT9 = new MReg(ir::BasicType::FLOAT, "ft9");
MReg *const MReg::FT10 = new MReg(ir::BasicType::FLOAT, "ft10");
MReg *const MReg::FT11 = new MReg(ir::BasicType::FLOAT, "ft11");

const std::vector<MReg *> MReg::I_REGS = {
    MReg::A0, MReg::A1, MReg::A2, MReg::A3, MReg::A4,  MReg::A5, MReg::A6,
    MReg::A7, MReg::S0, MReg::S1, MReg::S2, MReg::S3,  MReg::S4, MReg::S5,
    MReg::S6, MReg::S7, MReg::S8, MReg::S9, MReg::S10, MReg::S11};
const std::vector<MReg *> MReg::F_REGS = {
    MReg::FA0, MReg::FA1, MReg::FA2, MReg::FA3,  MReg::FA4,
    MReg::FA5, MReg::FA6, MReg::FA7, MReg::FS0,  MReg::FS1,
    MReg::FS2, MReg::FS3, MReg::FS4, MReg::FS5,  MReg::FS6,
    MReg::FS7, MReg::FS8, MReg::FS9, MReg::FS10, MReg::FS11};
const std::vector<MReg *> MReg::I_CALLER_REGS = {MReg::A0, MReg::A1, MReg::A2,
                                                 MReg::A3, MReg::A4, MReg::A5,
                                                 MReg::A6, MReg::A7};
const std::vector<MReg *> MReg::F_CALLER_REGS = {
    MReg::FA0, MReg::FA1, MReg::FA2, MReg::FA3,
    MReg::FA4, MReg::FA5, MReg::FA6, MReg::FA7};
const std::vector<MReg *> MReg::I_CALLEE_REGS = {
    MReg::S0, MReg::S1, MReg::S2, MReg::S3, MReg::S4,  MReg::S5,
    MReg::S6, MReg::S7, MReg::S8, MReg::S9, MReg::S10, MReg::S11};
const std::vector<MReg *> MReg::F_CALLEE_REGS = {
    MReg::FS0, MReg::FS1, MReg::FS2, MReg::FS3, MReg::FS4,  MReg::FS5,
    MReg::FS6, MReg::FS7, MReg::FS8, MReg::FS9, MReg::FS10, MReg::FS11};

} // namespace reg