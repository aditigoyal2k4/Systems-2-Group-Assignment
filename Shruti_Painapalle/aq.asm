section .text
	global main
main:	xor ecx,ecx
	and eax,ebx
	or esi,ebx
	ret
