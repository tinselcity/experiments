; ------------------------------------------------------------------------------
; Tribonacci!
; f(n) :
;     = n (for n < 3)
;     = f(n - 1) + 2f(n-2) + 3f(n-3) (for n >= 3)
; ------------------------------------------------------------------------------
(define (tribonacci_rcsv n)
    (if (< n 3)
        n
        (+  (* 1 (tribonacci_rcsv (- n 1)))
            (* 2 (tribonacci_rcsv (- n 2)))
            (* 3 (tribonacci_rcsv (- n 3)))
        )
    )
)


(define (tribonacci_iter_ n n_1 n_2 n_3)
    (cond
        ((= n 0)
            n_1)
        (else
            (tribonacci_iter_ 
                (- n 1)
                (+  (* 1 n_1) (* 2 n_2) (* 3 n_3))
                n_1
                n_2)
        )
    )
)

(define (tribonacci_iter n)
    (if (< n 3)
        n
        ; else...
        (tribonacci_iter_ (- n 2) 2 1 0)
    )
)

