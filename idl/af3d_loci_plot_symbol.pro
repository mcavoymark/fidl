;Copyright 9/20/02 Washington University.  All Rights Reserved.
;af3d_loci_plot_symbol.pro  $Revision: 1.3 $

;**********************************
pro af3d_loci_plot_symbol,af3dh,win
;**********************************
;print,'af3d_loci_plot_symbol top'

index = where(win.af3d_checked,count)
if count ne 0 then begin
    for i=0,count-1 do begin
        if win.naf3dpts_new[index[i]] gt 0 then begin
            af3d = win.af3d_new[index[i]]
            if af3d.psym eq 8 then begin
                case af3d.shape of
                    'square': begin
                        xs = [1,-1,-1, 1]
                        ys = [1, 1,-1,-1]
                    end
                    'circle': begin
                        a = findgen(33) * (!PI*2/32.)
                        xs = cos(a)
                        ys = sin(a)
                    end
                    'diamond': begin
                        xs = [1,0,-1, 0,1]
                        ys = [0,1, 0,-1,0]
                    end
                    'trianglen': begin
                        xs = [ 1,0,-1, 1]
                        ys = [-1,1,-1,-1]
                    end
                    'triangles': begin
                        xs = [1,-1, 0,1]
                        ys = [1, 1,-1,1]
                    end
                    'trianglew': begin
                        xs = [1,-1, 1,1]
                        ys = [1, 0,-1,1]
                    end
                    'trianglee': begin
                        xs = [1,-1,-1,1]
                        ys = [0, 1,-1,0]
                    end
                    else: stat = dialog_message('Problem2 with af3d.shape.',/ERROR)
                endcase
                ;THICK must be here for it to work.
                if af3d.fill eq 'unfilled.m.pm' then $
                    usersym,xs,ys,THICK=af3d.thick $
                else $
                    usersym,xs,ys,THICK=af3d.thick,/FILL
            endif
            vp = *win.af3dpts_new[index[i]]
            ;THICK still needs to be here for the + and x.
            for j=0,win.naf3dpts_new[index[i]]-1 do plots,vp[0,j],vp[1,j],PSYM=af3d.psym,COLOR=af3d.color_index+!WHITE, $
                /DEVICE,SYMSIZE=af3d.size,THICK=af3d.thick
        endif
    endfor
endif

;print,'af3d_loci_plot_symbol bottom'
end
