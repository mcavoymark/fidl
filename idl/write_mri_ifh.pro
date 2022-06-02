;Copyright 12/31/99 Washington University.  All Rights Reserved.
;write_mri_ifh.pro  $Revision: 12.83 $

;******************************
pro write_mri_ifh,file_name,ifh
;******************************

openw,lu,file_name,/GET_LUN
put_mri_ifh,lu,ifh,/IFH_ONLY
close,lu
free_lun,lu

end
