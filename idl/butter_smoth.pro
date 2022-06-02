;Copyright 12/31/99 Washington University.  All Rights Reserved.
;butter_smoth.pro  $Revision: 1.2 $

;****************************************
function butter_smoth,order,Wn,npts,btype
;****************************************
    ;order: filter order
    ;Wn cutoff frequency 0.0 < Wn < 1.0
    ;    1.0 corresponds to half the sampling frequency (0.212 for a TR=2.36s)
    ;npts = 124 frames for most designs

;-------------------------------------------------------------
;BEGIN /usr/local/pkg/matlab5.2/toolbox/signal/butter.m

PI = 3.14159265358979323846

;step 1: get analog, pre-warped frequencies
fs = 2.
u = 2.*fs*tan(PI*Wn/fs)

;step 2: convert to low-pass prototype estimate
;btype: 0=lowpass 1=bandpass 2=highpass 3=bandstop
if (btype eq 0) or (btype eq 2) then begin
    Wn = u[0]
endif else begin
    Bw = u[1] - u[0]
    Wn = sqrt(u[0]*u[1])
endelse

;step 3: Get N-th order Butterworth analog lowpass prototype
;--------------------------------------------------------------
;BEGIN /usr/local/pkg/matlab5.2/toolbox/signal/buttap.m
;Poles are on the unit circle in the left-half plane.
length = fix(order/2.)
imag_p = fltarr(length)
index = 0
for i=1,order-1,2 do begin
    imag_p[index]= PI*i/(2.*order)+PI/2.0
    index = index + 1
end
p = exp(complex(fltarr(length),imag_p))
p = reform([transpose(p),transpose(conj(p))],2*length)
if order mod 2 eq 1 then p = [p,complex(-1,0)]
prod = -p[0]
for i=1,n_elements(p)-1 do prod = prod*(-p[i])
k = float(prod)
;END /usr/local/pkg/matlab5.2/toolbox/signal/buttap.m
;---------------------------------------------------------------

;Transform to state-space
;---------------------------------------------------------------
;BEGIN /usr/local/pkg/matlab5.2/toolbox/matlab/polyfun/zp2ss.m
n = 1
p = p[where(finite(p) eq 1)]
floating_point_precision = machar()
tol = n_elements(p)*norm(p)*floating_point_precision.eps + floating_point_precision.eps

    ;-----------------------------------------------------------
    ;/usr/local/pkg/matlab5.2/toolbox/matlab/elfun/cplxpair.m
    np = n_elements(p)
    z = complexarr(np)
    idx = where(abs(imaginary(p)) le tol*abs(p))
    if idx[0] ne -1 then begin
        z[np-1] = p[idx]
        p = p[0:np-2]
        np = np - 1
    end

    if np gt 0 then begin
        if np mod 2 eq 1 then begin
            stat=widget_message('Odd number of entries remaining. Complex numbers cannot be paired.')
            return,-1
        end
        p = p(sort(float(p)))

        nxt_row = 0
        while np gt 0 do begin
            idx = where(abs(float(p)-float(p[0])) le tol*abs(p))
            nn = n_elements(idx)
            if nn le 1 then begin
                stat=widget_message('Only 1 value found - certainly not a pair. Complex numbers cannot be paired.')
                return,-1
            end
            idx = sort(imaginary(p[idx]))
            xq = p[idx]
            for i=0,nn-1 do z[nxt_row+i] = xq[i]
            nxt_row = nxt_row+nn

            idx = idx[sort(idx)]
            nidx = n_elements(idx)
            if np-nidx gt 0 then begin
                new_p = complexarr(np-nidx)
                ii = 0
                j = 0
                for i=0,np-1 do begin
                    if i ne idx[j] then begin
                        new_p[ii] = p[i]
                        ii = ii + 1
                    end else begin
                        if j ne nidx-1 then j = j + 1
                    end
                end
                p = new_p
                np = n_elements(p)
            end else begin
                np = 0
            end
        end;while
    end
    ;print,'z = ',z
    p = z
    ;------------------------------------------------------------------------------------

c = fltarr(1,1)
c[0,0] = 1
np = n_elements(p)
if np mod 2 eq 1 then begin
    odd_no_of_poles = 1
    a = p[np-1]
    b = 1.
    d = fltarr(1,1)
    np = np - 1
end else begin ;even number of poles
    odd_no_of_poles = 0
    b = 0.;
    d = fltarr(1,1)
    d[0,0] = 1.
endelse

if odd_no_of_poles eq 1 then a = float(a)
t = [[1.,0.],[0.,1.]]
tinv = invert(t)
d1 = fltarr(1,1)

i=0
while i lt np-1 do begin
    e = [p[i],p[i+1]]
    ;-----------------------------------
    ;matlab/polyfun/poly.m
    c_poly=complexarr(3)
    c_poly[0]=complex(1.,0.)
    for j=0,1 do c_poly[1:(j+1)] = c_poly[1:(j+1)] - e[j]*c_poly[0:j]
    ;-----------------------------------
    den = float(c_poly)
    ;print,'den = ',den ;den=1.0000    1.6180    1.0000

    a1 = tinv##[[-den[1],-den[2]],[1.,0.]]##t;
    b1 = tinv##[[1],[0]]
    c1 = [0,1]##t
    ;d1 = 0

    ;print,'a1 = ',a1
    ;print,'b1 = ',b1
    ;print,'c1 = ',c1
    ;print,'d1 = ',d1

    if (odd_no_of_poles eq 1) or (i gt 0) then begin
        size_a = size(a)
        if size_a[0] eq 0 then ma1=1 else ma1=size_a[2]
        ;print,'i = ',i,'        ma1 = ',ma1
    endif

    if (odd_no_of_poles eq 0) and (i eq 0) then begin
        a = a1
        b = b1
        c = c1
        d = d1
    endif else begin
        a = [[a,fltarr(2,ma1)],[b1##c,a1]]
        b = [[b],[b1##d]]
        c = [d1##c,c1]
        d = d1##d
    endelse

    ;print,'a = ',a
    ;print,'b = ',b
    ;print,'c = ',c
    ;print,'d = ',d

    i = i + 2
end
;END /usr/local/pkg/matlab5.2/toolbox/matlab/polyfun/zp2ss.m
;-----------------------------------------------------------
;print,'a = ',a
;print,'b = ',b
;print,'c = ',c
;print,'d = ',d

;step 4: Transform to lowpass, bandpass, highpass, or bandstop of desired Wn
if btype eq 0 then begin
    ;-----------------------------------------------------
    ;START /usr/local/pkg/matlab5.2/toolbox/signal/lp2lp.m
    a = Wn*a
    b = Wn*b
    ;END /usr/local/pkg/matlab5.2/toolbox/signal/lp2lp.m
    ;-----------------------------------------------------
endif else if btype eq 1 then begin
    ;-----------------------------------------------------
    ;START /usr/local/pkg/matlab5.2/toolbox/signal/lp2bp.m
    ma = n_elements(b)
    q = Wn/Bw
    a = Wn*[[a/q,identity(ma)],[-1*identity(ma),fltarr(ma,ma)]]
    b = Wn*[[b/q],[fltarr(1,ma)]]
    c = [c,fltarr(ma)]
    ;END /usr/local/pkg/matlab5.2/toolbox/signal/lp2bp.m
    ;-----------------------------------------------------
end else if btype eq 2 then begin
    ;-----------------------------------------------------
    ;START /usr/local/pkg/matlab5.2/toolbox/signal/lp2hp.m
    at = Wn*invert(a)
    bt = -Wn*(invert(a)##b)
    ct = c##invert(a)
    d = d - c##invert(a)##b
    a = at
    b = bt
    c = ct
    ;END /usr/local/pkg/matlab5.2/toolbox/signal/lp2hp.m
    ;-----------------------------------------------------
end else if btype eq 3 then begin
    ;-----------------------------------------------------
    ;START /usr/local/pkg/matlab5.2/toolbox/signal/lp2bs.m
    ma = n_elements(b)
    q = Wn/Bw
    at = [[Wn/q*invert(a),Wn*identity(ma)],[-Wn*identity(ma),fltarr(ma,ma)]]
    bt = -[[Wn/q*(invert(a)##b)],[fltarr(1,ma)]]
    ct = [c##invert(a),fltarr(ma)]
    d = d - c##invert(a)##b
    a = at
    b = bt
    c = ct
end
    ;print,'a = ',a
    ;print,'b = ',b
    ;print,'c = ',c
    ;print,'d = ',d
    ;print,'size a = ',size(a)
    ;print,'size b = ',size(b)
    ;print,'size c = ',size(c)
    ;print,'size d = ',size(d)

;--------------------------------------------------------
;START /usr/local/pkg/matlab5.2/toolbox/signal/bilinear.m
t = 1./fs
r = sqrt(t)
size_a = size(a)
t1 = identity(size_a[1]) + a*t/2.
t2 = identity(size_a[1]) - a*t/2.
ad = invert(t2)##t1
bd = t/r*(invert(t2)##b)
cd = r*c##invert(t2)
dd = c##invert(t2)##b*t/2. + d

a = ad
b = bd
c = cd
d = dd
;END /usr/local/pkg/matlab5.2/toolbox/signal/bilinear.m
;------------------------------------------------------
;print,'a = ',a
;print,'b = ',b
;print,'c = ',c
;print,'d = ',d

;------------------------------------------------------------
;BEGIN /usr/local/pkg/matlab5.2/toolbox/matlab/polyfun/poly.m
;print,'size(a) = ',size(a)
;print,'a = ',a

e = hqr(elmhes(a));
;print,'e = ',e
n = n_elements(e)
c_poly = complexarr(n+1)
c_poly[0]=complex(1.,0.)
for j=0,n-1 do c_poly[1:(j+1)] = c_poly[1:(j+1)] - e[j]*c_poly[0:j]
;den = float(c_poly)
den = c_poly
;END /usr/local/pkg/matlab5.2/toolbox/matlab/polyfun/poly.m
;----------------------------------------------------------
;print,'den = ',den

;-------------------------------
;START function b = buttnum(n,Wn,den)
Wn = 2*atan(Wn,4)
if btype eq 0 then begin
    r = fltarr(order)
    for i=0,order-1 do r[i] = -1.
    w = 0
endif else if btype eq 1 then begin
    r = [[fltarr(order)],[fltarr(order)]]
    for i=0,order-1 do r[i,0] = 1.
    for i=0,order-1 do r[i,1] = -1.
    w = Wn
end else if btype eq 2 then begin
    r = fltarr(order)
    for i=0,order-1 do r[i] = 1.
    w = PI
end else if btype eq 3 then begin
    imag_r = fltarr(2*order)
    for i=0,2*order-1 do imag_r[i] = Wn*((-1.)^i)
    r = exp(complex(fltarr(2*order),imag_r))
    w = 0
end

;------------------------------------------------------------
;START /usr/local/pkg/matlab5.2/toolbox/matlab/polyfun/poly.m

;print,'r = ',r
;print,'size r = ',size(r)
b = complexarr(n_elements(r)+1)
b[0] = 1.
for j=0,n_elements(r)-1 do b[1:(j+1)] = b[1:(j+1)] - r[j]*b[0:j]

;print,'b = ',b
;END /usr/local/pkg/matlab5.2/toolbox/matlab/polyfun/poly.m
;------------------------------------------------------------


;print,'print_this = ',complex(fltarr(n+1),-w*indgen(n+1))
;return,-1
kern = exp(complex(fltarr(n+1),-w*indgen(n+1)))
;print,'kern = ',kern
;return,-1

num = transpose(float(b##(kern##transpose(den))##invert(kern##transpose(b))))
;print,'num = ',num
;return,-1
den = float(den)

;print,'num = ',num
;print,'den = ',den

;-----------------------------------------------------
;START /usr/local/pkg/matlab5.2/toolbox/signal/freqz.m
na = n_elements(den)
nb = n_elements(num)
;w = findgen(npts)
;w = w*2.*PI/npts/2.
;print,'w = ',w
h = fft([num,fltarr(2.*npts-nb)]) / fft([den,fltarr(2.*npts-na)])
;NOTE: excluding the dc term, the second 124 pts are complex conjugates of the
;      first 124 pts


;print,'h = ',h
return,h
end
