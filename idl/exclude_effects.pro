;Copyright 1/9/15 Washington University.  All Rights Reserved.
;exclude_effects.pro  $Revision: 1.3 $
pro exclude_effects,effectlabels,EXCLUDEBASELINE=excludebaseline,EXCLUDEHZ=excludeHz
if keyword_set(EXCLUDEBASELINE) then begin
    index = where(strmatch(effectlabels,'*Baseline*') eq 0,count)
    effectlabels = effectlabels[index]
endif
index = where(strmatch(effectlabels,'*Trend*') eq 0,count)
effectlabels = effectlabels[index]
index = where(strmatch(effectlabels,"*dxR_*") eq 0,count)
effectlabels = effectlabels[index]
index = where(strmatch(effectlabels,"*dxR2_*") eq 0,count)
effectlabels = effectlabels[index]
index = where(strmatch(effectlabels,"*dxR'_*") eq 0,count)
effectlabels = effectlabels[index]
index = where(strmatch(effectlabels,"*dxR'2_*") eq 0,count)
effectlabels = effectlabels[index]
index = where(strmatch(effectlabels,"*dyR_*") eq 0,count)
effectlabels = effectlabels[index]
index = where(strmatch(effectlabels,"*dyR2_*") eq 0,count)
effectlabels = effectlabels[index]
index = where(strmatch(effectlabels,"*dyR'_*") eq 0,count)
effectlabels = effectlabels[index]
index = where(strmatch(effectlabels,"*dyR'2_*") eq 0,count)
effectlabels = effectlabels[index]
index = where(strmatch(effectlabels,"*dzR_*") eq 0,count)
effectlabels = effectlabels[index]
index = where(strmatch(effectlabels,"*dzR2_*") eq 0,count)
effectlabels = effectlabels[index]
index = where(strmatch(effectlabels,"*dzR'_*") eq 0,count)
effectlabels = effectlabels[index]
index = where(strmatch(effectlabels,"*dzR'2_*") eq 0,count)
effectlabels = effectlabels[index]
index = where(strmatch(effectlabels,"*XR_*") eq 0,count)
effectlabels = effectlabels[index]
index = where(strmatch(effectlabels,"*XR2_*") eq 0,count)
effectlabels = effectlabels[index]
index = where(strmatch(effectlabels,"*XR'_*") eq 0,count)
effectlabels = effectlabels[index]
index = where(strmatch(effectlabels,"*XR'2_*") eq 0,count)
effectlabels = effectlabels[index]
index = where(strmatch(effectlabels,"*YR_*") eq 0,count)
effectlabels = effectlabels[index]
index = where(strmatch(effectlabels,"*YR2_*") eq 0,count)
effectlabels = effectlabels[index]
index = where(strmatch(effectlabels,"*YR'_*") eq 0,count)
effectlabels = effectlabels[index]
index = where(strmatch(effectlabels,"*YR'2_*") eq 0,count)
effectlabels = effectlabels[index]
index = where(strmatch(effectlabels,"*ZR_*") eq 0,count)
effectlabels = effectlabels[index]
index = where(strmatch(effectlabels,"*ZR2_*") eq 0,count)
effectlabels = effectlabels[index]
index = where(strmatch(effectlabels,"*ZR'_*") eq 0,count)
effectlabels = effectlabels[index]
index = where(strmatch(effectlabels,"*ZR'2_*") eq 0,count)
effectlabels = effectlabels[index]

;START150129
index = where(strmatch(effectlabels,"*fidllpf*") eq 0,count)
effectlabels = effectlabels[index]

if keyword_set(EXCLUDEHZ) then begin
    index = where(strmatch(effectlabels,"*Hz*") eq 0,count)
    effectlabels = effectlabels[index]
endif
end
