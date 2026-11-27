# CPU test suite

This replaces the initial `zlx3_plan/zlx3.py` scaffold with shared definitions in
`base.py` and the previously tested reference simulation split by responsibility.
When integrating into the repository, remove `experiments/cpus/zlx3_plan/zlx3.py`
after copying this directory's files. No remote repository changes are included.

## Structure

- `base.py`: original 6+5+7+7+7 microinstruction layout, typed operands,
  packet builder, frontend/backend protocols. The 32-bit word is the operation
  encoding; a Packet carries simulator metadata and literal allocations separately.
- `frontends/`: Zerolox R32 binary translation, an RV32I subset, direct packets,
  and frontend publication settings.
- `backends/`: five-stage ordered reference model and backend settings.
- `settings.py`: machine configurations combining a frontend and backend.
- `tests/`: original tests plus independent frontend/backend integration tests.

Run from repository root:

```sh
python -m unittest discover -s experiments/cpus/tests -t . -v
```

Example (RV32I ADDI x8,x0,7):

```python
from experiments.cpus.settings import RV32_DUAL
from experiments.cpus.frontends.settings import FrontendSettings
cpu = RV32_DUAL.build(bytes.fromhex('13047000'),
                     settings=FrontendSettings(packets_per_cycle=2)).run()
print(cpu.registers[8], cpu.cycles, cpu.retired, cpu.uops)
for row in cpu.trace:
    print(row)
```

## Model contract and limits

D -> II -> EX -> MEM -> WB. In-order issue, configurable issue width 1/2,
EX/MEM forwarding, one memory operation per issue batch, group-atomic modeled
architectural writes, no prediction. Both lanes currently have identical
capabilities. Frontend publication rate counts architectural packets, not fetched
bytes; it is not a real fetch/decode timing model. The 8-uop queue excludes the
publisher staging buffer and other sideband state. Actual packet metadata,
allocation hardware, hazard control and pipeline latches require RTL design.

The reference operation set is 32-bit with 32 registers, register zero, 32
immediate slots and 32 latch slots. The original `ParamaterType` spelling and
opcode numbering are preserved. The fixed conventions are intentionally not a
universal representation of every ISA (e.g. flags, arbitrary register counts,
W64, FP, system operations and more addressing forms need extensions).

RV32I subset: integer ALU, immediate ALU, upper-immediate, loads/stores,
conditional branches, JAL/JALR. Not a compliance implementation: no M, C,
CSR, FENCE, system instructions, trap/interrupt delivery or full alignment/fault
behavior. Zerolox binary frontend follows the earlier reference translator;
not exhaustively validated against the C emulator. No real cache, stack unit,
multicycle numerical units, or asymmetric P/C implementation is modeled yet.
Results are only cycle-accurate for these defined reference timing rules.
