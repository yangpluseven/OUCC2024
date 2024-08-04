#include "mir.hpp"

namespace mir {

int MIR::_counter = 0;

std::vector<MIR *> RegAddImm::spill(reg::Reg *reg, int offset) {
  if (reg == dest) {
    std::vector<MIR *> irs;
    auto target = new reg::Virtual(reg->getType());
    irs.push_back(new RegAddImm(target, imm));
    irs.push_back(new StoreTo(StoreTo::SPILL, target, offset));
    return irs;
  }
  throw std::runtime_error("in AddRegLocalMIR::spill");
}

std::vector<MIR *> Branch::spill(reg::Reg *reg, int offset) {
  if (src1 == nullptr || src2 == nullptr) {
    return {this};
  }
  if (src1 == reg && src2 == reg) {
    auto src1 = new reg::Virtual(reg->getType());
    auto src2 = new reg::Virtual(reg->getType());
    MIR *mir1 = new LoadFrom(LoadFrom::SPILL, src1, offset);
    MIR *mir2 = new LoadFrom(LoadFrom::SPILL, src2, offset);
    MIR *mir3 = new Branch(op, src1, src2, block);
    return {mir1, mir2, mir3};
  }
  if (src1 == reg) {
    auto src1 = new reg::Virtual(reg->getType());
    MIR *mir1 = new LoadFrom(LoadFrom::SPILL, src1, offset);
    MIR *mir2 = new Branch(op, src1, src2, block);
    return {mir1, mir2};
  }
  if (src2 == reg) {
    auto src2 = new reg::Virtual(reg->getType());
    MIR *mir1 = new LoadFrom(LoadFrom::SPILL, src2, offset);
    MIR *mir2 = new Branch(op, src1, src2, block);
    return {mir1, mir2};
  }
  return {this};
}

std::vector<MIR *> LoadImm::spill(reg::Reg *reg, int offset) {
  if (reg == dest) {
    auto *target = new reg::Virtual(reg->getType());
    MIR *mir1 = new LoadImm(target, imm);
    MIR *mir2 = new StoreTo(StoreTo::SPILL, target, offset);
    return {mir1, mir2};
  }
  return {};
}

std::vector<MIR *> LLA::spill(reg::Reg *reg, int offset) {
  if (dest == reg) {
    auto *target = new reg::Virtual(reg->getType());
    MIR *mir1 = new LLA(target, global);
    MIR *mir2 = new StoreTo(StoreTo::SPILL, target, offset);
    return {mir1, mir2};
  }
  return {this};
}

std::vector<MIR *> LoadFrom::spill(reg::Reg *reg, int offset) {
  if (dest == reg) {
    auto target = new reg::Virtual(reg->getType());
    MIR *mir1 = new LoadFrom(from, target, imm);
    MIR *mir2 = new StoreTo(StoreTo::SPILL, target, offset);
    return {mir1, mir2};
  }
  return {this};
}

std::vector<MIR *> StoreTo::spill(reg::Reg *reg, int offset) {
  if (src == reg) {
    auto *vsrc = new reg::Virtual(src->getType());
    MIR *mir1 = new LoadFrom(LoadFrom::SPILL, vsrc, offset);
    MIR *mir2 = new StoreTo(to, vsrc, imm);
    return {mir1, mir2};
  }
  return {this};
}

std::vector<MIR *> Load::spill(reg::Reg *reg, int offset) {
  if (dest == reg && src == reg) {
    auto vsrc = new reg::Virtual(reg->getType());
    auto target = new reg::Virtual(reg->getType());
    MIR *mir1 = new LoadFrom(LoadFrom::SPILL, vsrc, offset);
    MIR *mir2 = new Load(target, vsrc, imm, size);
    MIR *mir3 = new StoreTo(StoreTo::SPILL, target, offset);
    return {mir1, mir2, mir3};
  }
  if (dest == reg) {
    auto target = new reg::Virtual(reg->getType());
    MIR *mir1 = new Load(target, src, imm, size);
    MIR *mir2 = new StoreTo(StoreTo::SPILL, target, offset);
    return {mir1, mir2};
  }
  if (src == reg) {
    auto vsrc = new reg::Virtual(reg->getType());
    MIR *mir1 = new LoadFrom(LoadFrom::SPILL, vsrc, offset);
    MIR *mir2 = new Load(dest, vsrc, imm, size);
    return {mir1, mir2};
  }
  return {this};
}

std::string Load::str() const {
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
  return loadStr + "\t" + dest->str() + ", " + std::to_string(imm) + "(" +
         src->str() + ")";
}

std::vector<MIR *> RR::spill(reg::Reg *reg, int offset) {
  if (dest == reg && src == reg) {
    auto vsrc = new reg::Virtual(reg->getType());
    auto target = new reg::Virtual(reg->getType());
    MIR *mir1 = new LoadFrom(LoadFrom::SPILL, vsrc, offset);
    MIR *mir2 = new RR(op, target, vsrc);
    MIR *mir3 = new StoreTo(StoreTo::SPILL, target, offset);
    return {mir1, mir2, mir3};
  }
  if (dest == reg) {
    auto target = new reg::Virtual(reg->getType());
    MIR *mir1 = new RR(op, target, src);
    MIR *mir2 = new StoreTo(StoreTo::SPILL, target, offset);
    return {mir1, mir2};
  }
  if (src == reg) {
    auto vsrc = new reg::Virtual(reg->getType());
    MIR *mir1 = new LoadFrom(LoadFrom::SPILL, vsrc, offset);
    MIR *mir2 = new RR(op, dest, vsrc);
    return {mir1, mir2};
  }
  return {this};
}

std::string RR::str() const {
  auto destType = dest->getType();
  auto srcType = src->getType();
  switch (op) {
  case CVT:
    if (destType == ir::BasicType::FLOAT) {
      if (srcType != ir::BasicType::I32) {
        throw std::runtime_error("invalid type in RrMIR::toString");
      }
      return "fcvt.s.w\t" + dest->str() + ", " + src->str();
    }
    if (destType == ir::BasicType::I32) {
      if (srcType != ir::BasicType::FLOAT) {
        throw std::runtime_error("invalid type in RrMIR::toString");
      }
      return "fcvt.w.s\t" + dest->str() + ", " + src->str() + ", rtz";
    }
    throw std::runtime_error("invalid type in RrMIR::toString");
  case FABS:
    return "fabs.s\t" + dest->str() + ", " + src->str();
  case NEG:
    if (destType == ir::BasicType::FLOAT) {
      if (srcType != ir::BasicType::FLOAT) {
        throw std::runtime_error("invalid type in RrMIR::toString");
      }
      return "fneg.s\t" + dest->str() + ", " + src->str();
    }
    if (destType == ir::BasicType::I32) {
      if (srcType != ir::BasicType::I32) {
        throw std::runtime_error("invalid type in RrMIR::toString");
      }
      return "negw\t" + dest->str() + ", " + src->str();
    }
    throw std::runtime_error("invalid type in RrMIR::toString");
  case MV:
    if (destType == ir::BasicType::FLOAT) {
      if (srcType == ir::BasicType::FLOAT) {
        return "fmv.s\t" + dest->str() + ", " + src->str();
      }
      if (srcType == ir::BasicType::I32) {
        return "fmv.w.x\t" + dest->str() + ", " + src->str();
      }
      throw std::runtime_error("invalid type in RrMIR::toString");
    }
    if (destType == ir::BasicType::I32) {
      if (srcType == ir::BasicType::FLOAT) {
        return "fmv.x.w\t" + dest->str() + ", " + src->str();
      }
      if (srcType == ir::BasicType::I32) {
        return "mv\t" + dest->str() + ", " + src->str();
      }
      throw std::runtime_error("invalid type in RrMIR::toString");
    }
    throw std::runtime_error("invalid type in RrMIR::toString");
  case SEQZ:
  case SNEZ:
    return _opToString(op) + "\t" + dest->str() + ", " + src->str();
  default:
    throw std::runtime_error("invalid op in RrMIR::toString");
  }
}

std::vector<MIR *> RRI::spill(reg::Reg *reg, int offset) {
  if (dest == reg && src == reg) {
    auto vsrc = new reg::Virtual(reg->getType());
    auto target = new reg::Virtual(reg->getType());
    MIR *mir1 = new LoadFrom(LoadFrom::SPILL, vsrc, offset);
    MIR *mir2 = new RRI(op, target, vsrc, imm);
    MIR *mir3 = new StoreTo(StoreTo::SPILL, target, offset);
    return {mir1, mir2, mir3};
  }
  if (dest == reg) {
    auto target = new reg::Virtual(reg->getType());
    MIR *mir1 = new RRI(op, target, src, imm);
    MIR *mir2 = new StoreTo(StoreTo::SPILL, target, offset);
    return {mir1, mir2};
  }
  if (src == reg) {
    auto vsrc = new reg::Virtual(reg->getType());
    MIR *mir1 = new LoadFrom(LoadFrom::SPILL, vsrc, offset);
    MIR *mir2 = new RRI(op, dest, vsrc, imm);
    return {mir1, mir2};
  }
  return {this};
}

std::vector<MIR *> RRR::spill(reg::Reg *reg, int offset) {
  if (dest == reg && src1 == reg && src2 == reg) {
    auto target = new reg::Virtual(reg->getType());
    auto vsrc1 = new reg::Virtual(reg->getType());
    auto vsrc2 = new reg::Virtual(reg->getType());
    MIR *mir1 = new LoadFrom(LoadFrom::SPILL, vsrc1, offset);
    // TODO check here
    MIR *mir2 = new LoadFrom(LoadFrom::SPILL, vsrc1, offset);
    MIR *mir3 = new RRR(op, target, vsrc1, vsrc2);
    MIR *mir4 = new StoreTo(StoreTo::SPILL, target, offset);
    return {mir1, mir2, mir3, mir4};
  }
  if (dest == reg && src1 == reg) {
    auto target = new reg::Virtual(reg->getType());
    auto vsrc1 = new reg::Virtual(reg->getType());
    MIR *mir1 = new LoadFrom(LoadFrom::SPILL, vsrc1, offset);
    MIR *mir2 = new RRR(op, target, vsrc1, src2);
    MIR *mir3 = new StoreTo(StoreTo::SPILL, target, offset);
    return {mir1, mir2, mir3};
  }
  if (dest == reg && src2 == reg) {
    auto target = new reg::Virtual(reg->getType());
    auto vsrc2 = new reg::Virtual(reg->getType());
    MIR *mir1 = new LoadFrom(LoadFrom::SPILL, vsrc2, offset);
    MIR *mir2 = new RRR(op, target, src1, vsrc2);
    MIR *mir3 = new StoreTo(StoreTo::SPILL, target, offset);
    return {mir1, mir2, mir3};
  }
  if (src1 == reg && src2 == reg) {
    auto vsrc1 = new reg::Virtual(reg->getType());
    auto vsrc2 = new reg::Virtual(reg->getType());
    MIR *mir1 = new LoadFrom(LoadFrom::SPILL, vsrc1, offset);
    MIR *mir2 = new LoadFrom(LoadFrom::SPILL, vsrc2, offset);
    MIR *mir3 = new RRR(op, dest, vsrc1, vsrc2);
    return {mir1, mir2, mir3};
  }
  if (dest == reg) {
    auto target = new reg::Virtual(reg->getType());
    MIR *mir1 = new RRR(op, target, src1, src2);
    MIR *mir2 = new StoreTo(StoreTo::SPILL, target, offset);
    return {mir1, mir2};
  }
  if (src1 == reg) {
    auto vsrc1 = new reg::Virtual(reg->getType());
    MIR *mir1 = new LoadFrom(LoadFrom::SPILL, vsrc1, offset);
    MIR *mir2 = new RRR(op, dest, vsrc1, src2);
    return {mir1, mir2};
  }
  if (src2 == reg) {
    auto vsrc2 = new reg::Virtual(reg->getType());
    MIR *mir1 = new LoadFrom(LoadFrom::SPILL, vsrc2, offset);
    MIR *mir2 = new RRR(op, dest, src1, vsrc2);
    return {mir1, mir2};
  }
  return {this};
}

std::string RRR::str() const {
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
  return instStr + "\t" + dest->str() + ", " + src1->str() + ", " +
         src2->str();
}

std::vector<MIR *> Store::spill(reg::Reg *reg, int offset) {
  if (src == reg && dest == reg) {
    auto vsrc = new reg::Virtual(reg->getType());
    auto target = new reg::Virtual(reg->getType());
    MIR *mir1 = new LoadFrom(LoadFrom::SPILL, vsrc, offset);
    MIR *mir2 = new LoadFrom(LoadFrom::SPILL, target, offset);
    MIR *mir3 = new Store(vsrc, target, imm, size);
    return {mir1, mir2, mir3};
  }
  if (src == reg) {
    auto vsrc = new reg::Virtual(reg->getType());
    MIR *mir1 = new LoadFrom(LoadFrom::SPILL, vsrc, offset);
    MIR *mir2 = new Store(vsrc, dest, imm, size);
    return {mir1, mir2};
  }
  if (dest == reg) {
    auto target = new reg::Virtual(reg->getType());
    MIR *mir1 = new LoadFrom(LoadFrom::SPILL, target, offset);
    MIR *mir2 = new Store(src, target, imm, size);
    return {mir1, mir2};
  }
  return {this};
}

std::string Store::str() const {
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
  return instStr + "\t" + src->str() + ", " + std::to_string(imm) + "(" +
         dest->str() + ")";
}

} // namespace mir