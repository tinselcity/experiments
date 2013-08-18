; ------------------------------------------------------------------------------
; Simpson's rule:
; 
; h/3 * [ y0 + 4*y1 + 2*y2 + 4*y3 + 2*y4 + ... + 2*yn-2 + 4*yn-1 + yn ]
;
; where:
; h = (b - a) / n
; ------------------------------------------------------------------------------
(define (sum term a next b)
  (if (> a b)
      0
      (+ (term a)
         (sum term (next a) next b))))


(define (even? x) (= 0 (remainder x 2)))
;(define (odd? x) (not (even? x)))


; ------------------------------------------------------------------------------
; ------------------------------------------------------------------------------
(define (simpsons-integral f a b n)
    (define (inc x) (+ x 1))
    (define h (/ (- b a) n))
	(define (term k)
		(*
			(cond
				((or (= k 0) (= k n)) 1)
				((even? k) 2)
				(else 4)
			)
			;1
			(f (+ a (* k h)))
		)
	)
    (/ (* h (sum term 0 inc n)) 3)
)


