(define (range n)
  (if (negative? n)
      '()
      (cons n (range (- n 1)))
  )
)

(define (FU-complete-aoe-sprite-set img drawable nbColumns)
  (let* (
    (width (car (gimp-drawable-width drawable)))
    (height (/ (car (gimp-drawable-height drawable)) 5))
    (layer)
  )

  (define (copy-strip oldpos newpos)
    (gimp-image-select-rectangle img CHANNEL-OP-REPLACE 0 (* height oldpos) width height)
    (gimp-edit-copy drawable)
    (set! layer (car (gimp-edit-paste drawable FALSE)))
    (gimp-layer-translate layer 0 (* height (- newpos oldpos)))
    (gimp-floating-sel-anchor layer)
  )

  (gimp-image-resize img      width (* height 8) 0 0)
  (gimp-layer-resize drawable width (* height 8) 0 0)

  (copy-strip 1 7)
  (copy-strip 2 6)
  (copy-strip 3 5)

  (set! width (/ width nbColumns))

  (for-each (lambda (i)
    (gimp-image-select-rectangle img CHANNEL-OP-REPLACE
      (* width i) (* height 5) width (* height 3))
    (gimp-item-transform-flip-simple drawable ORIENTATION-HORIZONTAL TRUE 0)

    ) (range (- nbColumns 1)))
  )
)

(script-fu-register "FU-complete-aoe-sprite-set"
	"<Image>/Script-Fu/Custom/Complete aoe sprite set"
	"Complete the last 3 orientations of an Age of Empires sprite set by symmetry."
	"Paul Vallet <luap.vallet@gmail.com>"
	"Paul Vallet"
	"2017, April"
	"*"
  SF-IMAGE      "Image"	         0
  SF-DRAWABLE   "Drawable"       0
  SF-VALUE      "nbColumns"      "10"
)
