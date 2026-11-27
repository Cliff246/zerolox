# Experimental LLVM IR to ZeroLox emitter

This is a prototype LLVM 19 IR emitter for a small subset of ZLX-1. It emits
assembly directly from LLVM IR through a standalone driver.

Build with LLVM 19 development headers and llvm-config available:

```sh
c++ -std=c++17 driver.cpp $(llvm-config-19 --cxxflags --ldflags --system-libs --libs core irreader support) -o zlx-emit
./zlx-emit input.ll > output.zlx
```

The emitter handles a limited selection of integer arithmetic, equality
comparisons, constant-offset addressing and branches. It emits 8-, 16-, and
32-bit integer loads and stores, and uses 32-bit loads and stores for pointers
in the default address space. Use little-endian IR with 32-bit pointers and
indices. The driver supplies a ZLX-1 data layout when the input omits one.

Functions take scalar arguments in `a0` through `a7` and return scalar values in
`ac`. Call them with `blink10`, `blink21`, or `jlink` and supply a valid `sp` for
PHI copies. The emitter uses `j rp` for leaf returns and emits `push3` and
`pop3` for groups of PHI copies. Use the current Python assembler to encode
its output. The prototype supports leaf functions in a limited register
budget and reports unsupported IR and spill requirements as errors.

The chibicc density experiment used a separate RV32I assembly lowering and
hand-optimized ZLX assembly. Its results are not output from this emitter.
