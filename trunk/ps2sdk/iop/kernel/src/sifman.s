/*
  _____     ___ ____ 
   ____|   |    ____|      PSX2 OpenSource Project
  |     ___|   |____       (C)2001, Gustavo Scotti (gustavo@scotti.com)
  ------------------------------------------------------------------------
  iop_sifman.s		   Serial Interface Manager Functions.
			   taken from .irx files with symbol table.
*/

	.text
	.set	noreorder


/* ############################### SIFMAN STUB ######## */
/* # Added by Oobles, 7th March 2002                  # */

	.local	sifman_stub
sifman_stub:
	.word	0x41e00000
	.word	0
	.word	0x00000101
	.ascii	"sifman\0\0"
	.align	2

	.globl  sceSifInit				# 0x05
sceSifInit:
	j	$31
	li	$0, 0x05

	.globl	sceSifSetDChain			# 0x06
sceSifSetDChain:
	j	$31
	li	$0, 0x06

	.globl	sceSifSetDma			# 0x07
sceSifSetDma:
	j	$31
	li	$0, 0x07

	.globl	sceSifDmaStat			# 0x08
sceSifDmaStat:
	j	$31
	li	$0, 0x08
	

	.globl	sceSifCheckInit			# 0x1d
sceSifCheckInit:
	j	$31
	li	$0, 0x1d

	.globl	sceSifSetDmaIntr		# 0x20
sceSifSetDmaIntr:
	j	$31
	li	$0, 0x20

	.word	0
	.word	0


