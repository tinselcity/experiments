; ------------------------------------------------------------------------------
; Answer:
; if b is greater than 0
;     value of compound expression evaluates to operator "+"
;     else evaluates to operator "-"
; ------------------------------------------------------------------------------
(define (a_plus_abs_b a b)
	((if (> b 0) + -) a b))
