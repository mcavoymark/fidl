;pro/replace_tagvalue.pro0000664000176200017620000001605612073020341017745 0ustar  hg-idl_sourcehg-idl_source
; Copyright (c) 2000, Forschungszentrum Juelich GmbH ICG-1
; All rights reserved.
; Unauthorized reproduction prohibited.
; This software may be used, copied, or redistributed as long as it is not
; sold and this copyright notice is reproduced on each copy made.  This
; routine is provided as is without any express or implied warranties
; whatsoever.
;
;+
;
; USERLEVEL:
;   TOPLEVEL
;
; NAME:
;    replace_tagvalue
;
; PURPOSE:
;   This functions replaces a tag in a structure or sub structure
;
; CATEGORY:
;   PROG_TOOLS/STRUCTURES
;
; CALLING SEQUENCE:
;   result = replace_tagvalue(struct, tagname, new_value [, NEW_TAGNAME = new_tagname, sub_structure = sub_structure])
;
; INPUTS:
;   struct: the structure where to replace a tag
;   tagname: the name of the tag to replace
;   new_value: the value of the new tag
;
; KEYWORD PARAMETERS:
;   new_tagname: A new name for the tag
;   sub_structure: to define the sub structure where a tag should be replaced
;         If not given tags on the main level will be replaced.
;   /ALL: If set all tags which agree to tagname will be replaced starting from the
;         main level or the level 'sub_structure' downwards.
;
; OUTPUTS:
;   This function returns a structure with some tags replaced.
;
; EXAMPLE:
;   d = {A: 1, B: {B1: 0, B2: 1}, C: {B1: 0, B2: 1}}
;
;   help, replace_tagvalue(d, 'A', 2, NEW_TAGNAME = 'CC'),/str
;   ** Structure <1056ea8>, 3 tags, length=10, refs=1:
;   CC              INT              2
;   B               STRUCT    -> <Anonymous> Array[1]
;   C               STRUCT    -> <Anonymous> Array[1]
;
;   result = replace_tagvalue(d, 'A', {A:2})
;   HELP, result.a, /STR
;   ** Structure <133aa58>, 1 tags, length=2, refs=2:
;   A               INT              2
;
;   result = replace_tagvalue(d, 'B1', {A:2}, NEW_TAGNAME = 'I', /ALL)
;   help,result.b,/str
;   ** Structure <1055248>, 2 tags, length=4, refs=2:
;   I               STRUCT    -> <Anonymous> Array[1]
;   B2              INT              1
;
;
;   result = replace_tagvalue(d, 'B1', {A:2}, sub='B')
;   HELP, result.b, /STR
;   ** Structure <13243f8>, 2 tags, length=4, refs=2:
;   B1              STRUCT    -> <Anonymous> Array[1]
;   B2              INT              1
;   HELP, result.b.b1, /STR
;   ** Structure <1369998>, 1 tags, length=2, refs=2:
;   A               INT              2
;
; MODIFICATION HISTORY:
;       Written by:     R.Bauer (ICG-1), 2000-Jan-05
;       14.03.2000: Header edited
;       07.03.2001 : bug removed for the example below
;                    a={peter:{a:1},clus:2}
;                    b=replace_tagvalue(a,'peter',5)
;                  in the prior version the sub structure peter wasn't changed to a tag
;
;-


FUNCTION replace_tagvalue,struct,tagname,new_tagname=new_tagname,new_value,sub_Structure=sub_Structure,sort=sort,all=all
   IF N_PARAMS() GE 1 THEN BEGIN
      if n_elements(sub_Structure) eq 0 and is_tag(struct,tagname) eq 1 then begin
         n_names=struct2names_and_ptrs(struct,'',names=names,ptr_values=ptr_values)
         ix = (where(strupcase(names) eq strupcase(tagname),count_ix))[0]
         if n_elements(new_tagname) eq 1 then names[ix]=new_tagname
         *ptr_values[ix]=new_value
         return,names_and_ptrs2struct(names,ptr_values)
      endif

      IF N_ELEMENTS(struct) GT 0 THEN in_struct=struct
      IF N_ELEMENTS(tagname) GT 0 THEN in_tagname=tagname
      IF N_ELEMENTS(new_tagname) GT 0 THEN in_new_tagname=new_tagname else in_new_tagname=tagname
      IF N_ELEMENTS(sub_structure) GT 0 THEN in_sub_structure=sub_structure

      ; ============================================================================= same in delete_tag and replace_tagvalue and rename_tag

      IF KEYWORD_SET(all) THEN in_sub_structure=TAG_NAMES(struct)
      IF N_ELEMENTS(in_sub_structure) GT 0 THEN BEGIN

         n=N_ELEMENTS(in_sub_structure)
         FOR ii=0,n-1 DO BEGIN

            IF STRPOS(in_sub_structure[ii],'.') GT -1 THEN BEGIN
               part=STRSPLIT(in_sub_structure[ii],'.', /EXTRACT)
               sub=STRUPCASE(part[0])  ; interne Variablen verwenden
               build_vector,to_add,STRUPCASE(arr2string(part[1:*],sep='.')+'.'+in_tagname)
               ENDIF ELSE BEGIN
               build_vector,sub,STRUPCASE(in_sub_structure[ii])
               IF N_ELEMENTS(in_tagname) GT 0 THEN build_vector,to_add, in_tagname
            ENDELSE
         ENDFOR

         IF N_ELEMENTS(to_add) GT 0 THEN in_tagname=to_Add[uniq(to_add,SORT(to_add))]
         n_names=struct2names_and_ptrs(struct,sub,names=names,ptr_values=ptr_values)
         subs=sub+'.'
         ENDIF ELSE BEGIN
         n_names=struct2names_and_ptrs(struct,names=names,ptr_values=ptr_values)
         subs=''
      ENDELSE
      ms=0b
      IF N_ELEMENTS(in_tagname) EQ 0 THEN dtext=names[strmatch2(names,STRUPCASE(subs)+'*')] ELSE BEGIN

         k=WHERE(STRPOS(in_tagname,'*') GT -1,count)
         IF count GT 0 AND subs[0] NE '' THEN BEGIN

            dtext=names[strmatch2(names,str_matrix(subs,STRUPCASE(in_tagname)))]


            ENDIF ELSE BEGIN

            del_tn=STRUPCASE(str_matrix(subs[0],in_tagname))
            FOR i=1,N_ELEMENTS(subs)-1 DO del_tn=[del_tn,STRUPCASE(str_matrix(subs[i],in_tagname))]
            del_tn=[del_tn,del_tn+'.*'] ; ?
            del_tn=del_tn[uniq(del_tn,SORT(del_tn))]
            idx=strmatch2(names,del_tn,count=c_idx)
            IF c_idx GT 0 THEN  dtext=names[idx] ELSE dtext=''

         ENDELSE
      ENDELSE
      dtext=dtext(UNIQ(dtext))

; =============================================================================

      idx = MAKE_ARRAY(N_ELEMENTS(names),/LONG)
      FOR i = 0L, N_ELEMENTS(names) - 1 DO idx[i] = (WHERE(dtext EQ names[i]))[0]
      idx=WHERE(idx NE -1,count_idx)

      IF MAX(STRPOS(names,in_tagname+'.')) GT -1 THEN BEGIN
         n_idx=LINDGEN(N_ELEMENTS(names))
         rest=indexlogic(n_idx,idx,'NOT')

         IF count_idx GT 0 THEN BEGIN
            rec_ptr_free,ptr_values[idx]
            names=names[rest]
            names=[names,in_new_tagname]
            ptr_values=ptr_values[rest]
            ptr_values=[ptr_values,PTR_NEW(new_value)]
         ENDIF
         ENDIF ELSE BEGIN


         IF count_idx GT 0 THEN BEGIN
            rec_ptr_free,ptr_values[idx]

;Unterschied zu change_value. tag_name wird auch geaendert
            IF subs[0] NE '' THEN BEGIN
               ;names[idx]=replace_string(names[idx],subs+replace_String(STRUPCASE(in_tagname),'*',''),STRUPCASE(subs+in_new_tagname))
               names[idx]=replace_string(names[idx],replace_String(STRUPCASE(in_tagname),'*',''),STRUPCASE(in_new_tagname))
               ENDIF ELSE BEGIN
               names[idx]=replace_string(names[idx],replace_String(STRUPCASE(in_tagname),'*',''),STRUPCASE(in_new_tagname))
            ENDELSE
;
; es muessen einzelne verschiedene Pointer sein!!!!
            FOR i=0,count_idx-1 DO BEGIN
               rec_ptr_free,ptr_values[idx[i]]
               ptr_values[idx[i]]=PTR_NEW(new_value)
            ENDFOR

         ENDIF
      ENDELSE

      result=names_and_ptrs2struct(names,ptr_values,sort=sort)

      RETURN,result
      ENDIF ELSE BEGIN
      MESSAGE,call_help(),/cont
      RETURN,-1
   ENDELSE

END
