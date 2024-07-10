; Disassembly for listing_0041_add_sub_cmp_jnz
bits 16
ADD BX, [BX + SI]
ADD BX, [BP]
ADD SI, 2
ADD BP, 2
ADD CX, 8
ADD BX, [BP]
ADD CX, [BX +2]
ADD BH, [BP + SI +4]
ADD DI, [BP + DI +6]
ADD [BX + SI], BX
ADD [BP], BX
ADD [BP], BX
ADD [BX +2], CX
ADD [BP + SI +4], BH
ADD [BP + DI +6], DI
ADD [BX], byte 34
ADD [BP + SI +1000], word 29
ADD AX, [BP]
ADD AL, [BX + SI]
ADD AX, BX
ADD AL, AH
ADD AX, 1000
ADD AL, 226
ADD AL, 9
SUB BX, [BX + SI]
SUB BX, [BP]
SUB SI, 2
SUB BP, 2
SUB CX, 8
SUB BX, [BP]
SUB CX, [BX +2]
SUB BH, [BP + SI +4]
SUB DI, [BP + DI +6]
SUB [BX + SI], BX
SUB [BP], BX
SUB [BP], BX
SUB [BX +2], CX
SUB [BP + SI +4], BH
SUB [BP + DI +6], DI
SUB [BX], byte 34
SUB [BX + DI], word 29
SUB AX, [BP]
SUB AL, [BX + SI]
SUB AX, BX
SUB AL, AH
SUB AX, 1000
SUB AL, 226
SUB AL, 9
CMP BX, [BX + SI]
CMP BX, [BP]
CMP SI, 2
CMP BP, 2
CMP CX, 8
CMP BX, [BP]
CMP CX, [BX +2]
CMP BH, [BP + SI +4]
CMP DI, [BP + DI +6]
CMP [BX + SI], BX
CMP [BP], BX
CMP [BP], BX
CMP [BX +2], CX
CMP [BP + SI +4], BH
CMP [BP + DI +6], DI
CMP [BX], byte 34
CMP [4834], word 29
CMP AX, [BP]
CMP AL, [BX + SI]
CMP AX, BX
CMP AL, AH
CMP AX, 1000
CMP AL, 226
CMP AL, 9
jnz ($+2)+2
jnz ($+2)-4
jnz ($+2)-6
jnz ($+2)-4
je ($+2)-2
jl ($+2)-4
jle ($+2)-6
jb ($+2)-8
jbe ($+2)-10
jp ($+2)-12
jo ($+2)-14
js ($+2)-16
jnz ($+2)-18
jnl ($+2)-20
jg ($+2)-22
jnb ($+2)-24
ja ($+2)-26
jnp ($+2)-28
jno ($+2)-30
jns ($+2)-32
loop ($+2)-34
loopz ($+2)-36
loopnz ($+2)-38
jcxz ($+2)-40
