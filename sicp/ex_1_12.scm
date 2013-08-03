; ------------------------------------------------------------------------------
; Pascal's triangle
;
; Each term in row is the sum of two numbers above...
;
; 1.           1
;
; 2.         1   1
;             \ /
; 3.       1   2   1
;           \ / \ /
; 4.     1   3   3   1
;         \ / \ / \ /
; 5.   1   4   6   4   1
;       \ / \ / \ / \ /
; 6. 1   5   10  10  5   1
; ...
;
; ------------------------------------------------------------------------------
(define (pascal_rcsv row col)
	(cond 
		((or (= col 1) (= row col)) 1)
		(else 
			(+ (pascal_rcsv (- row 1) (- col 1)) (pascal_rcsv (- row 1) col))
		)
	)
)

