(begin
  (define tail-sum (lambda (i n a) (if (eq? i n) a (tail-sum (+ i 1) n (+ i a)))))
  (define recur-sum (lambda (i n) (if (eq? i n) 0 (+ i (recur-sum (+ i 1) n)))))
  (define x 3)
  (define y 4)
  (define sqr (lambda (x) (* x x)))
  (define append
    (lambda (xs ys)
      (if (eq? xs '()) 
          ys 
          (cons (car xs) (append (cdr xs) ys)))))
  (define enumerate 
    (lambda (i n)
      (if (eq? i n) 
          '() 
          (cons i (enumerate (+ i 1) n)))))
  (define for 
    (lambda (f i n)
      (if (eq? i n) 
          '() 
          (begin (f) (for f (+ i 1) n)))))
  (define loop (lambda (i n) (if (eq? i n) 0 (loop (+ i 1) n))))
  (print (+ (sqr x x) (sqr y)))
  (print '(yes i am))
  ; (loop 0 1000000)
  (for (lambda () (print (append (enumerate 0 10) (enumerate (- 0 10) 10)))) 0 2048)
  (if (eq? 'yes 'yes) 'no 'yes)
  )
