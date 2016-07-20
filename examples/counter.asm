.data
     count: 60
     array: .space 10
     char: 0xfe
.code
       	ldi 0 count
       	ld  0 0
       	ldi 1 array
       	ldi 2 char
       	ld  2 2
lpp:
        st 1 2
       	inc 1
       	dec 0
       	jz loop
       	jmp lpp
loop:
        sub 1 2 3
lp1:
        jmp lp1
