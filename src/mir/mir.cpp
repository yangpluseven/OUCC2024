#include "mir.hpp"

namespace mir {

int MIR::_counter = 0;

std::vector<MIR *> AddRegLocalMIR::spill(reg::Reg *reg, int offset) {
  if (reg == dest) {
    std::vector<MIR *> irs;
    auto target = new reg::VReg(reg->getType());
    irs.push_back(new AddRegLocalMIR(target, imm));
    irs.push_back(new StoreItemMIR(StoreItemMIR::SPILL, target, offset));
    return irs;
  }
  throw std::runtime_error("in AddRegLocalMIR::spill");
}

std::vector<MIR *> BMIR::spill(reg::Reg *reg, int offset) {
  if (src1 == nullptr || src2 == nullptr) {
    return {this};
  }
  if (src1 == reg && src2 == reg) {
    auto src1 = new reg::VReg(reg->getType());
    auto src2 = new reg::VReg(reg->getType());
    MIR *mir1 = new LoadItemMIR(LoadItemMIR::SPILL, src1, offset);
    MIR *mir2 = new LoadItemMIR(LoadItemMIR::SPILL, src2, offset);
    MIR *mir3 = new BMIR(op, src1, src2, block);
    return {mir1, mir2, mir3};
  }
  if (src1 == reg) {
    auto src1 = new reg::VReg(reg->getType());
    MIR *mir1 = new LoadItemMIR(LoadItemMIR::SPILL, src1, offset);
    MIR *mir2 = new BMIR(op, src1, src2, block);
    return {mir1, mir2};
  }
  if (src2 == reg) {
    auto src2 = new reg::VReg(reg->getType());
    MIR *mir1 = new LoadItemMIR(LoadItemMIR::SPILL, src2, offset);
    MIR *mir2 = new BMIR(op, src1, src2, block);
    return {mir1, mir2};
  }
  return {this};
}

std::vector<MIR *> LiMIR::spill(reg::Reg *reg, int offset) {
  if (reg == dest) {
    auto *target = new reg::VReg(reg->getType());
    MIR *mir1 = new LiMIR(target, imm);
    MIR *mir2 = new StoreItemMIR(StoreItemMIR::SPILL, target, offset);
    return {mir1, mir2};
  }
  return {};
}

std::vector<MIR *> LlaMIR::spill(reg::Reg *reg, int offset) {
  if (dest == reg) {
    auto *target = new reg::VReg(reg->getType());
    MIR *mir1 = new LlaMIR(target, global);
    MIR *mir2 = new StoreItemMIR(StoreItemMIR::SPILL, target, offset);
    return {mir1, mir2};
  }
  return {this};
}

std::vector<MIR *> LoadItemMIR::spill(reg::Reg *reg, int offset) {
  if (dest == reg) {
    auto target = new reg::VReg(reg->getType());
    MIR *mir1 = new LoadItemMIR(item, target, imm);
    MIR *mir2 = new StoreItemMIR(StoreItemMIR::SPILL, target, offset);
    return {mir1, mir2};
  }
  return {this};
}

std::vector<MIR *> StoreItemMIR::spill(reg::Reg *reg, int offset) {
  if (src == reg) {
    auto *vsrc = new reg::VReg(src->getType());
    MIR *mir1 = new LoadItemMIR(LoadItemMIR::SPILL, vsrc, offset);
    MIR *mir2 = new StoreItemMIR(item, vsrc, imm);
    return {mir1, mir2};
  }
  return {this};
}

std::vector<MIR *> LoadMIR::spill(reg::Reg *reg, int offset) {
  if (dest == reg && src == reg) {
    auto vsrc = new reg::VReg(reg->getType());
    auto target = new reg::VReg(reg->getType());
    MIR *mir1 = new LoadItemMIR(LoadItemMIR::SPILL, vsrc, offset);
    MIR *mir2 = new LoadMIR(target, vsrc, imm, size);
    MIR *mir3 = new StoreItemMIR(StoreItemMIR::SPILL, target, offset);
    return {mir1, mir2, mir3};
  }
  if (dest == reg) {
    auto target = new reg::VReg(reg->getType());
    MIR *mir1 = new LoadMIR(target, src, imm, size);
    MIR *mir2 = new StoreItemMIR(StoreItemMIR::SPILL, target, offset);
    return {mir1, mir2};
  }
  if (src == reg) {
    auto vsrc = new reg::VReg(reg->getType());
    MIR *mir1 = new LoadItemMIR(LoadItemMIR::SPILL, vsrc, offset);
    MIR *mir2 = new LoadMIR(dest, vsrc, imm, size);
    return {mir1, mir2};
  }
  return {this};
}

std::string LoadMIR::toString() const {
  std::string loadStr;
  auto type = dest->getType();
  if (type == ir::BasicType::FLOAT) {
    loadStr = "flw";
  } else if (type == ir::BasicType::I32) {
    if (size == 4) {
      loadStr = "lw";
    } else if (size == 8) {
      loadStr = "ld";
    } else {
      throw std::runtime_error("invalid size in LoadMIR::toString");
    }
  } else {
    throw std::runtime_error("invalid type in LoadMIR::toString");
  }
  return loadStr + "\t" + dest->toString() + ", " + std::to_string(imm) + "(" +
         src->toString() + ")";
}

std::vector<MIR *> RrMIR::spill(reg::Reg *reg, int offset) {
  if (dest == reg && src == reg) {
    auto vsrc = new reg::VReg(reg->getType());
    auto target = new reg::VReg(reg->getType());
    MIR *mir1 = new LoadItemMIR(LoadItemMIR::SPILL, vsrc, offset);
    MIR *mir2 = new RrMIR(op, target, vsrc);
    MIR *mir3 = new StoreItemMIR(StoreItemMIR::SPILL, target, offset);
    return {mir1, mir2, mir3};
  }
  if (dest == reg) {
    auto target = new reg::VReg(reg->getType());
    MIR *mir1 = new RrMIR(op, target, src);
    MIR *mir2 = new StoreItemMIR(StoreItemMIR::SPILL, target, offset);
    return {mir1, mir2};
  }
  if (src == reg) {
    auto vsrc = new reg::VReg(reg->getType());
    MIR *mir1 = new LoadItemMIR(LoadItemMIR::SPILL, vsrc, offset);
    MIR *mir2 = new RrMIR(op, dest, vsrc);
    return {mir1, mir2};
  }
  return {this};
}

std::string RrMIR::toString() const {
  auto destType = dest->getType();
  auto srcType = src->getType();
  switch (op) {
  case CVT:
    if (destType == ir::BasicType::FLOAT) {
      if (srcType != ir::BasicType::I32) {
        throw std::runtime_error("invalid type in RrMIR::toString");
      }
      return "fcvt.s.w\t" + dest->toString() + ", " + src->toString();
    }
    if (destType == ir::BasicType::I32) {
      if (srcType != ir::BasicType::FLOAT) {
        throw std::runtime_error("invalid type in RrMIR::toString");
      }
      return "fcvt.w.s\t" + dest->toString() + ", " + src->toString() + ", rtz";
    }
    throw std::runtime_error("invalid type in RrMIR::toString");
  case FABS:
    return "fabs.s\t" + dest->toString() + ", " + src->toString();
  case NEG:
    if (destType == ir::BasicType::FLOAT) {
      if (srcType != ir::BasicType::FLOAT) {
        throw std::runtime_error("invalid type in RrMIR::toString");
      }
      return "fneg.s\t" + dest->toString() + ", " + src->toString();
    }
    if (destType == ir::BasicType::I32) {
      if (srcType != ir::BasicType::I32) {
        throw std::runtime_error("invalid type in RrMIR::toString");
      }
      return "negw\t" + dest->toString() + ", " + src->toString();
    }
    throw std::runtime_error("invalid type in RrMIR::toString");
  case MV:
    if (destType == ir::BasicType::FLOAT) {
      if (srcType == ir::BasicType::FLOAT) {
        return "fmv.s\t" + dest->toString() + ", " + src->toString();
      }
      if (srcType == ir::BasicType::I32) {
        return "fmv.w.x\t" + dest->toString() + ", " + src->toString();
      }
      throw std::runtime_error("invalid type in RrMIR::toString");
    }
    if (destType == ir::BasicType::I32) {
      if (srcType == ir::BasicType::FLOAT) {
        return "fmv.x.w\t" + dest->toString() + ", " + src->toString();
      }
      if (srcType == ir::BasicType::I32) {
        return "mv\t" + dest->toString() + ", " + src->toString();
      }
      throw std::runtime_error("invalid type in RrMIR::toString");
    }
    throw std::runtime_error("invalid type in RrMIR::toString");
  case SEQZ:
  case SNEZ:
    return _opToString(op) + "\t" + dest->toString() + ", " + src->toString();
  default:
    throw std::runtime_error("invalid op in RrMIR::toString");
  }
}

std::vector<MIR *> RriMIR::spill(reg::Reg *reg, int offset) {
  if (dest == reg && src == reg) {
    auto vsrc = new reg::VReg(reg->getType());
    auto target = new reg::VReg(reg->getType());
    MIR *mir1 = new LoadItemMIR(LoadItemMIR::SPILL, vsrc, offset);
    MIR *mir2 = new RriMIR(op, target, vsrc, imm);
    MIR *mir3 = new StoreItemMIR(StoreItemMIR::SPILL, target, offset);
    return {mir1, mir2, mir3};
  }
  if (dest == reg) {
    auto target = new reg::VReg(reg->getType());
    MIR *mir1 = new RriMIR(op, target, src, imm);
    MIR *mir2 = new StoreItemMIR(StoreItemMIR::SPILL, target, offset);
    return {mir1, mir2};
  }
  if (src == reg) {
    auto vsrc = new reg::VReg(reg->getType());
    MIR *mir1 = new LoadItemMIR(LoadItemMIR::SPILL, vsrc, offset);
    MIR *mir2 = new RriMIR(op, dest, vsrc, imm);
    return {mir1, mir2};
  }
  return {this};
}

std::vector<MIR *> RrrMIR::spill(reg::Reg *reg, int offset) {
  if (dest == reg && src1 == reg && src2 == reg) {
    auto target = new reg::VReg(reg->getType());
    auto vsrc1 = new reg::VReg(reg->getType());
    auto vsrc2 = new reg::VReg(reg->getType());
    MIR *mir1 = new LoadItemMIR(LoadItemMIR::SPILL, vsrc1, offset);
    // TODO check here
    MIR *mir2 = new LoadItemMIR(LoadItemMIR::SPILL, vsrc1, offset);
    MIR *mir3 = new RrrMIR(op, target, vsrc1, vsrc2);
    MIR *mir4 = new StoreItemMIR(StoreItemMIR::SPILL, target, offset);
    return {mir1, mir2, mir3, mir4};
  }
  if (dest == reg && src1 == reg) {
    auto target = new reg::VReg(reg->getType());
    auto vsrc1 = new reg::VReg(reg->getType());
    MIR *mir1 = new LoadItemMIR(LoadItemMIR::SPILL, vsrc1, offset);
    MIR *mir2 = new RrrMIR(op, target, vsrc1, src2);
    MIR *mir3 = new StoreItemMIR(StoreItemMIR::SPILL, target, offset);
    return {mir1, mir2, mir3};
  }
  if (dest == reg && src2 == reg) {
    auto target = new reg::VReg(reg->getType());
    auto vsrc2 = new reg::VReg(reg->getType());
    MIR *mir1 = new LoadItemMIR(LoadItemMIR::SPILL, vsrc2, offset);
    MIR *mir2 = new RrrMIR(op, target, src1, vsrc2);
    MIR *mir3 = new StoreItemMIR(StoreItemMIR::SPILL, target, offset);
    return {mir1, mir2, mir3};
  }
  if (src1 == reg && src2 == reg) {
    auto vsrc1 = new reg::VReg(reg->getType());
    auto vsrc2 = new reg::VReg(reg->getType());
    MIR *mir1 = new LoadItemMIR(LoadItemMIR::SPILL, vsrc1, offset);
    MIR *mir2 = new LoadItemMIR(LoadItemMIR::SPILL, vsrc2, offset);
    MIR *mir3 = new RrrMIR(op, dest, vsrc1, vsrc2);
    return {mir1, mir2, mir3};
  }
  if (dest == reg) {
    auto target = new reg::VReg(reg->getType());
    MIR *mir1 = new RrrMIR(op, target, src1, src2);
    MIR *mir2 = new StoreItemMIR(StoreItemMIR::SPILL, target, offset);
    return {mir1, mir2};
  }
  if (src1 == reg) {
    auto vsrc1 = new reg::VReg(reg->getType());
    MIR *mir1 = new LoadItemMIR(LoadItemMIR::SPILL, vsrc1, offset);
    MIR *mir2 = new RrrMIR(op, dest, vsrc1, src2);
    return {mir1, mir2};
  }
  if (src2 == reg) {
    auto vsrc2 = new reg::VReg(reg->getType());
    MIR *mir1 = new LoadItemMIR(LoadItemMIR::SPILL, vsrc2, offset);
    MIR *mir2 = new RrrMIR(op, dest, src1, vsrc2);
    return {mir1, mir2};
  }
  return {this};
}

std::string RrrMIR::toString() const {
  std::string instStr;
  auto destType = dest->getType();
  if (destType == ir::BasicType::FLOAT) {
    switch (op) {
    case ADD:
    case SUB:
    case MUL:
    case DIV:
      instStr = "f" + _opToString(op) + ".s";
      break;
    default:
      throw std::runtime_error("invalid op in RrrMIR::toString");
    }
  } else if (destType == ir::BasicType::I32) {
    switch (op) {
    case ADD:
    case ADDW:
    case SUB:
    case SUBW:
    case MUL:
    case MULW:
    case DIV:
    case DIVW:
    case REMW:
    case XOR:
    case AND:
    case SLT:
    case SGT:
      instStr = _opToString(op);
      break;
    case EQ:
    case GE:
    case GT:
    case LE:
    case LT:
      instStr = "f" + _opToString(op) + ".s";
      break;
    default:
      throw std::runtime_error("invalid op in RrrMIR::toString");
    }
  } else {
    throw std::runtime_error("invalid type in RrrMIR::toString");
  }
  return instStr + "\t" + dest->toString() + ", " + src1->toString() + ", " +
         src2->toString();
}

std::vector<MIR *> StoreMIR::spill(reg::Reg *reg, int offset) {
  if (src == reg && dest == reg) {
    auto vsrc = new reg::VReg(reg->getType());
    auto target = new reg::VReg(reg->getType());
    MIR *mir1 = new LoadItemMIR(LoadItemMIR::SPILL, vsrc, offset);
    MIR *mir2 = new LoadItemMIR(LoadItemMIR::SPILL, target, offset);
    MIR *mir3 = new StoreMIR(vsrc, target, imm, size);
    return {mir1, mir2, mir3};
  }
  if (src == reg) {
    auto vsrc = new reg::VReg(reg->getType());
    MIR *mir1 = new LoadItemMIR(LoadItemMIR::SPILL, vsrc, offset);
    MIR *mir2 = new StoreMIR(vsrc, dest, imm, size);
    return {mir1, mir2};
  }
  if (dest == reg) {
    auto target = new reg::VReg(reg->getType());
    MIR *mir1 = new LoadItemMIR(LoadItemMIR::SPILL, target, offset);
    MIR *mir2 = new StoreMIR(src, target, imm, size);
    return {mir1, mir2};
  }
  return {this};
}

std::string StoreMIR::toString() const {
  std::string instStr;
  auto srcType = src->getType();
  if (srcType == ir::BasicType::FLOAT) {
    instStr = "fsw";
  } else if (srcType == ir::BasicType::I32) {
    if (size == 4) {
      instStr = "sw";
    } else if (size == 8) {
      instStr = "sd";
    } else {
      throw std::runtime_error("invalid size in StoreMIR::toString");
    }
  } else {
    throw std::runtime_error("invalid type in StoreMIR::toString");
  }
  return instStr + "\t" + src->toString() + ", " + std::to_string(imm) + "(" +
         dest->toString() + ")";
}

} // namespace mir