# Experimental LLVM IR to ZeroLox emitter

This is a prototype LLVM 19 IR emitter for a small subset of ZLX-1. It emits
assembly directly from an LLVM function pass. The driver provides an entry
point for experiments without modifying LLVM.

Build with LLVM 19 development headers and llvm-config available:

```sh
c++ -std=c++17 driver.cpp $(llvm-config-19 --cxxflags --ldflags --system-libs --libs core irreader support) -o zlx-emit
./zlx-emit input.ll > output.zlx
```

The emitter handles a limited selection of integer arithmetic, byte memory
operations, equality comparisons, constant-offset addressing and branches.
It uses ac for adjacent single-use values and scalar return values, and reuses
some temporary registers after their last local use. It does not implement
spilling or a complete ABI, and unsupported IR can terminate the emitter.
The target registration is experimental. Standalone llc loading previously
crashed, so use the driver above.

The chibicc density experiment used a separate RV32I assembly lowering and
hand-optimized ZLX assembly. Its results are not output from this emitter.
