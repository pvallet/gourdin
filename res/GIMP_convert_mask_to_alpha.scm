(define (FU-convert-to-alpha-mask img drawable)
  (gimp-layer-add-alpha drawable)
  (gimp-image-select-color img CHANNEL-OP-REPLACE drawable
    (car (gimp-image-pick-color img drawable 0 0 FALSE FALSE 0))
  )
  (gimp-edit-clear drawable)
  (gimp-selection-none img)
)

(script-fu-register "FU-convert-to-alpha-mask"
	"<Image>/Script-Fu/Custom/Convert mask to alpha"
	"Replaces the color picked on the top left of the image by transparency."
	"Paul Vallet <luap.vallet@gmail.com>"
	"Paul Vallet"
	"2016, October"
	"*"
  SF-IMAGE      "Image"	         0
  SF-DRAWABLE   "Drawable"       0
)
