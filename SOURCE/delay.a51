
NAME	DELAY

;-----------------------------------------------------------------------------
;---- extern void udelay(unsigned long usecs);
?PR?udelay?DELAY	SEGMENT CODE
	PUBLIC	_udelay
	RSEG	?PR?udelay?DELAY
	USING	0

;---- variable 'usecs' assigned to register 'R4/R5/R6/R7'
_udelay:
	MOV	A, #0x01
	XCH	A, R7
	CPL	A
	ADD	A, R7
	XCH	A, R7

	CLR	A
	XCH	A, R6
	CPL	A
	ADDC	A, R6
	XCH	A, R6

	XCH	A, R5
	CPL	A
	ADDC	A, R5
	XCH	A, R5

	XCH	A, R4
	CPL	A
	ADDC	A, R4
	XCH	A, R4

	;.......
__udelay:
	CLR	C
	MOV	A, #0x01

	ADD	A, R7
	XCH	A, R7
	CLR	A

	ADDC	A, R6
	XCH	A, R6
	CLR	A

	ADDC	A, R5
	XCH	A, R5
	CLR	A

	ADDC	A, R4
	XCH	A, R4

	;.......
	MOV	A, R4
	ORL	A, R5
	ORL	A, R6
	ORL	A, R7
	JNZ	__udelay
	RET


	END
