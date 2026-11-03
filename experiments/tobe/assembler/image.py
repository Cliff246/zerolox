"""Experimental flat-image encoding, matching the current C decoder."""
import ast
import extra
from linker import link
from model import Error, mnemonic
from syntax import split_args
from values import expr, reg, data_size

MINOR = {'m.mv':0, 'm.add':1, 'm.adda':2, 'm.sub':3, 'm.geta':4, 'm.get8':5,
         'm.b':6, 'm.baeqz':7, 'm.baneqz':8,
         'm.lda':9, 'm.ald':10, 'm.sta':11, 'm.ast':12}
WIDTH = {'8':0, '16':1, '32':2, 'p':3}

def parcel(value):
    return value.to_bytes(2,'little')

def encode(rec, chosen, symbols, pc):
    args=rec['args']; op=mnemonic(rec); base=chosen.rsplit('.',1)[0]
    if op in extra.names():
        form,fields=extra.parse(op,args,symbols,pc)
        return extra.encode(form,fields)
    if chosen in ('m.mv','m.add','m.adda','m.sub'):
        if op in ('add','n.add','sub','n.sub'):
            d,a,b=map(reg,args)
            a,b=(a,b) if chosen in ('m.add','m.sub') else (d,b)
        else: a,b=map(reg,args)
        return parcel((MINOR[chosen]<<11)|(a<<6)|(b<<1)|1)
    if chosen in ('m.geta','m.b','m.baeqz','m.baneqz'):
        v=expr(args[0],symbols,pc)
        if chosen!='m.geta': v//=2
        return parcel((MINOR[chosen]<<11)|((v & 1023)<<1)|1)
    if chosen=='m.get8':
        return parcel((MINOR[chosen]<<11)|((reg(args[0])-8)<<9)|(expr(args[1],symbols,pc)<<1)|1)
    if chosen in ('n.add','n.sub','n.sll','n.srl','n.sra','n.xor','n.or','n.and'):
        d,a,b=map(reg,args)
        selector={'n.add':0,'n.sub':1,'n.sll':2,'n.srl':3,'n.sra':4,'n.xor':5,'n.or':6,'n.and':7}[chosen]
        return parcel((12<<6)|(a<<1))+parcel((selector<<11)|(d<<6)|(b<<1))
    if base in ('m.lda','m.ald','m.sta','m.ast','n.ld','n.st'):
        fields=split_args(args[-1][1:-1]); address=reg(fields[0]); offset=expr(fields[1],symbols,pc) if len(fields)==2 else 0
        width=chosen.rsplit('.',1)[1]; size={'8':1,'16':2,'32':4,'p':3}[width]
        value_reg=1 if op.startswith(('m.lda.','m.sta.')) else reg(args[0])
        if base.startswith('m.'):
            operand=address if base in ('m.lda','m.sta') else value_reg
            return parcel((MINOR[base]<<11)|(WIDTH[width]<<9)|((offset//size)<<6)|(operand<<1)|1)
        raw=offset & 0x1ff
        first=((0 if base=='n.ld' else 1)<<6)|(address<<1)
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
            code=encode(rec,chosen,symbols,pc)
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
