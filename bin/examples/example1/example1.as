; file example1.as
mov r0,r1
cmp #1,LABEL1
.extern W
.entry LABEL2
jmp W(r0,#-1)
LABEL1: .data 6,-9,15
STRING: .string "abcdef"
LABEL2: jmp END
not r0
END: stop