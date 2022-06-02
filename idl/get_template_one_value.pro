;pro/get_template_one_value.pro0000664000176200017620000001751412073020362021154 0ustar  hg-idl_sourcehg-idl_source;
; Copyright (c) 1998, Forschungszentrum Juelich GmbH ICG-1
; All rights reserved.
; Unauthorized reproduction prohibited.
; This software may be used, copied, or redistributed as long as it is not
; sold and this copyright notice is reproduced on each copy made.  This
; routine is provided as is without any express or implied warranties
; whatsoever.
;
;+
; NAME:
;	get_template_one_value
;
; PURPOSE:
;	The result of this function is a string with template information
;
; CATEGORY:
;   CASE_TOOLS
;
; CALLING SEQUENCE:
;   result=get_template_one_value(file_name,attribute,[/declaration])
;
; INPUTS:
;   file_name: the name of the idl source file
;   attribute: the attribute of the template to read
;
; KEYWORD PARAMETERS:
;   declaration: the attribute to add to the output string
;   translate_description_to_purpose:  some external authors are using DESCRIPTION: instead of PURPOSE:.
;                                      if this key is set DESCRIPTION: is translated to PURPOSE:
;   translate_routine_to_name: some external authors are using ROUTINE: instead of NAME:.
;                                      if this key is set ROUTINE: is translated to NAME:
;
; OUTPUTS:
;	result: the readed template information behind the attribute
;
; EXAMPLE:
;   print,get_template_one_value(file_which('get_template_one_value.pro'),'CALLING SEQUENCE')
;   result=get_template_one_value(file_name,attribute,[/declaration])
;
;
; MODIFICATION HISTORY:
; 	Written by	R.Bauer (ICG-1), 1998-Aug-30
;   1998-Oct-26 mistake removed for using the last declaration before ; -
;   1999-Nov-6 a bit smaller now
;              embedded bytes2strarr,arr2string,filesize
;   2000-Jan-23 : attribute could be an array now
;   2000-Feb-08 : translate_description_to_purpose and translate_routine_to_name added
;   2000-Feb-25 : def_strings implemented, free search of : disabled
;   2000-Apr-04 : file_name could be an array now
;   2002-Jul-15 : bug removed if filesize eq 0
;   2003-Mar-07 : code updated to idl 5.5/5.6
;
;-



FUNCTION get_template_one_value,file_name,attribute,$
   declaration=declaration,$
   translate_description_to_purpose=translate_description_to_purpose ,$
   translate_routine_to_name=translate_routine_to_name
   
   IF N_PARAMS(0) LT 2 THEN BEGIN
      MESSAGE,call_help(),/cont
      RETURN,-1
      help_open: MESSAGE,'File: '+file_name[0]+' does NOT exist',/cont
      RETURN,-1
   ENDIF
   
   n_files=N_ELEMENTS(file_name)
   n_attribute=N_ELEMENTS(attribute)
   result=MAKE_ARRAY(n_attribute,n_files,/STRING)
   FOR k=0,n_files-1 DO BEGIN
      
      def_strings='; '+$
                  ['NAME:',$
                  'AUTHOR:',$
                  'PURPOSE:',$
                  'CATEGORY:',$
                  'CALLING SEQUENCE:',$
                  'INPUTS:',$
                  'OPTIONAL INPUTS:',$
                  'KEYWORD PARAMETERS:',$
                  'OUTPUTS:',$
                  'OPTIONAL OUTPUTS:',$
                  'COMMON BLOCKS:',$
                  'SIDE EFFECTS:',$
                  'RESTRICTIONS:',$
                  'PROCEDURE:',$
                  'EXAMPLE:',$
                  'MODIFICATION HISTORY:']
      
      n_def_strings=N_ELEMENTS(def_strings)
      
;n_attribute=N_ELEMENTS(attribute)
;result=MAKE_ARRAY(n_Attribute,/STRING)
      IF file_test(file_name[k]) THEN BEGIN
         
         in_file_name=file_name[k]
         
         IF STRPOS(in_file_name,'.pro') EQ -1 THEN in_file_name=in_file_name+'.pro'
         
         OPENR,lun,in_file_name,err=err,/GET_LUN
         
         
         IF err NE 0 THEN GOTO,help_open
         stats = FSTAT(lun)
         file_size=stats.size
         IF N_ELEMENTS(lun) GT 0 THEN FREE_LUN,lun
         IF file_size EQ 0 THEN BEGIN
            FREE_LUN,lun
            RETURN,-1
         ENDIF
         bytes=BYTARR(file_size)
         OPENR,lun,   in_file_name,/GET_LUN
         READU,lun,bytes
         FREE_LUN,lun
         
         b2=bytes
         
         idx=WHERE(bytes EQ 13B, cdx)
         IF cdx EQ N_ELEMENTS(b2) THEN RETURN, ''
         IF cdx GT 0 THEN b2[idx]=0b
         
         idx = WHERE(b2 EQ 10B, count_line)
         IF count_line EQ 0 THEN RETURN, STRING(b2)
         
;IF KEYWORD_SET(remove_10b) THEN b2[idx]=0b
         il=[-1, idx]
         text=STRARR(count_line)
         FOR i=0L,count_line-1 DO BEGIN
            text[i]=STRING(b2[il[i]+1:il[i+1]])
         ENDFOR
         
         
         ende=(WHERE(STRPOS(STRTRIM(text,2),';-') EQ 0,count_ende))[0]
         IF count_ende EQ 1 THEN text=STRTRIM(STRCOMPRESS(text[0:ende+1]),2)
         
         
         IF KEYWORD_SET(translate_description_to_purpose) THEN text=replace_string(text,'DESCRIPTION:','PURPOSE:')
         IF KEYWORD_SET(translate_routine_to_name) THEN text=replace_string(text,'ROUTINE:','NAME:')
         dummy=TEMPORARY(ende)
         
         
         FOR ii=0,n_attribute-1 DO BEGIN
            
            in_attribute=STRCOMPRESS(STRTRIM(STRUPCASE(attribute[ii]),2))
            
            
            IF STRPOS(in_attribute,':') EQ -1 THEN in_attribute=in_attribute+':'
            
            found=(WHERE(STRPOS(STRCOMPRESS(STRUPCASE(text),/remove_all),STRCOMPRESS(in_attribute,/remove_all)) GT 0,count_found))(0)
            
            
            i_def=WHERE(def_strings NE '; '+in_attribute,count_i_def)
            IF count_i_def GT 0 THEN in_def_strings=def_strings[i_def]
            n_in_def_strings=N_ELEMENTS(in_def_strings)
            IF count_found GT 0 THEN BEGIN
; count_found=0
               nn=0
               WHILE  nn LT n_in_def_strings DO BEGIN
                  IF SIZE(/N_ELEMENTS,ende) EQ 0 THEN ende=WHERE(STRPOS(text[found+1:*],(in_def_strings[nn])[0]) EQ 0,count_found) ELSE $
                                                           ende=[ende,WHERE(STRPOS(text[found+1:*],(in_def_strings[nn])[0]) EQ 0,count_found)]
                  nn=nn+1
               ENDWHILE
               l_idx=WHERE(ende NE -1,count_found)
               IF count_found GT 0 THEN ende=MIN(ende[l_idx])
;leider gibt es Leute die das template abaendern
               
               
               IF count_found EQ 0 THEN  ende=WHERE(STRPOS(text[found+1:*],':') GT 0,count_found)
               
               IF count_found EQ 0 THEN  ende=WHERE(STRPOS(text[found+1:*],';-') GT -1,count_found) ; fuer das letzte vor ;-
               
               
               test_text = text[found+ende+1]
               last_char = STRPOS(test_text,':')
               
               IF last_char[0] EQ -1 THEN last_char = STRPOS(test_text,';-') ; fuer das letzte vor ;-
               
               n_test_text = N_ELEMENTS(test_text)-1
               FOR i=0,n_test_text DO $
                     IF N_ELEMENTS(test_byte) EQ 0 THEN test_byte=BYTE(STRMID(test_text[i],last_char[i]-1,1)) ELSE $
                     test_byte=[test_byte , BYTE(STRMID(test_text[i],last_char[i]-1,1))]
               
               upper = (WHERE(test_byte LT 90b,count_test_byte))[0]
               IF count_test_byte GT 0 THEN ende =(ende)[upper] ELSE ende = (ende)[0]
               
               IF ende LE 0 THEN ende=1 ;EQ
               str=STRTRIM(STRMID(STRTRIM(text[found+1:ende+found],2),1,1000),2)
               IF N_ELEMENTS(str) GT 1 THEN result[ii,k]=STRTRIM(replace_string(str[0],STRING(10b),''),2)+STRING(10b) ELSE result[ii,k]=STRTRIM(replace_string(str[0],STRING(10b),''),2)
               IF N_ELEMENTS(str) GT 1 THEN FOR i=1,N_ELEMENTS(str)-1 DO BEGIN
                  IF STRTRIM(str[i],2) NE STRING(10b) AND STRTRIM(str[i],2) NE '' THEN $
                  result[ii,k]=result[ii,k]+STRING(10b)+STRTRIM(str[i],2)
                  
               ENDFOR
            ENDIF
            IF N_ELEMENTS(ende) GT 0 THEN dummy=TEMPORARY(ende)
         ENDFOR
         IF KEYWORD_SET(declaration) THEN result = STRTRIM(STRUPCASE(attribute),2)+STRING(10b)+result
         
      ENDIF
   ENDFOR
   IF N_ELEMENTS(result) EQ 1 THEN result=(result)[0]
   RETURN,result
   
END
