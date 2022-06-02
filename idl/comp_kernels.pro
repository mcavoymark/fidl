;Copyright 12/13/00 Washington University.  All Rights Reserved.
;comp_kernels.pro  $Revision: 1.9 $
;***************
pro comp_kernels
;***************

;common stats_comm,wd,wd1,fi,pr,dsp,fl,st,stc,cstm,bltimg, $
;       glm,dsp_image,help,pref,first_pass,roi,xm1,ym1,vw
common stats_comm,wd,wd1,fi,pr,dsp,fl,st,stc,cstm,bltimg, $
       glm,dsp_image,help,pref,first_pass,xm1,ym1,vw


@stats.h
@stats_init.h
stats_init,dsp,fi,pr,fl,st,wd,pref

fwhm = [1.,2.,3.,4.,5.,6.]

;print,'111'
;print,'---'
;xdim = 176L
;ydim = 208L
;zdim = 176L
;for i=0,n_elements(fwhm)-1 do begin
;    image = fltarr(xdim,ydim,zdim)
;    image[88,104,88] = 1.
;    image_smoth = gauss_smoth(image,fwhm[i])
;    print,'fwhm = ',fwhm[i]
;    print,'kernel = ',sqrt(total(image_smoth*image_smoth))
;endfor

print,'222'
print,'---'
xdim = 128L
ydim = 128L
zdim = 75L
for i=0,n_elements(fwhm)-1 do begin
    image = fltarr(xdim,ydim,zdim)
    image[64,64,37] = 1.
    image_smoth = gauss_smoth(image,fwhm[i])
    ;print,'fwhm = ',fwhm[i]
    ;print,'kernel = ',sqrt(total(image_smoth*image_smoth))
    print,'fwhm = ',fwhm[i],' kernel = ',sqrt(total(image_smoth*image_smoth))
endfor

print,''
print,'333'
print,'---'
xdim = 48L
ydim = 64L
zdim = 48L
for i=0,n_elements(fwhm)-1 do begin
    image = fltarr(xdim,ydim,zdim)
    image[24,32,24] = 1.
    image_smoth = gauss_smoth(image,fwhm[i])
    ;print,'fwhm = ',fwhm[i]
    ;print,'    kernel = ',sqrt(total(image_smoth*image_smoth))
    print,'fwhm = ',fwhm[i],'    kernel = ',sqrt(total(image_smoth*image_smoth))
    ;print,'    total(image_smoth*image_smoth) = ',total(image_smoth*image_smoth)
    ;index = where(image_smoth ne 0.,count)
    ;print,'    count=',count
    ;print,'    index=',index
    ;print,'    image_smoth[index]=',image_smoth[index]
    ;print,'    image_smoth[24,32,24]=',image_smoth[24,32,24]
    ;print,'    total(image_smoth[index])=',total(image_smoth[index])
endfor









;This is not right because you no longer have an impulse function when you transform to atlas.
;t4_dir = '/home/hannah/mcavoy/gordon/data_cn_data8_gordon/t4s/'
;t4_files = strarr(22)
;t4_files[0] = t4_dir + 'motionst19_anat_ave_to_711-2A_t4'
;t4_files[1] = t4_dir + 'motionst20_anat_ave_to_711-2A_t4' 
;t4_files[2] = t4_dir + 'motionst21_anat_ave_to_711-2A_t4' 
;t4_files[3] = t4_dir + 'motionst22_anat_ave_to_711-2A_t4' 
;t4_files[4] = t4_dir + 'motionst23_anat_ave_to_711-2A_t4' 
;t4_files[5] = t4_dir + 'motionst24_anat_ave_to_711-2A_t4' 
;t4_files[6] = t4_dir + 'motionst25_anat_ave_to_711-2A_t4' 
;t4_files[7] = t4_dir + 'motionst26_anat_ave_to_711-2A_t4' 
;t4_files[8] = t4_dir + 'motionst27_anat_ave_to_711-2A_t4' 
;t4_files[9] = t4_dir + 'motionst28_anat_ave_to_711-2A_t4' 
;t4_files[10] = t4_dir + 'motionst29_anat_ave_to_711-2A_t4' 
;t4_files[11] = t4_dir + 'motionst30_anat_ave_to_711-2A_t4' 
;t4_files[12] = t4_dir + 'motionst31_anat_ave_to_711-2A_t4' 
;t4_files[13] = t4_dir + 'motionst32_anat_ave_to_711-2A_t4' 
;t4_files[14] = t4_dir + 'motionst33_anat_ave_to_711-2A_t4' 
;t4_files[15] = t4_dir + 'motionst34_anat_ave_to_711-2A_t4' 
;t4_files[16] = t4_dir + 'motionst37_anat_ave_to_711-2A_t4' 
;t4_files[17] = t4_dir + 'motionst38_anat_ave_to_711-2A_t4' 
;t4_files[18] = t4_dir + 'motionst39_anat_ave_to_711-2A_t4' 
;t4_files[19] = t4_dir + 'motionst40_anat_ave_to_711-2A_t4' 
;t4_files[20] = t4_dir + 'motionst41_anat_ave_to_711-2A_t4' 
;t4_files[21] = t4_dir + 'motionst42_anat_ave_to_711-2A_t4' 
;
;xdim = 64L
;ydim = 64L
;zdim = 16L
;lpxy = 3.75 
;lpz  = 8. 
;for i=0,n_elements(fwhm)-1 do begin
;    print,'fwhm = ',fwhm[i]
;    for j=0,n_elements(t4_files)-1 do begin
;        print,t4_files[j]
;        image = fltarr(xdim,ydim,zdim)
;        image[32,32,8] = 1.
;
;        twoAis0_twoBis1_unknown2 = 2.
;        passtype = bytarr(2)
;        passtype[0] = 1
;        passtype[1] = 0
;        stat=call_external(!SHARE_LIB,'_twoA_or_twoB',t4_files[j],twoAis0_twoBis1_unknown2,VALUE=passtype)
;        if twoAis0_twoBis1_unknown2 eq 2. then begin
;            stat = widget_message('t4 filename is unrecognizable. Must include either 2A or 2B, but not both.',/ERROR)
;            return
;        endif
;
;        openr,lu,t4_files[j],/GET_LUN
;        line = ''
;        repeat begin
;            readf,lu,line
;        endrep until (strpos(line,'t4') eq 0)
;        t4 = fltarr(16)
;        ioff = 0
;        for k=0,3 do begin
;            readf,lu,line
;            line = strcompress(strtrim(line,2))
;            fstr = str_sep(line,' ',/REMOVE_ALL)
;            for m=0,3 do begin
;                t4[m+ioff] = float(fstr[m])
;            endfor
;            ioff = ioff + 4
;        endfor
;        close,lu
;        free_lun,lu
;
;        t4img = fltarr(!ATLAS_222_X,!ATLAS_222_Y,!ATLAS_222_Z)
;        passtype = bytarr(9)
;        passtype[0] = 0
;        passtype[1] = 0
;        passtype[2] = 0
;        passtype[3] = 1
;        passtype[4] = 1
;        passtype[5] = 1
;        passtype[6] = 0
;        passtype[7] = 0
;        passtype[8] = 1
;        stat=call_external(!SHARE_LIB,'_t4_atlas',image,t4img,t4,xdim,ydim,zdim,lpxy,lpz,fix(twoAis0_twoBis1_unknown2),VALUE=passtype)
;        
;        mask = t4img eq !UNSAMPLED_VOXEL
;        t4img = t4img*(1-mask)
;
;        image_smoth = gauss_smoth(t4img,fwhm[i])
;        print,'kernel = ',sqrt(total(image_smoth*image_smoth))
;        print,' '
;    endfor
;endfor

end
