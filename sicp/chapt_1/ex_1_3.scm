; Square
(define (square x)
    (* x x)
)

; Sum of Squares
(define (sum_of_squares x y)
    (+ (square x) (square y))
)

; Larger
(define (larger x y) (if (> x y) x y))

; Sum of Squares of 2 larger arguments
(define (sum_sq_larger_2 x y z)
    (cond
        ((> x y) (sum_of_squares x (larger y z)))
        (else (sum_of_squares y (larger x z)))
    )
)


