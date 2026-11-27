#!/usr/bin/env python3
"""Measure function code bytes from the same C kernels on three targets."""
import argparse
from pathlib import Path
import re
import subprocess
import sys
from tempfile import TemporaryDirectory


HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parent / 'assembler'))
from assembler import assemble
from image import build_image


FLAGS = ('-Oz', '-ffreestanding', '-fno-builtin', '-fno-unwind-tables',
         '-fno-asynchronous-unwind-tables')
TARGETS = {
    'RV32IC': ('-target', 'riscv32-unknown-elf', '-march=rv32ic', '-mabi=ilp32', '-mno-relax'),
    'Thumb-2': ('-target', 'armv7m-none-eabi', '-mcpu=cortex-m3', '-mthumb'),
}


def command(*args):
    return subprocess.check_output(args, text=True)


def native_sizes(clang, readelf, directory):
    by_target = {}
    for target, flags in TARGETS.items():
        obj = directory / f'{target}.o'
        command(clang, *flags, *FLAGS, '-c', HERE / 'density_kernels.c', '-o', obj)
        sizes = {}
        for row in command(readelf, '-sW', obj).splitlines():
            fields = row.split()
            if len(fields) == 8 and fields[3] == 'FUNC' and fields[6] != 'UND':
                sizes[fields[7]] = int(fields[2])
        by_target[target] = sizes
    return by_target


def verify(emulator, output, assembly, name, setup, expected, epilogue=()):
    source = '\n'.join(('get16 sp, 0x8000', *setup, f'blink21 #{name}', *epilogue,
                        'done:', 'b #done', assembly))
    output.write_bytes(build_image(assemble(source)))
    registers = {
        key: int(value)
        for key, value in (line.split('=', 1) for line in command(emulator, output, '96').splitlines()
                           if re.fullmatch(r'r\d+=\d+', line))
    }
    expected = {**expected, 'r2': 0x8000}
    actual = {register: registers[register] for register in expected}
    if actual != expected:
        raise RuntimeError(f'{name}: expected {expected}, got {actual}')


def smoke_test(emulator, output, assembly, name):
    memory = ('get16 a0, 8192', 'get16 a1, 12288', 'get t0, 5',
              'st.32 t0, [a0]', 'st.32 t0, [a0, 4]', 'st.32 t0, [a0, 8]', 'st.32 t0, [a0, 12]',
              'get t0, 0xff', 'st.8 t0, [a0, 1]', 'get16 t0, 0x1234', 'st.16 t0, [a1, 4]')
    cases = {
        'add_three': [(('get a0, 3', 'get a1, 7', 'get a2, 13'), 23)],
        'constant_value': [((), 400)],
        'bit_mix': [(('get16 a0, 0x1234', 'get16 a1, 0xffff'), 0xcb)],
        'shift_mix': [(('get a0, 20', 'get16 a1, 2048'), 224)],
        'sum_four': [(memory[:7], 20)],
        'load_mix': [(memory, 0x1333)],
        'signed_byte': [(memory, 0xffffffff)],
        'equal32': [(('get a0, 4', 'get a1, 4'), 1), (('get a0, 9', 'get a1, 4'), 0)],
        'greater32': [(('get a0, -2', 'get a1, 1'), 0), (('get a0, 9', 'get a1, 4'), 1)],
        'choose32': [(('get a0, 0', 'get a1, 9', 'get a2, 4'), 4),
                     (('get a0, 1', 'get a1, 9', 'get a2, 4'), 9)],
    }
    if name == 'copy_four':
        setup = ('get16 a0, 8192', 'get16 a1, 12288', 'get t0, 1', 'st.32 t0, [a1]',
                 'get t0, 2', 'st.32 t0, [a1, 4]', 'get t0, 3', 'st.32 t0, [a1, 8]',
                 'get t0, 4', 'st.32 t0, [a1, 12]')
        epilogue = tuple(f'ld.32 t{index}, [a0, {4 * index}]' for index in range(4))
        expected = {f'r{8 + index}': 1 + index for index in range(4)}
        verify(emulator, output, assembly, name, setup, expected, epilogue)
    else:
        for setup, expected in cases[name]:
            verify(emulator, output, assembly, name, setup, {'r1': expected})


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--emitter', type=Path, required=True)
    parser.add_argument('--baseline', type=Path)
    parser.add_argument('--emulator', type=Path)
    parser.add_argument('--clang', default='clang-19')
    parser.add_argument('--readelf', default='readelf')
    args = parser.parse_args()

    with TemporaryDirectory() as temporary:
        directory = Path(temporary)
        ir = directory / 'kernels.ll'
        command(args.clang, '-target', 'i386-unknown-none', *FLAGS, '-S', '-emit-llvm',
                HERE / 'density_kernels.c', '-o', ir)
        module = ir.read_text()
        definitions = list(re.finditer(r'^define [^\n]*?@([A-Za-z0-9_]+)\([^\n]*\n.*?^}', module, re.M | re.S))
        if not definitions:
            raise RuntimeError('no LLVM IR function definitions found')
        sizes = native_sizes(args.clang, args.readelf, directory)
        columns = ['Kernel'] + (['ZLX before'] if args.baseline else []) + ['ZLX', *sizes]
        rows = []
        for definition in definitions:
            name = definition[1]
            function_ir = directory / f'{name}.ll'
            source_parts = []
            position = 0
            for match in definitions:
                source_parts.append(module[position:match.start()])
                if match is definition:
                    source_parts.append(match[0])
                position = match.end()
            function_ir.write_text(''.join(source_parts) + module[position:])
            assembly = command(args.emitter.resolve(), function_ir)
            code_size = len(build_image(assemble(assembly)))
            if args.emulator:
                smoke_test(args.emulator.resolve(), directory / 'smoke.bin', assembly, name)
            row = [name]
            if args.baseline:
                baseline = subprocess.run([args.baseline.resolve(), function_ir], capture_output=True, text=True)
                row.append(len(build_image(assemble(baseline.stdout))) if baseline.returncode == 0 else None)
            row.extend([code_size, *(sizes[target][name] for target in sizes)])
            rows.append(row)
        print('| ' + ' | '.join(columns) + ' |')
        print('| --- |' + ' ---: |' * (len(columns) - 1))
        for row in rows:
            print('| ' + ' | '.join('—' if value is None else str(value) for value in row) + ' |')
        print('| **Total** | ' + ' | '.join(
            '—' if None in values else str(sum(values)) for values in zip(*(row[1:] for row in rows))) + ' |')


if __name__ == '__main__':
    main()
