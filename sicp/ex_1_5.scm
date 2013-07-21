; ------------------------------------------------------------------------------
; Applicative v. Normal Order Evaluation...
; Answer:
; In applicative order eval the p will call itself and loop forever.
; In normal form eval comes after expansion and since x == 0
; 
; Note this program will run until SO in scheme interp after load since scheme uses
; applicative order eval.
; ------------------------------------------------------------------------------


(define (p) (p))

(define (test x y)
	(if (= x 0)
		0
		y))

(test 0 (p))
