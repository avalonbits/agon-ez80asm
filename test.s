;add a, (hl)
;adl 1
;add.s a, (hl)
;adl 0
;add.l a, (hl)
;add a, ixh
;add a, ixl
;add a, iyh
;add a, iyl

;ADd A, (HL)
;add a,(ix+5)
;adl 1
;add.s a,(ix+5)
;adl 0
;add.l a,(ix+5)
;add a,(iy+5)
;adl 1
;add.s a,(iy+5)
;adl 0
;add.l a,(iy+5)
adl 1
bit.s 0, (HL)
bit 1, (HL)
bit 6, (HL)
bit 7, (HL)