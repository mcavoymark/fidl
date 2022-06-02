;Copyright 12/31/99 Washington University.  All Rights Reserved.
;load_colortable.pro  $Revision: 1.54 $

;*****************************
pro adjust_colortable_event,ev
;*****************************
common adjust_colortable_comm,red1,green1,blue1,wd_min1,wd_max1,wd_gamma1, $
    wd_tables1,wd_exit,num_colors1,lct1,lc_use_2nd_table, $
    image_index,wd_tables2,lct2,wd_min2,wd_max2,wd_gamma2,table_num

common stats_comm,wd,wd1,fi,pr,dsp,fl,st,stc,cstm,bltimg, $
       glm,dsp_image,help,pref,first_pass,xm1,ym1,vw

;print,'here adjust_colortable_event top'

lcshow = !FALSE
case ev.id of

wd_tables1: begin
    case ev.value of
        0: begin
            fi.color_scale1[fi.current_colors] = !GRAY_SCALE
            fi.color_scale2[fi.current_colors] = !NO_SCALE
            fi.color_gamma1[fi.n] = -2
            widget_control,wd_gamma1,SET_VALUE = fi.color_gamma1[fi.n]
            lc_use_2nd_table = !FALSE
            widget_control,wd_tables2,SENSITIVE=0
            widget_control,wd_min2,SENSITIVE=0
            widget_control,wd_max2,SENSITIVE=0
            widget_control,wd_gamma2,SENSITIVE=0
        end
        1: begin
            fi.color_scale1[fi.current_colors] = !GRAY_SCALE_10
            fi.color_scale2[fi.current_colors] = !NO_SCALE
            fi.color_gamma1[fi.n] = 0
            widget_control,wd_gamma1,SET_VALUE = fi.color_gamma1[fi.n]
            lc_use_2nd_table = !FALSE
            widget_control,wd_tables2,SENSITIVE=0
            widget_control,wd_min2,SENSITIVE=0
            widget_control,wd_max2,SENSITIVE=0
            widget_control,wd_gamma2,SENSITIVE=0
        end
        2: begin
            fi.color_scale1[fi.current_colors] = !COLOR_42_SCALE
            fi.color_scale2[fi.current_colors] = !NO_SCALE
            fi.color_gamma1[fi.n] = 0
            widget_control,wd_gamma1,SET_VALUE = fi.color_gamma1[fi.n]
            lc_use_2nd_table = !FALSE
            widget_control,wd_tables2,SENSITIVE=0
            widget_control,wd_min2,SENSITIVE=0
            widget_control,wd_max2,SENSITIVE=0
            widget_control,wd_gamma2,SENSITIVE=0
        end
        3: begin
            fi.color_scale1[fi.current_colors] = !COLOR_42_WHITE_SCALE
            fi.color_scale2[fi.current_colors] = !NO_SCALE
            fi.color_gamma1[fi.n] = 0
            widget_control,wd_gamma1,SET_VALUE = fi.color_gamma1[fi.n]
            lc_use_2nd_table = !FALSE
            widget_control,wd_tables2,SENSITIVE=0
            widget_control,wd_min2,SENSITIVE=0
            widget_control,wd_max2,SENSITIVE=0
            widget_control,wd_gamma2,SENSITIVE=0
        end
        4: begin
            fi.color_scale1[fi.current_colors] = !DIFF_SCALE
            fi.color_scale2[fi.current_colors] = !NO_SCALE
            fi.color_gamma1[fi.n] = 0
            widget_control,wd_gamma1,SET_VALUE = fi.color_gamma1[fi.n]
            lc_use_2nd_table = !FALSE
            widget_control,wd_tables2,SENSITIVE=0
            widget_control,wd_min2,SENSITIVE=0
            widget_control,wd_max2,SENSITIVE=0
            widget_control,wd_gamma2,SENSITIVE=0
        end
        5: begin
            fi.color_scale1[fi.current_colors] = !YELLOW_TO_RED
            fi.color_scale2[fi.current_colors] = !NO_SCALE
            fi.color_gamma1[fi.n] = 0
            widget_control,wd_gamma1,SET_VALUE = fi.color_gamma1[fi.n]
            lc_use_2nd_table = !FALSE
            widget_control,wd_tables2,SENSITIVE=0
            widget_control,wd_min2,SENSITIVE=0
            widget_control,wd_max2,SENSITIVE=0
            widget_control,wd_gamma2,SENSITIVE=0
        end
        6: begin
            fi.color_scale1[fi.current_colors] = !LINECOLORS
            fi.color_scale2[fi.current_colors] = !NO_SCALE
            fi.color_gamma1[fi.n] = 0
            widget_control,wd_gamma1,SET_VALUE = fi.color_gamma1[fi.n]
            lc_use_2nd_table = !FALSE
            widget_control,wd_tables2,SENSITIVE=0
            widget_control,wd_min2,SENSITIVE=0
            widget_control,wd_max2,SENSITIVE=0
            widget_control,wd_gamma2,SENSITIVE=0
        end
        7: begin
            fi.color_scale1[fi.current_colors] = !OVERLAY_SCALE
            fi.color_scale2[fi.current_colors] = !DIFF_SCALE
            fi.color_gamma1[fi.n] = 2
            fi.color_gamma2[fi.n] = 0
            widget_control,wd_gamma1,SET_VALUE = fi.color_gamma1[fi.n]
            widget_control,wd_gamma2,SET_VALUE = fi.color_gamma2[fi.n]
            widget_control,wd_tables2,SET_VALUE = 4
            lc_use_2nd_table = !TRUE
            widget_control,wd_tables2,/SENSITIVE
            widget_control,wd_min2,/SENSITIVE
            widget_control,wd_max2,/SENSITIVE
            widget_control,wd_gamma2,/SENSITIVE
        end
        else: begin
            print,'Invalid index for primary color scale.'
        end
    endcase
    lcshow = !TRUE
end

wd_tables2: begin
    case ev.value of
        0: fi.color_scale2[fi.current_colors] = !GRAY_SCALE
        1: fi.color_scale2[fi.current_colors] = !GRAY_SCALE_10
        2: fi.color_scale2[fi.current_colors] = !COLOR_42_SCALE
        3: fi.color_scale2[fi.current_colors] = !COLOR_42_WHITE_SCALE
        4: fi.color_scale2[fi.current_colors] = !DIFF_SCALE
        5: fi.color_scale2[fi.current_colors] = !YELLOW_TO_RED
        6: fi.color_scale2[fi.current_colors] = !LINECOLORS

        7: fi.color_scale2[fi.current_colors] = !RUSS_FIV_NOMID

        else: print,'Invalid index for primary color scale.'
    endcase
    lcshow = !TRUE
end

wd_min1: begin
    widget_control,wd_min1,GET_VALUE=min1
    fi.color_min1[fi.current_colors] = min1
end

wd_max1: begin
    widget_control,wd_max1,GET_VALUE=max1
    fi.color_max1[fi.current_colors] = max1
end

wd_gamma1: begin
    widget_control,wd_gamma1,GET_VALUE=gamma1
    fi.color_gamma1[fi.current_colors] = gamma1
end

wd_min2: begin
    widget_control,wd_min2,GET_VALUE=min2
    fi.color_min2[fi.current_colors] = min2
end

wd_max2: begin
    widget_control,wd_max2,GET_VALUE=max2
    fi.color_max2[fi.current_colors] = max2
end

wd_gamma2: begin
    widget_control,wd_gamma2,GET_VALUE=gamma2
    fi.color_gamma2[fi.current_colors] = gamma2
end

wd_exit: begin
    dsp[fi.cw].adjust_color_top = 0L
    wd_tables1 = 0L
    wd_min1 = 0L
    wd_max1 = 0L
    wd_gamma1 = 0L
    wd_tables2 = 0L
    wd_min2 = 0L
    wd_max2 = 0L
    wd_gamma2 = 0L
    widget_control,ev.top,/DESTROY
end

else: print,'*** Invalid event in adjust_colortable.  ***'

endcase

;if lcshow eq !TRUE then begin
;    load_colortable,fi,dsp
;    showcolors,'Main',wd,dsp,fi
;endif else begin
;    load_colortable,fi,dsp
;endelse

load_colortable,fi,dsp
if lcshow eq !TRUE then showcolors,'Main',wd,dsp,fi 

check_math_errors,!FALSE

;print,'here adjust_colortable_event bottom'

return
end

;**********************************************************************************************
pro compute_lct,table_number,min,max,gamma,color_table,lct,len_colortab,pcolor_lct1,color_index
;**********************************************************************************************

i0 = fix(float(min)*len_colortab/100.)
i1 = fix(float(max)*len_colortab/100.)
if(i1 lt 0) then i1 = 0
if(i0 gt i1) then i0 = i1
lct = intarr(len_colortab,3)
lct_tmp = intarr(len_colortab,3)

; First fill in the color table.
case color_table of
    !GRAY_SCALE: linear_gray_scale,red,green,blue
    !GRAY_SCALE_10: linear_gray_scale_10,red,green,blue
    !COLOR_42_SCALE: color42,red,green,blue
    !COLOR_42_WHITE_SCALE: color42_white,red,green,blue
    !DIFF_SCALE: difference_scale,red,green,blue
    !YELLOW_TO_RED: yellow_to_red,red,green,blue 
    !LINECOLORS: begin
        linecolors,red,green,blue
        if color_index[0] ne -1 then begin
            red = red[color_index]
            green = green[color_index]
            blue = blue[color_index]
        endif
    end
    !OVERLAY_SCALE: linear_gray_scale,red,green,blue
    !NO_SCALE: begin
        if(ptr_valid(pcolor_lct1)) then $
            lct_in = *pcolor_lct1 $
        else $
            return
        red = lct_in[*,0]
        green = lct_in[*,1]
        blue = lct_in[*,2]
    end
    !RUSS_FIV_NOMID: russ_fiv_nomid,red,green,blue
    else: begin
        print,'Invalid value of color_table in compute_lct: ',color_table
        if(ptr_valid(pcolor_lct1)) then $
            lct_in = *pcolor_lct1 $
        else $
            return
        red = lct_in[*,0]
        green = lct_in[*,1]
        blue = lct_in[*,2]
    end
endcase

if color_table ne !LINECOLORS then begin
    ;print,'len_colortab=',len_colortab,' i0=',i0,' i1=',i1
    if i1 gt i0 then begin
        slp = 255./float(i1 - i0)
        indices = slp*findgen(i1 - i0)
        lct_tmp[i0:i1-1,0] = red[indices]
        lct_tmp[i0:i1-1,1] = green[indices]
        lct_tmp[i0:i1-1,2] = blue[indices]
        ;lct[i0:i1-1,0] = red[indices]
        ;lct[i0:i1-1,1] = green[indices]
        ;lct[i0:i1-1,2] = blue[indices]
    endif
    ; Fill in the lower part of the scale.
    if i0 gt 0 then begin
        lct[0:i0-1,0] = red[0]
        lct[0:i0-1,1] = green[0]
        lct[0:i0-1,2] = blue[0]
    endif
    ;Now fill in the upper part.
    if i1 lt len_colortab then begin
        lct[i1:len_colortab-1,0] = red[255]
        lct[i1:len_colortab-1,1] = green[255]
        lct[i1:len_colortab-1,2] = blue[255]
    endif
    if i1 gt i0 then begin
        ; Do gamma correction.
        gamma = 10^((gamma+.1)/10.)
        s = long((i1 - i0)*((findgen(i1 - i0)/float(i1 - i0))^gamma)) + i0 + 1
        lct[i0:i1-1,0] = lct_tmp[s,0]
        lct[i0:i1-1,1] = lct_tmp[s,1]
        lct[i0:i1-1,2] = lct_tmp[s,2]
    endif
endif else begin
nr = n_elements(red)
if nr lt len_colortab then begin
    stat=dialog_message('LINECOLORS has fewer colors then len_colortab!',/ERROR)
    print,'nr=',nr,' len_colortab=',len_colortab
endif
lct[*,0] = red[0:len_colortab-1]
lct[*,1] = green[0:len_colortab-1]
lct[*,2] = blue[0:len_colortab-1]
endelse
;print,'compute_lct bottom'
end

;***********************************
pro linear_gray_scale,red,green,blue
;***********************************
red = indgen(256)
blue = indgen(256)
green = indgen(256)
end

;**************************************
pro linear_gray_scale_10,red,green,blue
;**************************************
red = intarr(256)
blue = intarr(256)
green = intarr(256)
red[0:26] = 0
red[27:51] = 28
red[52:77] = 57
red[78:102] = 85
red[103:128] = 114
red[129:153] = 142
red[154:178] = 171
red[179:204] = 199
red[205:229] = 228
red[230:255] = 255
green[0:26] = 0
green[27:51] = 28
green[52:77] = 57
green[78:102] = 85
green[103:128] = 114
green[129:153] = 142
green[154:178] = 171
green[179:204] = 199
green[205:229] = 228
green[230:255] = 255
blue[0:26] = 0
blue[27:51] = 28
blue[52:77] = 57
blue[78:102] = 85
blue[103:128] = 114
blue[129:153] = 142
blue[154:178] = 171
blue[179:204] = 199
blue[205:229] = 228
blue[230:255] = 255
end

;*******************************
pro overlay_scale,red,green,blue
;*******************************
red = intarr(256)
blue = intarr(256)
green = intarr(256)

r = intarr(20)
g = intarr(20)
b = intarr(20)

r = [ 0, 8, 6, 0, 0, 0, 0, 0, 0, 0, 1, 5, 7, 9,11,13,15,13,13,14]
g = [12, 8, 6,15,13,11, 9, 7, 5, 1, 0, 0, 0, 0, 0, 0, 0, 9,13,14]
b = [12,14,11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 5]

for i=0,235 do begin
    red(i) = i*(255/235.)
    green(i) = i*(255/235.)
    blue(i) = i*(255/235.)
endfor

scl = .01
C = 255/alog(1+scl*235)
for i=0,235 do begin
    red(i) = alog(1+scl*i)*C
    green(i) = alog(1+scl*i)*C
    blue(i) = alog(1+scl*i)*C
endfor

for i=0,19 do begin
    red(235+i) = 17*r(i)
    green(235+i) = 17*g(i)
    blue(235+i) = 17*b(i)
endfor
end

;**********************************
pro difference_scale,red,green,blue
;**********************************
red = intarr(256)
blue = intarr(256)
green = intarr(256)
r = intarr(20)
g = intarr(20)
b = intarr(20)

r = [ 0, 8, 6, 0, 0, 0, 0, 0, 0, 0, 1, 5, 7, 9,11,13,15,13,13,14]
g = [12, 8, 6,15,13,11, 9, 7, 5, 1, 0, 0, 0, 0, 0, 0, 0, 9,13,14]
b = [12,14,11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 5]

for i=0,255 do begin
    j = fix(float(i)/12.8)
    if(j gt 19) then j = 19
    red(i) = 17*r(j)
    green(i) = 17*g(j)
    blue(i) = 17*b(j)
endfor
end

;*************************
pro color42,red,green,blue
;*************************

rgb = intarr(256,3)
red = intarr(256)
blue = intarr(256)
green = intarr(256)

j = 0
for i=j,j+11 do $
    rgb[i,*] = [0,0,0]
j = j + 12
for i=j,j+11 do $
    rgb[i,*] = [102,0,85]
j = j + 12
for i=j,j+11 do $
    rgb[i,*] = [51,51,85]
j = j + 12
for i=j,j+11 do $
    rgb[i,*] = [51,51,119]
j = j + 12
for i=j,j+12 do $
    rgb[i,*] = [85,85,136]
j = j + 13
for i=j,j+12 do $
    rgb[i,*] = [102,102,187]
j = j + 13
for i=j,j+12 do $
    rgb[i,*] = [119,119,204]
j = j + 13
for i=j,j+12 do $
    rgb[i,*] = [136,136,238]
j = j + 13
for i=j,j+12 do $
    rgb[i,*] = [51,102,51]
j = j + 13
for i=j,j+12 do $
    rgb[i,*] = [51,153,51]
j = j + 13
for i=j,j+12 do $
    rgb[i,*] = [85,204,85]
j = j + 13
for i=j,j+12 do $
    rgb[i,*] = [68,238,68]
j = j + 13
for i=j,j+12 do $
    rgb[i,*] = [204,255,34]
j = j + 13
for i=j,j+12 do $
    rgb[i,*] = [238,238,102]
j = j + 13
for i=j,j+12 do $
    rgb[i,*] = [221,221,85]
j = j + 13
for i=j,j+12 do $
    rgb[i,*] = [221,187,68]
j = j + 13
for i=j,j+12 do $
    rgb[i,*] = [221,136,34]
j = j + 13
for i=j,j+12 do $
    rgb[i,*] = [204,102,17]
j = j + 13
for i=j,j+12 do $
    rgb[i,*] = [187,68,0]
j = j + 13
for i=j,j+12 do $
    rgb[i,*] = [255,0,0]

red[*] = rgb[*,0]
green[*] = rgb[*,1]
blue[*] = rgb[*,2]
end

;*******************************
pro color42_white,red,green,blue
;*******************************
rgb = intarr(256,3)
red = intarr(256)
blue = intarr(256)
green = intarr(256)
j = 0
for i=j,j+11 do $
    rgb[i,*] = [0,0,0]
j = j + 12
for i=j,j+11 do $
    rgb[i,*] = [102,0,85]
j = j + 12
for i=j,j+11 do $
    rgb[i,*] = [51,51,85]
j = j + 12
for i=j,j+11 do $
    rgb[i,*] = [51,51,119]
j = j + 12
for i=j,j+12 do $
    rgb[i,*] = [85,85,136]
j = j + 13
for i=j,j+12 do $
    rgb[i,*] = [102,102,187]
j = j + 13
for i=j,j+12 do $
    rgb[i,*] = [119,119,204]
j = j + 13
for i=j,j+12 do $
    rgb[i,*] = [136,136,238]
j = j + 13
for i=j,j+12 do $
    rgb[i,*] = [51,102,51]
j = j + 13
for i=j,j+12 do $
    rgb[i,*] = [51,153,51]
j = j + 13
for i=j,j+12 do $
    rgb[i,*] = [85,204,85]
j = j + 13
for i=j,j+12 do $
    rgb[i,*] = [68,238,68]
j = j + 13
for i=j,j+12 do $
    rgb[i,*] = [204,255,34]
j = j + 13
for i=j,j+12 do $
    rgb[i,*] = [238,238,102]
j = j + 13
for i=j,j+12 do $
    rgb[i,*] = [221,221,85]
j = j + 13
for i=j,j+12 do $
    rgb[i,*] = [221,187,68]
j = j + 13
for i=j,j+12 do $
    rgb[i,*] = [221,136,34]
j = j + 13
for i=j,j+12 do $
    rgb[i,*] = [204,102,17]
j = j + 13
for i=j,j+12 do $
    rgb[i,*] = [187,68,0]
j = j + 13
for i=j,j+12 do $
    rgb[i,*] = [255,255,255]

red[*] = rgb[*,0]
green[*] = rgb[*,1]
blue[*] = rgb[*,2]
end

;*************************
pro color42_old,red,green,blue
;*************************
red = intarr(256)
blue = intarr(256)
green = intarr(256)
red(0:12) = 0
red(13:25) = 102
red(26:37) = 51
red(51:63) = 85
red(64:76) = 102
red(77:88) = 119
red(89:101) = 136
red(102:114) = 51
red(128:139) = 85
red(140:152) = 68
red(153:165) = 204
red(166:177) = 238
red(178:190) = 221
red(217:228) = 204
red(229:241) = 187
red(242:254) = 255
green(0:12) = 0
green(26:37) = 51
green(51:63) = 85
green(64:76) = 102
green(77:88) = 119
green(89:101) = 136
green(77:88) = 119
green(89:101) = 136
green(102:114) = 102
green(115:127) = 153
green(128:139) = 204
green(140:152) = 238
green(153:165) = 255
green(166:177) = 238
green(178:190) = 221
green(191:203) = 187
green(204:216) = 136
green(217:228) = 102
green(229:241) = 68
green(242:254) = 0
green(255) = 88
blue(0:12) = 0
blue(13:25) = 85
blue(38:50) = 119
blue(51:63) = 136
blue(64:76) = 187
blue(77:88) = 204
blue(89:101) = 238
blue(102:114) = 51
blue(128:139) = 85
blue(140:152) = 68
blue(153:165) = 34
blue(166:177) = 102
blue(178:190) = 85
blue(191:203) = 68
blue(204:216) = 34
blue(217:228) = 17
blue(229:255) = 0
end

;*******************************
pro yellow_to_red,red,green,blue
;*******************************
red = intarr(256)
green = intarr(256)
blue = intarr(256)
red[*] = 255
for i=0,255 do green[i] = 255-i
end

;****************************
pro linecolors,red,green,blue
;****************************
cnt = readf_ascii_file(!LINECOLORSFILE)
if cnt.name eq 'ERROR' then begin
    stat=dialog_message('Cannot read '+!LINECOLORSFILE,/ERROR)
    return
end
data = cnt.data
data=reform(data,cnt.NF,cnt.NR,/OVERWRITE) ;NECESSARY if cnt.NR=1
    ;Tried this back in readf_ascii_file, but dimension is truncated after being passed.
    ;Tried sending cnt.dat as ptr_new(cnt.data) but dimension still truncated.
    ;Also need to rename data = cnt.data for dimensions to take.
red = reform(fix(data[1,*]))
green = reform(fix(data[2,*]))
blue = reform(fix(data[3,*]))
end

;********************************
pro russ_fiv_nomid,red,green,blue
;********************************
red = intarr(256)
green = intarr(256)
blue = intarr(256)
red[0:11] = 0 &  green[0:11] = 255 & blue[0:11] = 0            ;rbgyr20_10
red[12:23] = 0 & green[12:23] = 237 & blue[12:23] = 18         ;rbgyr20_31
red[24:35] = 0 & green[24:35] = 213 & blue[24:35] = 42         ;rbgyr20_32
red[36:47] = 0 & green[36:47] = 185 & blue[36:47] = 70         ;rbgyr20_33
red[48:59] = 0 & green[48:59] = 155 & blue[48:59] = 100        ;rbgyr20_34
red[60:71] = 0 & green[60:71] = 123 & blue[60:71] = 132        ;rbgyr20_35
red[72:83] = 0 & green[72:83] = 91 & blue[72:83] = 164         ;rbgyr20_36
red[84:95] = 0 & green[84:95] = 68 & blue[84:95] = 187         ;rbgyr20_37
red[96:107] = 0 & green[96:107] = 36 & blue[96:107] = 219      ;rbgyr20_38
red[108:119] = 0 & green[108:119] = 0 & blue[108:119] = 255    ;rbgyr20_39
red[120:135] = 157 & green[120:135] = 34 & blue[120:135] = 193 ;rbgyr20_21
red[136:147] = 255 & green[136:147] = 0 & blue[136:147] = 0    ;rbgyr20_20
red[148:159] = 255 & green[148:159] = 35 & blue[148:159] = 0   ;rbgyr20_30
red[160:171] = 255 & green[160:171] = 61 & blue[160:171] = 0   ;rbgyr20_29
red[172:183] = 255 & green[172:183] = 92 & blue[172:183] = 0   ;rbgyr20_28
red[184:195] = 255 & green[184:195] = 124 & blue[184:195] = 0  ;rbgyr20_27
red[196:207] = 255 & green[196:207] = 156 & blue[196:207] = 0  ;rbgyr20_26
red[208:219] = 255 & green[208:219] = 188 & blue[208:219] = 0  ;rbgyr20_25
red[220:231] = 255 & green[220:231] = 214 & blue[220:231] = 0  ;rbgyr20_24
red[232:243] = 255 & green[232:243] = 236 & blue[232:243] = 0  ;rbgyr20_23
red[244:255] = 255 & green[244:255] = 255 & blue[244:255] = 0  ;rbgyr20_15
end







;*****************************************
pro set_wd_tables1,color_scale1,wd_tables1
;*****************************************
case color_scale1 of
    !GRAY_SCALE: widget_control,wd_tables1,SET_VALUE=0
    !GRAY_SCALE_10: widget_control,wd_tables1,SET_VALUE=1
    !COLOR_42_SCALE: widget_control,wd_tables1,SET_VALUE=2
    !COLOR_42_WHITE_SCALE: widget_control,wd_tables1,SET_VALUE=3
    !DIFF_SCALE: widget_control,wd_tables1,SET_VALUE=4
    !YELLOW_TO_RED: widget_control,wd_tables1,SET_VALUE=5
    !LINECOLORS: widget_control,wd_tables1,SET_VALUE=6
    !OVERLAY_SCALE: widget_control,wd_tables1,SET_VALUE=7
    !NO_SCALE: ;Do nothing. 
    else: print,'Invalid primary color scale index.'
endcase
end
;*****************************************
pro set_wd_tables2,color_scale2,wd_tables2
;*****************************************
case color_scale2 of
    !GRAY_SCALE: widget_control,wd_tables2,SET_VALUE=0
    !GRAY_SCALE_10: widget_control,wd_tables2,SET_VALUE=1
    !COLOR_42_SCALE: widget_control,wd_tables2,SET_VALUE=2
    !COLOR_42_WHITE_SCALE: widget_control,wd_tables2,SET_VALUE=3
    !DIFF_SCALE: widget_control,wd_tables2,SET_VALUE=4
    !YELLOW_TO_RED: widget_control,wd_tables2,SET_VALUE=5
    !LINECOLORS: widget_control,wd_tables2,SET_VALUE=6

    !RUSS_FIV_NOMID: widget_control,wd_tables2,SET_VALUE=7

    !NO_SCALE: ; Do nothing.
    else: print,'Invalid secondary color scale index.'
endcase
end





;********************
pro adjust_colortable
;********************
common adjust_colortable_comm
common stats_comm

image_index = !D.WINDOW
min1 = 0
max1 = 100
gamma1 = 0.
;;;lc_use_2nd_table = !FALSE
dsp[fi.cw].num_colors = !NUM_COLORS
lct1 = intarr(!LEN_COLORTAB1,3)
if(!LEN_COLORTAB2 gt 0) then $
    lct2 = intarr(!LEN_COLORTAB2,3)


intbase = widget_base(/COLUMN,TITLE='Color Tables',GROUP_LEADER=!FMRI_LEADER,KILL_NOTIFY='load_colortable_dead')
dsp[fi.cw].adjust_color_top = intbase
color_base = widget_base(intbase,/ROW,TITLE='Color Tables')
button_base1 = widget_base(color_base,/COLUMN,/BASE_ALIGN_CENTER,FRAME=3)
button_base2 = widget_base(color_base,/COLUMN,/BASE_ALIGN_CENTER,FRAME=3)
slider_base = widget_base(intbase,/COLUMN)

scrap = ['Gray','Gray 10','Peak red','Peak white','Pos/Neg','Yellow to red','Region colors']
wd_tables1 = cw_bgroup(button_base1,[scrap,'Overlay'],EVENT_FUNC=adjust_colortable_event,COLUMN=1, $
    LABEL_TOP='Primary Color Tables',/EXCLUSIVE,/NO_RELEASE)
set_wd_tables1,fi.color_scale1[fi.current_colors],wd_tables1


;wd_tables2 = cw_bgroup(button_base2,scrap,EVENT_FUNC=adjust_colortable_event,COLUMN=1, $
;    LABEL_TOP='Secondary Color Tables',/EXCLUSIVE,/NO_RELEASE)
wd_tables2 = cw_bgroup(button_base2,[scrap,'Russ fiv nomid'],EVENT_FUNC=adjust_colortable_event,COLUMN=1, $
    LABEL_TOP='Secondary Color Tables',/EXCLUSIVE,/NO_RELEASE)


set_wd_tables2,fi.color_scale2[fi.current_colors],wd_tables2

; Primary color scale.
wd_min1 = cw_fslider(button_base1,Title='Stretch bottom', $
    /DRAG,MINIMUM=0,MAXIMUM=100,XSIZE=256,FORMAT='(i3)', $
    VALUE=fi.color_min1[fi.current_colors],/EDIT)
wd_max1 = cw_fslider(button_base1,Title='Stretch Top', $
    /DRAG,MINIMUM=0,MAXIMUM=100,XSIZE=256,FORMAT='(i3)', $
    VALUE=fi.color_max1[fi.current_colors],/EDIT)
wd_gamma1 = cw_fslider(button_base1,Title='Gamma Correction', $
    /DRAG,MINIMUM=-10,MAXIMUM=10,XSIZE=256,FORMAT='(f6.1)', $
    VALUE = fi.color_gamma1[fi.current_colors],/EDIT)

; Secondary color scale.
wd_min2 = cw_fslider(button_base2,Title='Stretch bottom', $
    /DRAG,MINIMUM=0,MAXIMUM=100,XSIZE=256,FORMAT='(i3)', $
    VALUE=fi.color_min2[fi.current_colors],/EDIT)
wd_max2 = cw_fslider(button_base2,Title='Stretch Top', $
    /DRAG,MINIMUM=0,MAXIMUM=100,XSIZE=256,FORMAT='(i3)', $
    VALUE=fi.color_max2[fi.current_colors],/EDIT)
wd_gamma2 = cw_fslider(button_base2,Title='Gamma Correction', $
    /DRAG,MINIMUM=-10,MAXIMUM=10,XSIZE=256,FORMAT='(f6.1)', $
    VALUE = fi.color_gamma2[fi.current_colors],/EDIT)

if(fi.color_scale2[fi.current_colors] eq !NO_SCALE) then begin
    widget_control,wd_tables2,SENSITIVE=0
    widget_control,wd_min2,SENSITIVE=0
    widget_control,wd_max2,SENSITIVE=0
    widget_control,wd_gamma2,SENSITIVE=0
endif

wd_exit = widget_button(intbase,VALUE='Exit')

; Refresh color bar.
showcolors,'Main',wd,dsp,fi

widget_control,intbase,/REALIZE
xmanager,'adjust_colortable',intbase
wset,image_index
check_math_errors,!FALSE
return
end


;********************
pro reload_colortable
;********************

common stats_comm,wd,wd1,fi,pr,dsp,fl,st,stc,cstm,bltimg, $
       glm,dsp_image,help,pref,first_pass,xm1,ym1,vw

load_colortable,fi,dsp

return
end

;*****************************************************************************
;pro load_colortable,fi,dsp,TABLE1=table1,TABLE2=table2,INIT=init,CODING=coding
;pro load_colortable,fi,dsp,TABLE1=table1,TABLE2=table2,INIT=init,COLOR_INDEX=color_index
pro load_colortable,fi,dsp,color_index,TABLE1=table1,TABLE2=table2,INIT=init
;*****************************************************************************
common adjust_colortable_comm
;print,'load_colortable top'

;if not keyword_set(CODING) then coding = 0
;if not keyword_set(COLOR_INDEX) then color_index=-1 
if n_elements(color_index) eq 0 then color_index=-1 

if n_elements(lc_use_2nd_table) eq 0 then begin
    lc_use_2nd_table = !FALSE
    if!LEN_COLORTAB2 gt 0 then lct2 = intarr(!LEN_COLORTAB2,3)
endif

if(keyword_set(INIT)) then begin
    fi.color_min1[fi.current_colors] = init[0]
    fi.color_max1[fi.current_colors] = init[1]
    fi.color_gamma1[fi.current_colors] = init[2]
    fi.color_min2[fi.current_colors] = init[3]
    fi.color_max2[fi.current_colors] = init[4]
    fi.color_gamma2[fi.current_colors] = init[5]
    if(n_elements(wd_gamma2) gt 0) then begin
        if(widget_info(long(wd_gamma1),/VALID_ID)) then begin
            widget_control,wd_min1,SET_VALUE= init[0]
            widget_control,wd_max1,SET_VALUE= init[1]
            widget_control,wd_gamma1,SET_VALUE= init[2]
        endif
    endif
    if(n_elements(wd_gamma2) gt 0) then begin
        if(widget_info(long(wd_gamma2),/VALID_ID)) then begin
            widget_control,wd_min2,SET_VALUE= init[3]
            widget_control,wd_max2,SET_VALUE= init[4]
            widget_control,wd_gamma2,SET_VALUE= init[5]
        endif
    endif
endif

lc_newcolor = !FALSE
if(keyword_set(TABLE1)) then begin
    lc_newcolor = !TRUE
    if(table1 eq !OVERLAY_SCALE) then begin
        lc_use_2nd_table = !TRUE
        if(n_elements(wd_gamma2) gt 0) then begin
            if(widget_info(long(wd_gamma2),/VALID_ID)) then begin
                widget_control,wd_tables2,/SENSITIVE
                widget_control,wd_min2,/SENSITIVE
                widget_control,wd_max2,/SENSITIVE
                widget_control,wd_gamma2,/SENSITIVE
            endif
        endif
    endif else begin
        lc_use_2nd_table = !FALSE
        if(n_elements(wd_gamma2) gt 0) then begin
            if(widget_info(long(wd_gamma2),/VALID_ID)) then begin
                widget_control,wd_tables2,SENSITIVE=0
                widget_control,wd_min2,SENSITIVE=0
                widget_control,wd_max2,SENSITIVE=0
                widget_control,wd_gamma2,SENSITIVE=0
            endif
        endif
    endelse
    fi.color_scale1[fi.current_colors] = table1
endif

if(keyword_set(TABLE2)) then begin
    lc_newcolor = !TRUE
    fi.color_scale2[fi.current_colors] = table2
endif

if n_elements(wd_tables1) then begin
    if wd_tables1 ne 0 then begin
        set_wd_tables1,fi.color_scale1[fi.current_colors],wd_tables1
    endif
endif
if n_elements(wd_tables2) then begin
    if wd_tables2 ne 0 then begin
        set_wd_tables2,fi.color_scale2[fi.current_colors],wd_tables2
    endif
endif

; Compute color scales.
;compute_lct,1,fi.color_min1[fi.current_colors],fi.color_max1[fi.current_colors],fi.color_gamma1[fi.current_colors], $
;    fi.color_scale1[fi.current_colors],lct1,!LEN_COLORTAB1,fi.color_lct1[fi.current_colors],coding
compute_lct,1,fi.color_min1[fi.current_colors],fi.color_max1[fi.current_colors],fi.color_gamma1[fi.current_colors], $
    fi.color_scale1[fi.current_colors],lct1,!LEN_COLORTAB1,fi.color_lct1[fi.current_colors],color_index

if lc_use_2nd_table eq !TRUE then begin
    ;if !LEN_COLORTAB2 gt 0 then $ 
    ;     compute_lct,1,fi.color_min2[fi.current_colors],fi.color_max2[fi.current_colors],fi.color_gamma2[fi.current_colors], $
    ;        fi.color_scale2[fi.current_colors],lct2,!LEN_COLORTAB2,fi.color_lct2[fi.current_colors],coding
    if !LEN_COLORTAB2 gt 0 then $ 
         compute_lct,1,fi.color_min2[fi.current_colors],fi.color_max2[fi.current_colors],fi.color_gamma2[fi.current_colors], $
            fi.color_scale2[fi.current_colors],lct2,!LEN_COLORTAB2,fi.color_lct2[fi.current_colors],color_index
endif else begin
    ;Set secondary scale to black.
    if(!LEN_COLORTAB2 gt 0) then lct2 = intarr(!LEN_COLORTAB2,3)
endelse

dsp.lct = intarr(!LEN_COLORTAB,3)
dsp.lct[0:!LEN_COLORTAB1-1,*] = lct1
if !LEN_COLORTAB2 gt 0 then dsp.lct[!LEN_COLORTAB1:!LEN_COLORTAB-1,*] = lct2

if(lc_newcolor eq !TRUE) then begin
    if(ptr_valid(fi.color_lct1[fi.current_colors])) then $
        ptr_free,fi.color_lct1[fi.current_colors]
    fi.color_lct1[fi.current_colors] = ptr_new(lct1)
    if(ptr_valid(fi.color_lct2[fi.current_colors])) then $
        ptr_free,fi.color_lct2[fi.current_colors]
    fi.color_lct2[fi.current_colors] = ptr_new(lct2)
endif

linecolors,red,green,blue

dsp.lct[!LEN_COLORTAB:!LEN_COLORTAB+!NUM_LINECOLORS-1,0] = red[0:!NUM_LINECOLORS-1]
dsp.lct[!LEN_COLORTAB:!LEN_COLORTAB+!NUM_LINECOLORS-1,1] = green[0:!NUM_LINECOLORS-1]
dsp.lct[!LEN_COLORTAB:!LEN_COLORTAB+!NUM_LINECOLORS-1,2] = blue[0:!NUM_LINECOLORS-1]

; Write color table to frame buffer.
tvlct,dsp.lct[*,0],dsp.lct[*,1],dsp.lct[*,2]

dsp[fi.cw].color_scale = fi.color_scale1[fi.current_colors]

;print,'load_colortable bottom'
end
