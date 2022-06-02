pro white_noise,filnam,xdim,ydim,zdim,tdim

help,filnam,xdim,ydim,zdim,tdim
seed = intarr(3)
rvs = normaln_jmo(long(xdim)*long(ydim)*long(zdim)*long(tdim),seed)

help,rvs
print,max(rvs),min(rvs)
get_lun,lu
openw,lu,filnam
writeu,lu,rvs
close,lu
free_lun,lu

return
end
