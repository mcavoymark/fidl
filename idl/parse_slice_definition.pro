;Copyright 12/31/99 Washington University.  All Rights Reserved.
;parse_slice_definition.pro  $Revision: 1.4 $

;*************************************
function parse_slice_definition,string
;*************************************

; Format: x,y1...y2\dy,z

left_paren = strpos(string,'(')
right_paren = strpos(string,')')
xyz = str_sep(strmid(string,left_paren+1,right_paren-left_paren-1),',',/TRIM)

px = strpos(xyz[0],'...')
py = strpos(xyz[1],'...')
pz = strpos(xyz[2],'...')
if(px gt 0) then begin
    x0 = float(strmid(xyz[0],0,px))
    pslash = strpos(xyz[0],'\')
    if(pslash lt 0) then $
        ps = strlen(xyz[0]) $
    else $
        ps = pslash
    x1 = float(strmid(xyz[0],px+3,ps-px-3))
    if(pslash gt 0) then $
        dx = float(strmid(xyz[0],pslash+1,strlen(xyz[0])-pslash)) $
    else $
        dx = 1
    y0 = float(xyz[1])
    y1 = 0.
    dy = 0.
    z0 = float(xyz[2])
    z1 = 0.
    dz = 0.
    view = !SAGITAL
endif else if(py gt 0) then begin
    x0 = float(xyz[0])
    x1 = 0.
    dx = 0.
    y0 = float(strmid(xyz[1],0,py))
    pslash = strpos(xyz[1],'\')
    if(pslash lt 0) then $
        ps = strlen(xyz[0]) $
    else $
        ps = pslash
    y1 = float(strmid(xyz[1],py+3,ps-py-3))
    if(pslash gt 0) then $
        dy = float(strmid(xyz[1],pslash+1,strlen(xyz[1])-pslash)) $
    else $
        dy = 1
    z0 = float(xyz[2])
    z1 = 0.
    dz = 0.
    view = !CORONAL
endif else if(pz gt 0) then begin
    x0 = float(xyz[0])
    x1 = 0.
    dx = 0.
    y0 = float(xyz[1])
    y1 = 0.
    dy = 0.
    z0 = float(strmid(xyz[2],0,pz))
    pslash = strpos(xyz[2],'\')
    if(pslash lt 0) then $
        ps = strlen(xyz[0]) $
    else $
        ps = pslash
    z1 = float(strmid(xyz[2],pz+3,ps-pz-3))
    if(pslash gt 0) then $
        dz = float(strmid(xyz[2],pslash+1,strlen(xyz[2])-pslash)) $
    else $
        dz = 1
     view = !TRANSVERSE
endif else begin
    print,'A range must be specified for at least one dimension.'
    print,'Valid formats are "x0...x1\dx,y,z", "x,y0...y1\dy,z" or "x,y,z0...z1\dz"
    print,' In each case, *0 is the starting coordinate, *1 is the ending coordinate.'
    print,' and d* is the increment between coordinates.
    return,-1
endelse

;;;print,x0,x1,dx
;;;print,y0,y1,dy
;;;print,z0,z1,dz
;;;help,xyz,x0,x1,dx,px,pslash

xx = {name:'',x0:x0,x1:x1,dx:dx,y0:y0,y1:y1,dy:dy,z0:z0,z1:z1,dz:dz,view:view}

return,xx
end
