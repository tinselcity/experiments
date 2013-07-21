; ------------------------------------------------------------------------------
; Good enough Improvement:
; 0.0006^2 = 0.00000036
; But...
; scheme@(guile-user)> (sqrt 0.00000036)
; $3 = 0.0312538361560919
;
; ------------------------------------------------------------------------------
(define (average x y)
  (/ (+ x y) 2))

(define (improve guess x)
  (average guess (/ x guess)))

; Idea is to improve on the convergence in the guesses per each step.
; Get the next step with improve and verify the difference is
; less than guess/factor
;(eg 1000000 -difference less than millionth of guess)
(define (good-enough? guess x)
  (< (abs (- guess (improve guess x))) (/ guess 1000000)))

(define (sqrt-iter guess x)
  (if (good-enough? guess x)
      guess
      (sqrt-iter (improve guess x)
                 x)))

(define (sqrt x)
  (sqrt-iter 1.0 x))
