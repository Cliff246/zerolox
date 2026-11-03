#!/usr/bin/env python3
"""Experimental zerolox syntax and linker model; not a machine-code encoder."""
import argparse
import json
import sys
from pathlib import Path
from model import Error
from syntax import assemble
from linker import link
from image import build_image

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('source',type=Path); parser.add_argument('--object',type=Path,help='write structured JSON object')
    parser.add_argument('-o', '--output', type=Path, help='write experimental flat binary image')
    args=parser.parse_args()
    try:
        obj=assemble(args.source.read_text(),str(args.source))
        if args.object: args.object.write_text(json.dumps(obj,indent=2)+'\n')
        listing,symbols=link(obj)
        if args.output: args.output.write_bytes(build_image(obj))
        for pc,size,op,line in listing: print(f'{pc:06x}  {size} bytes  {op:12}  line {line}')
        for name,s in symbols.items():
            if s['kind']=='label': print(f"{name}: {s['value']:#x}")
    except (Error,OSError,ValueError) as e: print(e,file=sys.stderr); return 1
    return 0

if __name__=='__main__': sys.exit(main())
