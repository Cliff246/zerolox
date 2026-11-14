/:
%include "whatever.txt"
/:this defines that things of group "csv" should be next to each other
%group "csv" 
%segment .text

/:comment
csv_row:

    mv ix, z0
    get t0, 44 /: m.get8 t0, 44
    get t1, 34 /: m.get8 t1, 34
    get t2, 13 /: m.get8 t2, 13
    get t3, 10 /: m.get8 t3, 10

field_begin:
/:this is m.bieqc with x iterator, counter pair 
/:the others would be byeqc, byneqc, bxneqc. or it would be 
/:bne ix, cx, #capacity_error 
/:could be an incrementor or decrementor form, in which case it would be bne+ ix{+4}, cx

/:we have many different forms

    bxeqc #capacity_error 
    mv ac, t5
    st+a.p [s0{1}]

    inc ix
    ld+a.8 [t4{1}]
    bteqa t1, #quoted_loop

plain_check:
    baeqz #end_eof
    bteqa t0, #end_comma
    bteqa t3, #end_line
    bteqa t2, #end_cr
    bteqa t1, #bad_csv
    st+a.8 [t5{1}]
    ld+a.8 [t4{1}]
    b #plain_check

quoted_loop:
    ld+a.8 [t4{1}]
    baeqz #bad_csv
    bteqa t1, #after_quote
    st+a.8 [t5{1}]
    b #quoted_loop

after_quote:
    ld+a.8 [t4{1}]
    m.baeqz #end_eof
    m.bteqa t1, #escape_quote
    m.bteqa t0, #end_comma
    m.bteqa t3, #end_line
    m.bteqa t2, #end_cr

bad_csv:
    get ac, -1 /:m.geta -1
    ret ac

escape_quote:
    st+a.8 [t5{1}]
    b #quoted_loop

end_comma:
    mv ac, z0
    st+a.8 [t5{1}]
    b #field_begin

end_cr:
    lda.8 [t4, 0]
	/:could be bneq ac, t3, #end_line
    btneqa t3, #end_line
    inc t4

end_line:
    mv ac, z0
	
	/: could be 
	/:st.8 ac, [t5, 0] 
    sta.8 [t5, 0]
	/: empty ret is just ret ac
    ret


/: for example  
/: when we see, the assembler should pick the form smallest that can fit the necesary data. 
/: if the data is unknown, assume the 'best' case scenario that it fits the smallest form.
/: as in if there is an unknown data type, in the assemblers sight. it reserves the smallest it could be. and errors if it fails. 
/: we will see if this is a problem. but use bigger forms then, or declare the data large. 
/: semantic operations are what should be used otherwise. 
/: ugh.... hopefully this isn't a pain. but @get for example would be able to load of anysize.
/: @b, @bne, @beq, @blt @bge, @bltu, @bgeu,



%segment .bss
data:
	!db "hello world"



