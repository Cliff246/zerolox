"""Experimental flat-image encoding, matching the current C decoder."""
import ast
import extra
from linker import link
from dialect import lower
from model import Error, CURRENT_ISA, legacy_encoding, mnemonic
from syntax import split_args
from values import expr, reg, data_size

from opcodes import MAJOR, MINOR, LEGACY_MAJOR, LEGACY_MINOR, LEGACY_REG_SELECTOR

WIDTH = {'8':0, '16':1, '32':2}

def parcel(value):
    return value.to_bytes(2,'little')

def encode(rec, chosen, symbols, pc, target=CURRENT_ISA):
    op,args=lower(mnemonic(rec),rec['args'],symbols,pc,target)
    base=chosen.rsplit('.',1)[0]
    minor_name = 'move' if chosen == 'm.mv' else chosen.removeprefix('m.')
    major, minor = (MAJOR, MINOR) if not legacy_encoding(target) else (LEGACY_MAJOR, LEGACY_MINOR)
    if op in extra.names(target):
        form,fields=extra.parse(op,args,symbols,pc,target)
        return extra.encode(form,fields)
    if chosen in ('m.mv','m.add','m.adda','m.sub'):
        if op in ('add','n.add','sub','n.sub'):
            d,a,b=map(reg,args)
            a,b=(a,b) if chosen in ('m.add','m.sub') else (d,b)
        else: a,b=map(reg,args)
        return parcel((minor[minor_name]<<11)|(a<<6)|(b<<1)|1)
    if chosen in ('m.geta','m.b','m.baeqz','m.baneqz'):
        v=expr(args[0],symbols,pc)
        if chosen!='m.geta': v//=2
        return parcel((minor[minor_name]<<11)|((v & 1023)<<1)|1)
    if chosen=='m.get8':
        return parcel((minor[minor_name]<<11)|((reg(args[0])-8)<<9)|(expr(args[1],symbols,pc)<<1)|1)
    if chosen.startswith('n.') and chosen[2:] in extra.REG_ALU:
        d,a,b=map(reg,args)
        if legacy_encoding(target):
            selector = LEGACY_REG_SELECTOR[chosen[2:]]
            return parcel((major['reg_group']<<6)|(a<<1))+parcel((selector<<11)|(d<<6)|(b<<1))
        direction, move_reg = rec.get('move', ('amv', 'z0'))
        first = (major[chosen[2:]] << 6) | (a << 1)
        second = (d << 11) | (b << 6) | (reg(move_reg) << 1) | (direction == 'mva')
        return parcel(first) + parcel(second)
    if base in ('m.lda','m.ald','m.sta','m.ast','n.ld','n.st'):
        fields=split_args(args[-1][1:-1]); address=reg(fields[0]); offset=expr(fields[1],symbols,pc) if len(fields)==2 else 0
        width=chosen.rsplit('.',1)[1]; size={'8':1,'16':2,'32':4}[width]
        value_reg=1 if op.startswith(('m.lda.','m.sta.')) else reg(args[0])
        if base.startswith('m.'):
            operand=address if base in ('m.lda','m.sta') else value_reg
            return parcel((minor[base[2:]]<<11)|(WIDTH[width]<<9)|((offset//size)<<6)|(operand<<1)|1)
        raw=offset & 0x1ff
        first=(major[base[2:]]<<6)|(address<<1)
        second=((raw&31)<<11)|(WIDTH[width]<<9)|(((raw>>5)&7)<<6)|(value_reg<<1)|((raw>>8)&1)
        return parcel(first)+parcel(second)
    raise Error(f'no image encoder for {chosen}')

def encode_data(args,width,symbols,pc):
    out=bytearray()
    def add(arg):
        if arg.startswith('{') and arg.endswith('}'):
            for x in split_args(arg[1:-1]): add(x)
        elif arg.startswith(('"',"'")):
            value=ast.literal_eval(arg)
            if arg[0]=='"': out.extend(value.encode('latin-1')+b'\0')
            else: out.extend(ord(value).to_bytes(width,'little'))
        else: out.extend(expr(arg,symbols,pc).to_bytes(width,'little'))
    for arg in args: add(arg)
    return bytes(out)

def build_image(obj):
    listing,symbols=link(obj)
    selected=iter(listing); out=bytearray()
    for rec in obj['records']:
        pc=len(out); kind=rec['kind']
        if kind=='instruction':
            position,size,chosen,_=next(selected)
            if position!=pc: raise Error('internal layout mismatch')
            code=encode(rec,chosen,symbols,pc,obj['target'])
            if len(code)!=size: raise Error('internal instruction size mismatch')
            out.extend(code)
        elif kind=='data':
            data_size(rec['args'],rec['width'],symbols,pc)
            out.extend(encode_data(rec['args'],rec['width'],symbols,pc))
        elif kind=='align':
            alignment=expr(rec['value'],symbols,pc)
            out.extend(bytes((-pc)%alignment))
        elif kind=='jumpto':
            target=expr(rec['value'],symbols,pc)
            if target<pc: raise Error('@jumpto cannot move backwards')
            out.extend(bytes(target-pc))
    return bytes(out)
