; ------------------------------------------------------------------------------
; Newtons method for cube roots
; 
; new_guess = (((x/y^2) + 2y) / 3) 
; ------------------------------------------------------------------------------

(define (square x) (* x x))

(define (improve_cbrt guess x)
    (/ (+ (/ x (square guess)) (* 2 guess)) 3))

; | g - gn | / g < g / 1000000
(define (good_enough? guess x)
    (< (abs (- guess (improve_cbrt guess x))) (/ guess 1000000)))

; Cube root iterator
(define (cbrt_iter guess x)
    (if (good_enough? guess x)
      guess
      (cbrt_iter (improve_cbrt guess x) x)))

; Cube Root
(define (cbrt x)
    (cbrt_iter 1.0 x))
