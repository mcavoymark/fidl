;Copyright 12/31/99 Washington University.  All Rights Reserved.
;stats.h $Revision: 12.188 $

; Variables defined for roi program.

rev = "$Revision: 12.188 $
;;;defsysv,'!FIDL_REV',strmid(rev,11,strlen(rev)-13)

;defsysv,'!NUM_FILES',40		; Maximum simultaneously opened files.
;defsysv,'!NUM_FILES',100		; Maximum simultaneously opened files.
;START130220
defsysv,'!NUM_FILES',1000

defsysv,'!OK',-1
defsysv,'!FALSE',0
defsysv,'!TRUE',1
defsysv,'!NEVER',2

defsysv,'!TRIAL',1
defsysv,'!RUN',2
defsysv,'!DATASET',3

defsysv,'!CREATE',1
defsysv,'!APPEND',2

defsysv,'!DATA_X',64
defsysv,'!DATA_Y',64
defsysv,'!THREET_X',128
defsysv,'!THREET_Y',128

defsysv,'!ATLAS_111_X',176L
defsysv,'!ATLAS_111_Y',208L
defsysv,'!ATLAS_111_Z',176L
defsysv,'!VOL_111',long(!ATLAS_111_X)*long(!ATLAS_111_Y)*long(!ATLAS_111_Z)
defsysv,'!LENBRAIN_111',1726930L

defsysv,'!ATLAS_222_X',128L
defsysv,'!ATLAS_222_Y',128L
defsysv,'!ATLAS_222_Z',75L
defsysv,'!VOL_222',long(!ATLAS_222_X)*long(!ATLAS_222_Y)*long(!ATLAS_222_Z)
defsysv,'!LENBRAIN_222',210237L
defsysv,'!LENBRAIN',210237 ;298130
defsysv,'!ATLAS_222_YOFFSET',8  ; Offset of center in 222 space along y axis.

defsysv,'!ATLAS_333_X',48L
defsysv,'!ATLAS_333_Y',64L
defsysv,'!ATLAS_333_Z',48L
defsysv,'!VOL_333',long(!ATLAS_333_X)*long(!ATLAS_333_Y)*long(!ATLAS_333_Z)
defsysv,'!LENBRAIN_333',65549L

defsysv,'!MNI222_X',91L
defsysv,'!MNI222_Y',109L
defsysv,'!MNI222_Z',91L
defsysv,'!VOL_MNI222',long(!MNI222_X)*long(!MNI222_Y)*long(!MNI222_Z)
defsysv,'!LENBRAIN_MNI222',292019L

defsysv,'!MNI333_X',60L
defsysv,'!MNI333_Y',72L
defsysv,'!MNI333_Z',60L
;defsysv,'!VOL_MNI333',long(!MNI333_X)*long(!MNI333_Y)*long(!MNI333_Z)

;START170614
defsysv,'!MNI111_X',197L
defsysv,'!MNI111_Y',233L
defsysv,'!MNI111_Z',189L
;defsysv,'!VOL_MNI111',long(!MNI111_X)*long(!MNI111_Y)*long(!MNI111_Z)

defsysv,'!SPACE_DATA',0
defsysv,'!SPACE_111',1
defsysv,'!SPACE_222',2
defsysv,'!SPACE_333',3
defsysv,'!SPACE_MNI222',4
defsysv,'!SPACE_MNI333',5

;START170614
defsysv,'!SPACE_MNI111',6

defsysv,'!MMPPIX_X_111',1.
defsysv,'!MMPPIX_Y_111',-1.
defsysv,'!MMPPIX_Z_111',-1.
defsysv,'!CENTER_X_111',89.
defsysv,'!CENTER_Y_111',-85.
defsysv,'!CENTER_Z_111',-101.

defsysv,'!MMPPIX_X_222',2.
defsysv,'!MMPPIX_Y_222',-2.
defsysv,'!MMPPIX_Z_222',-2.
defsysv,'!CENTER_X_222',129.
defsysv,'!CENTER_Y_222',-129.
defsysv,'!CENTER_Z_222',-82.

defsysv,'!MMPPIX_X_333',3.
defsysv,'!MMPPIX_Y_333',-3.
defsysv,'!MMPPIX_Z_333',-3.
defsysv,'!CENTER_X_333',73.5
defsysv,'!CENTER_Y_333',-87.
defsysv,'!CENTER_Z_333',-84.

defsysv,'!MMPPIX_X_MNI222',2.
defsysv,'!MMPPIX_Y_MNI222',-2.
defsysv,'!MMPPIX_Z_MNI222',-2.

defsysv,'!VIEW_XDIM_DRAW',768
defsysv,'!VIEW_YDIM_DRAW',512

;defsysv,'!VIEW_DX',85
;defsysv,'!VIEW_DY',95
;defsysv,'!VIEW_DZ',75

defsysv,'!TRANSVERSE_WIDTH_DFLT',85
defsysv,'!TRANSVERSE_HEIGHT_DFLT',95
defsysv,'!SAGITTAL_WIDTH_DFLT',115
defsysv,'!SAGITTAL_HEIGHT_DFLT',75
defsysv,'!CORONAL_WIDTH_DFLT',115
defsysv,'!CORONAL_HEIGHT_DFLT',75



defsysv,'!DISPLAY_Z',0
defsysv,'!DISPLAY_P',1
defsysv,'!DISPLAY_P_ROUNDED',2

defsysv,'!VIEW_MAX_LABELS',40

defsysv,'!MOVE_CELL',1
defsysv,'!DEFINE_BOX',2
defsysv,'!MOVE_LABEL',3

defsysv,'!PAIRED_COMPARISON',0
defsysv,'!UNPAIRED_COMPARISON',1
defsysv,'!GROUP_COMPARISON',2
defsysv,'!CORRELATION',3
defsysv,'!PARTIAL_CORRELATION',4
defsysv,'!SS_PARTIAL_CORRELATION',5
defsysv,'!NONLINEAR',6
defsysv,'!SS_VS_GROUP',7
defsysv,'!TYPE_II_REGRESSION',8

;START120725
defsysv,'!LINEARREGRESS',9

defsysv,'!VSCL',1.665109 ; sqrt(4*ln2)
defsysv,'!UNSAMPLED_VOXEL',1.e-37

;volume counts for Worsley's unified model
defsysv,'!I_222',78 ; [25,102]
defsysv,'!J_222',96 ; [25,120]
defsysv,'!K_222',75 ; [1,75]

defsysv,'!BATCH',0  ; FALSE = 0

defsysv,'!MAX_CONTRASTS',100  ; Maximum number of contrasts.
defsysv,'!MAX_CONTRAST_NAME',120  ; Maximum length of a contrast name.

defsysv,'!MAX_WORKING_WINDOWS',20  ; Maximum number of working windows.

;defsysv,'!GRAND_MEAN_THRESH',100  ; FALSE = 0
defsysv,'!GRAND_MEAN_THRESH',1

;Types of functions for hemodynamic response.
defsysv,'!TIME_COURSE_EST',0
defsysv,'!BOYNTON',1
defsysv,'!FRISTON',2   ; Sum of 3 gamma function and their derivatives.
defsysv,'!BEHAVIORAL',3
defsysv,'!BOXCAR_STIM',4
defsysv,'!SPM_CANNONICAL',5
defsysv,'!GAMMA_DERIV',6
defsysv,'!BLOCK_DESIGN',7

;defsysv,'!USER_DEFINED',8
;START69
defsysv,'!TREND',8
defsysv,'!USER_DEFINED',9

defsysv,'!GAMMA_ORDER',6 ; Order of gamma function in the "GAMMA_DERIV" model for the hrf.
defsysv,'!GAMMA_DERIV_ASLOPE',.161447 ; First slope for linear approx. to magnitude.
defsysv,'!GAMMA_DERIV_BSLOPE',-0.00111060 ; Second slope for linear approx. to magnitude.

defsysv,'!NEAREST_NEIGHBOR',1
defsysv,'!BILINEAR',2
defsysv,'!CUBIC_SPLINE',3

defsysv,'!DRAW_LOCATION',1
defsysv,'!DRAW_BOX',2

; Dummy define
defsysv,'!FMRI_LEADER',0L

defsysv,'!MAX_ACQ_ZDIM',30L	; Maximum number of acquired planes. This is
; used as a threshold to determine if a file has been converted to atlas space.

defsysv,'!EDIT_MASK_SIZE',256L

defsysv,'!EDIT_MASK_NONE',0	; No action.
defsysv,'!EDIT_MASK_DELETE',1	; Delete region.
defsysv,'!EDIT_MASK_LABEL',2	; Label region.
defsysv,'!EDIT_MASK_DELETE_VOXELS',3	; Delete voxel.
defsysv,'!EDIT_MASK_ADD_VOXELs',4	; Add voxel.
defsysv,'!EDIT_MASK_LABEL_VOX',5	; Label voxel.
defsysv,'!EDIT_MASK_UNLABEL',6	; Unlabel voxels.

defsysv,'!GLMSIM_STIMLEN',7
defsysv,'!GLMSIM_MAXEFF',9
defsysv,'!GLMSIM_MAX_STIMLEN',9

defsysv,'!GLM_SKIP',4

defsysv,'!HIPASS_CUTOFF',.009	; Cutoff freq of hi-pass filter in Hz.

defsysv,'!LITTLE_ENDIAN',0

defsysv,'!STATS_FONT','-*-terminal-medium-r-normal-*-14-140-*-*-*-*-iso8859-*'

defsysv,'!VIEW_FONT_FAMILIES',['times','helvetica','courier','symbol']

defsysv,'!BASIS_DELTA',0 ; Estimate time-courses.
defsysv,'!BASIS_FUNC',1  ; Regress against assumed response shape.

defsysv,'!HRF_DELTA',2.
defsysv,'!HRF_TAU',1.25
defsysv,'!TR_PLANE',.136  ; Time required to image a single slice.

defsysv,'!HRF_FRISTON_DELTA1',1.1716
defsysv,'!HRF_FRISTON_DELTA2',2.
defsysv,'!HRF_FRISTON_DELTA3',10.3431
defsysv,'!HRF_FRISTON_TAU1',1.4142
defsysv,'!HRF_FRISTON_TAU2',2.
defsysv,'!HRF_FRISTON_TAU3',2.8284
defsysv,'!HRF_FRISTON_ALPHA1',4.
defsysv,'!HRF_FRISTON_ALPHA2',8.
defsysv,'!HRF_FRISTON_ALPHA3',16.

defsysv,'!HRF_SPM_RESPONSE_DELAY',6
defsysv,'!HRF_SPM_UNDERSHOOT_DELAY',16
defsysv,'!HRF_SPM_MAGNITUDE_RATIO',6

defsysv,'!TARGET_2A',1
defsysv,'!TARGET_2B',2

defsysv,'!ASSOC_ARRAY',1
defsysv,'!FLOAT_ARRAY',2
defsysv,'!STITCH_ARRAY',3
defsysv,'!OVERLAY_ARRAY',4
defsysv,'!LINEAR_MODEL',5
defsysv,'!VARIANCE',6
defsysv,'!LOGICAL_ARRAY',7

defsysv,'!OVERLAY_MODE',1
defsysv,'!ANAT_OVER_ACT_MODE',2

defsysv,'!ANATOMIC',1
defsysv,'!STATISTICAL',2

defsysv,'!BOXCAR',1
defsysv,'!GAUSSIAN',2

defsysv,'!NEURO',1
defsysv,'!RADIOL',2

defsysv,'!PROF_NO',1
defsysv,'!PROF_HV',2
defsysv,'!PROF_T',3
defsysv,'!PROF_REG',4
defsysv,'!PROF_NWIN',500
defsysv,'!MAX_PROFILES',500

defsysv,'!SIGNIFICANCE',1
defsysv,'!PERCENT',2

defsysv,'!EDGE',1  ; Edge and Face connectivity for regions in multcomp.
defsysv,'!FACE',2  ; Face connectivity for regions in multcomp.

defsysv,'!Z_STAT',1		; Z statistic.
defsysv,'!T_STAT',2		; T statistic.
defsysv,'!LOG_PROB',3		; Log-probability map.
defsysv,'!F_STAT',4             ; F statistic

defsysv,'!ALL_ACTIVATIONS',1	; Show all activations.
defsysv,'!POSITIVE_ACTIVATIONS',2 ; Show positive activations.
defsysv,'!NEGATIVE_ACTIVATIONS',3 ; Show negative activations.

defsysv,'!DRAG_MOVE',1 ; Move cell as cursor is dragged.

defsysv,'!SQRT2',1.41421356

defsysv,'!PLAIN_TPROF',1	; Simple sum over time profile region.
defsysv,'!THRESH_TPROF',2	; Sum over threshold pixels in profile region.
defsysv,'!GANG_TPROF',3		; 3x3 ganged array of plots.
defsysv,'!HISTO_TPROF',4	; Compute histogram over time profile.
defsysv,'!EXT_THRESH_TPROF',5	; Use external threshold on profile.
defsysv,'!REGION_TPROF',6	; Region defined on overlay image.
defsysv,'!NOT_GANGED',7		; Not a ganged profile
defsysv,'!SCATTER_PLOT',8	; Scatter plot of correlation analysis.

defsysv,'!LOCAL',0
defsysv,'!GLOBAL',1
defsysv,'!FIXED',2
defsysv,'!VOLUME',3
defsysv,'!SLICE',4

defsysv,'!PLOT_NOTHING',0	; Overplot nothing.
defsysv,'!PLOT_PARADIGM',1	; Plot paradigm over time-course.
defsysv,'!PLOT_GLM',2		; Overplot estimated time-course.
defsysv,'!PLOT_RESIDUALS',3	; Overplot residuals of linear model.
defsysv,'!PLOT_HRF',4		; Overplot assumed hemodynamic response.
defsysv,'!PLOT_PCA',5		; Overplot principal eigenvector.
defsysv,'!PLOT_STDERR',6	; Overplot standard error.

defsysv,'!PROF_PROC_NOTHING',0	; No processing.
defsysv,'!PROF_PROC_PCT',1	; Define magnitude based on a percentile.

;defsysv,'!GLM_FILE_REV',-25
;defsysv,'!GLM_FILE_REV',-26
;START160914
defsysv,'!GLM_FILE_REV',-27

defsysv,'!PARADIGM_HGHT',120
defsysv,'!PARADIGM_OFFSET',50
defsysv,'!PARADIGM_WIDTH',612
defsysv,'!PARADIGM_Y0',40
defsysv,'!PARADIGM_Y1',100

defsysv,'!DRAW_SIZE',512	; Size of image displayed.
defsysv,'!DRAW_SIZEX',512	; Size of image displayed.
defsysv,'!DRAW_SIZEY',512	; Size of image displayed.
defsysv,'!DRAW_SIZE_TOT',512	; Size of image displayed (3-pixel frame).
defsysv,'!DRAW_SCROLL',400
defsysv,'!ZOOM_SIZE',256

defsysv,'!SQUARE',+6		; Square plotting symbol.
defsysv,'!PLUS',+1		; Plus plotting symbol.
defsysv,'!TRIANGLE',+5		; Triangle plotting symbol.
defsysv,'!STAR',+2		; Asterisk plotting symbol.
defsysv,'!DOT',+3		; Period plotting symbol.

defsysv,'!TRANSVERSE',1
defsysv,'!SAGITAL',2
defsysv,'!SAGITTAL',2
defsysv,'!CORONAL',3
defsysv,'!VIEW_COLOR_SCALE',4

defsysv,'!NO_COLOR_BAR',0
defsysv,'!HORIZ_COLOR_BAR',1
defsysv,'!VERT_COLOR_BAR',2

defsysv,'!VIEW_WINDOWS_MAX',10
defsysv,'!MAX_CELLS',100

defsysv,'!MAX_BLTIMG',30	; Maximum number of panes in custom image.

;;;defsysv,'!SHARE_LIB',string('/usr/local/lib/fidl.so') ;Library containing shared object modules.
;;;defsysv,'!BIN',string('/usr/local/bin/stats/bin') ;Path to binary files.

defsysv,'!GRAY_SCALE',1		; Index of gray color scale.
defsysv,'!GRAY_SCALE_10',2	; Gray color scale (10 levels).
defsysv,'!COLOR_42_SCALE',3		; Index of color 42 color scale.
defsysv,'!COLOR_42_WHITE_SCALE',4 ; Color 42 color scale with white as maximum.
defsysv,'!DIFF_SCALE',5		; Index of difference image scale.
defsysv,'!OVERLAY_SCALE',6	; Activation overlay color scale.
defsysv,'!YELLOW_TO_RED',7	; yellow on bottom, red on top 
defsysv,'!LINECOLORS',8
defsysv,'!BLACK_BODY',9		; Index of black body color scale.
defsysv,'!DEFAULT_SCALE',10	; Use current default scale.
defsysv,'!NO_SCALE',11	        ; No color scale defined.

defsysv,'!RUSS_FIV_NOMID',12	        

;defsysv,'!MAX_NUM_COLORS',187 ;Length of color table for gray scale.
;;;defsysv,'!LEN_COLORTAB',175 ;Length of color table for gray scale.
defsysv,'!NUM_LINECOLORS',15 ;Number of colors reserved for line-drawing.
;defsysv,'!NUM_LINECOLORS',70 ;Number of colors reserved for line-drawing.
;defsysv,'!NUM_LINECOLORS',100 ;Number of colors reserved for line-drawing.

;;Now color table changed, redefine length
;defsysv,'!WHITE',182
;defsysv,'!YELLOW',183
;defsysv,'!RED',184
;defsysv,'!GREEN',185
;defsysv,'!BLUE',186

defsysv,'!COLSCL_WIN_XSZ',90
defsysv,'!COLSCL_WIN_XSZ_DUAL',180
defsysv,'!COLSCL_WIN_YSZ',350
defsysv,'!COLSCL_XOFF',15
defsysv,'!COLSCL_YOFF',30
defsysv,'!COLSCL_YSZ',300
defsysv,'!COLSCL_WIDTH',20

defsysv,'!CUSTOM_XSZ',520
defsysv,'!CUSTOM_YSZ',776

defsysv,'!CTI',0
defsysv,'!CUBE',1
defsysv,'!TIFF',2
defsysv,'!MONTE_ALL',3
defsysv,'!STITCH',4
defsysv,'!RAW',5
defsysv,'!INTERFILE',7
defsysv,'!MRI_ANALYZE',9
defsysv,'!MRI_4DFP',10
defsysv,'!GIF',11
defsysv,'!ECAT_DATA',12
defsysv,'!MATRIX7V',13
defsysv,'!SPM_MAT',14
defsysv,'!UNKNOWN',15
defsysv,'!XWD',16

;START160112
defsysv,'!LIST',17


defsysv,'!BYTE',1
defsysv,'!SHORT',2
defsysv,'!FLOAT',4
defsysv,'!DOUBLE',8
defsysv,'!COMPLEX',16

defsysv,'!NANGMC',96
defsysv,'!NRAYMC',64
defsysv,'!NRINGMC',4
defsysv,'!NRINGSQMC',16
defsysv,'!NSCATMC',4
defsysv,'!NPHGTOT',6291456

defsysv,'!NANGCTI',192
defsysv,'!NRAYCTI',128

defsysv,'!PRINTER',"magoo"
defsysv,'!COLOR_PRINTER',"tweetie"


;defsysv,'!PRINTERS',"magoo tweetie hpdj2 ljcolor luau wimpy bugs"
;defsysv,'!PRINTERS','magoo tweetie hpdj2 ljcolor luau wimpy bugs "HP Color LaserJet 3550"'
;defsysv,'!PRINTERS','magoo tweetie hpdj2 ljcolor luau wimpy bugs HP_Color_LaserJet_3550'
;defsysv,'!PRINTERS','magoo,tweetie,hpdj2,ljcolor,luau,wimpy,bugs,HP Color LaserJet 3550'
;defsysv,'!PRINTERS','magoo,tweetie,hpdj2,ljcolor,luau,wimpy,bugs,trust'
;defsysv,'!PRINTERS',['magoo','tweetie','hpdj2','ljcolor','luau','wimpy','bugs','HP Color LaserJet 3550']

defsysv,'!PRINTERS','magoo,tweetie,hpdj2,ljcolor,luau,wimpy,bugs,HP Color LaserJet 3550,clj'



defsysv,'!PRINT_PORTRAIT',1
defsysv,'!PRINT_LANDSCAPE',2
defsysv,'!PRINT_COLOR',3
defsysv,'!PRINT_BW',4
defsysv,'!PRINT_PRINTER',5
defsysv,'!PRINT_FILE',6
defsysv,'!PRINT_XSIZE',10
defsysv,'!PRINT_YSIZE',7.5
defsysv,'!PRINT_XOFFSET',.5
defsysv,'!PRINT_YOFFSET',.5
defsysv,'!PRINT_X_PAPER_SIZE',8.5
defsysv,'!PRINT_Y_PAPER_SIZE',11.

defsysv,'!LINE',1
defsysv,'!SYMBOL',2
defsysv,'!SYMLINE',3

defsysv,'!SOLID',0
defsysv,'!DOTTED',1

defsysv,'!IRREG_ROI',1
defsysv,'!SQUARE_ROI',2
defsysv,'!CIRC_ROI',3

defsysv,'!PRESS',1
defsysv,'!RELEASE',2
defsysv,'!MOTION',3

defsysv,'!WIDGET',0
defsysv,'!DRAG',4
defsysv,'!CLICK',5
defsysv,'!DOUBLE_CLICK',6

defsysv,'!DRAW_OFF',0	; Values for prof_mode.
defsysv,'!DRAWING',1
defsysv,'!DRAW_DONE',2

cmd = string('pwd')
if(!VERSION.OS_FAMILY eq 'Windows') then $
    str = 'd:\data'$
else $
    spawn,cmd,str
defsysv,'!PWD',str(0)

path = getenv('HOME')
defsysv,'!HOME',path

fidl_macro = strcompress(getenv('FIDL_MACRO'))
defsysv,'!FIDL_MACRO',fidl_macro

defsysv,'!INTERNAL',0 ;fi.whereisit
defsysv,'!ON_DISK',1 

defsysv,'!NAF3D_FILES',10
defsysv,'!AF3D_NCOLORS',10

;defsysv,'!DISPXMAX',1024
;defsysv,'!DISPYMAX',800
