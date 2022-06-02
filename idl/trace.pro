;$Id: trace.pro,v 12.82 1999/12/16 20:48:59 jmo Exp $
function trace,G

info = size(G)
if(info(0) ne 2) then return,-1

xdim = info(1)
ydim = info(2)
if(xdim ne ydim) then return,-1

trace = 0.
for i=0,xdim-1 do $
    trace = trace + G(i,i)

return,trace
end
