# Passes

Everything from the original `compile/llvm/pass` should be moved here.

I don't think we need the interface `Analysis` since there's only one class implementing it.

- dominance_tree_analysis.hpp
- branch_opt_pass.hpp
- const_drop_pass.hpp
- ...