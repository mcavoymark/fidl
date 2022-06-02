	FH6 = 	{   MH_File_Info6,			$
			original_file_name:'',		$
			sw_version:0,			$
			data_type:0,			$
			system_type:0,			$
			file_type:0,			$
			node_id:'',			$
			nframes:0,			$
			nplanes:0,			$
			ngates:0,			$
			nbeds:0,			$
			scan_start:intarr(3,2),		$
			isotope_code:'',		$
			isotope_halflife:0.0,		$
			radiopharmaceutical:'',		$		
			facility_name:'',		$
			user_process_code:''		$
		}

	PI6 =	{   MH_Patient_Info6,			$
			patient_info:strarr(11)		$
		}

	SI6 = 	{   MH_Scan_Info6,			$
			gantry:fltarr(2),		$
			bed_elevation:0.0,		$
			rot_source_speed:0,		$
			wobble_speed:0,			$
			transm_source_type:0,		$
			axial_fov:0.0,			$
			transaxial_fov:0.0,		$
			transaxial_samp_mode:0,		$
			coin_samp_mode:0,		$
			axial_samp_mode:0,		$
			calibration_factor:0.0,		$
			calibration_units:0,		$
			compression_code:0,		$
			acquisition_type:0,		$
			bed_type:0,			$
			septa_type:0,			$
			init_bed_position:0.0,		$
			bed_offset:fltarr(15),		$
			plane_separation:0.0,		$
			lwr_sctr_thresh:0,		$
			true_thresh:intarr(2),		$
			collimator:0.0,			$
	 		acquisition_mode:0		$
		}			

	MH6 = 	{   Main_Header6,			$
			file:{MH_File_Info6},		$
			pat:strarr(11),			$
			scan:{MH_Scan_Info6}		$
		}

	SH6 =	{   Scn_Subheader6,			$
			data_type:0,			$
			dimension_1:0,			$
			dimension_2:0,			$
			smoothing:0,			$
			processing_code:0,		$
			sample_distance:0.0,		$
			isotope_halflife:0.0,		$
			frame_duration_sec:0,		$
			gate_duration:0L,		$
			r_wave_offset:0L,		$
			scale_factor:0.0,		$
			scan_min:0,			$
			scan_max:0,			$
			prompts:0L,			$
			delayed:0L,			$
			multiples:0L,			$
			net_trues:0L,			$
			cor_singles:fltarr(16),		$
			uncor_singles:fltarr(16),	$
			tot_avg_cor:0.0,		$
			tot_avg_uncor:0.0,		$
			total_coin_rate:0L,		$
			frame_start_time:0L,		$
			frame_duration:0L,		$
			loss_correction_fctr:0.0,	$
      	                phy_planes:lonarr(8) 		$
		}

	RH6 = 	{   Img_Recon_Info6,			$
			recon_start:intarr(3,2),	$
			recon_duration:0L,		$
			recon_scale:0.0,		$
			filter_code:0,			$
			filter_params:fltarr(6),	$
			scan_matrix_num:0L,		$
			norm_matrix_num:0L,		$
			atten_cor_matrix_num:0L		$
		}


	IH6 =	{   Img_Subheader6,			$
			data_type:0,			$
			num_dimensions:0,		$
			dimension_1:0,			$
			dimension_2:0,			$
			x_origin:0.0,			$
			y_origin:0.0,			$
			quant_scale:0.0,		$
			image_min:0,			$
			image_max:0,			$
			pixel_size:0.0,			$
			slice_width:0.0,		$
			frame_duration:0L,		$
			frame_start_time:0L,		$
			slice_location:0,		$
			image_rotation:0.0,		$
			plane_eff_corr_fctr:0.0,	$
			decay_corr_fctr:0.0,		$
			loss_corr_fctr:0.0,		$
			intrinsic_tilt:0.0,		$
			processing_code:0,		$
			quant_units:0,			$
			ecat_calibration_fctr:0.0,	$
			well_counter_cal_fctr:0.0,	$
			annotation:'',			$
			recon:{Img_Recon_Info6}		$
		}

	NH6 = 	{   Nrm_Subheader6,			$
			data_type:0,			$
			dimension_1:0,			$
			dimension_2:0,			$
			scale_factor:0.0,		$
			norm_hour:0,			$
			norm_minute:0,			$
			norm_second:0,			$
			norm_day:0,			$
			norm_month:0,			$
			norm_year:0,			$
			fov_source_width:0.0,		$
			ecat_calib_factor:0.0		$
		}

	AH6 = 	{   Atn_Subheader6,			$
			data_type:0,			$
			attenuation_type:0,		$
			dimension_1:0,			$
			dimension_2:0,			$
			scale_factor:0.0,		$
			x_origin:0.0,			$
			y_origin:0.0,			$
			x_radius:0.0,			$
			y_radius:0.0,			$
			tilt_angle:0.0,			$
			attenuation_coeff:0.0,		$	
			sample_distance:0.0		$
		}

	PM6 = 	{   Pm_Subheader6,		$
			type:0,			$
			num_rings:0,		$
			sect_per_ring:intarr(32), $
			ring_pos:fltarr(32),	$
			ring_angle:intarr(32),	$
			start_angle:0,		$
			long_axis_left:intarr(3),  $
			long_axis_right:intarr(3), $
			position_data:0,	$
			image_min:0,		$
			image_max:0,		$
			scale_factor:0.0,	$
			x_origin:0.0,		$
			y_origin:0.0,		$
			rotation:0.0,		$
			pixel_size:0.0,		$
			frame_duration:0L,	$
			frame_start_time:0L,	$
			proc_code:0,		$
			quant_units:0,		$
			annotation:''		$
		}

  	SI7 =    {   MH_Scan_Info7,                     $
                        lwr_sctr_thres:0,               $
                        lwr_true_thres:0,               $
                        upr_true_thres:0,               $
                        user_process_code:bytarr(10),   $
                        d1:0,                           $
                        acquisition_mode:0,             $
                        bin_size:0.0,                   $
                        branching_fraction:0.0,         $
                        dose_start_time:0L,             $
                        dosage:0.0,                     $
                        well_counter_corr_factor:0.0,   $
                        data_units:bytarr(32)		$
                }


        GI7 = {  MH_Gantry_Info7,                         $
                        gantry_tilt:0.0,                $
                        gantry_rotation:0.0,            $
                        bed_elevation:0.0,              $
                        intrinsic_tilt:0.0,             $
                        wobble_speed:0,                 $
                        transm_source_type:0,           $
                        distance_scanned:0.0,           $
                        transaxial_fov:0.0,             $
                        angular_compression:0,          $
                        coin_samp_mode:0,               $
                        axial_samp_mode:0,              $
                        d0:0                            $
                }

   	MH7 =    {   Main_Header7,                        $
                        magic_number:bytarr(14),        $
                        original_file_name:bytarr(32),  $
                        sw_version:0,                   $
                        system_type:0,                  $
                        file_type:0,                    $
                        serial_number:bytarr(10),       $
                        d0:0,                           $
                        scan_start_time:0L,             $
                        isotope_name:bytarr(8),         $
                        isotope_halflife:0.0,           $
                        radiopharmaceutical:bytarr(32), $
                        gantry:{MH_Gantry_Info7},        $
                        ecat_calibration_factor:0.0,    $
                        cal_units:0,                    $
                        cal_units_label:0,              $
                        compression_code:0,             $
                        study_name:bytarr(12),          $
                        patient_id:bytarr(16),          $
                        patient_name:bytarr(32),        $
                        patient_sex:' ',                $
                        patient_dexterity:' ',          $
                        patient_age:0.0,                $
                        patient_height:0.0,             $
		        patient_weight:0.0,             $
                        patient_birth_date:0L,          $
                        physician_name:bytarr(32),      $
                        operator_name:bytarr(32),       $
                        study_desc:bytarr(32),          $
                        acquisition_type:0,             $
                        patient_orientation:0,          $
                        facility_name:bytarr(20),       $
                        num_planes:0,                   $
                        num_frames:0,                   $
                        num_gates:0,                    $
                        num_bed_pos:0,                  $
                        init_bed_position:0.0,          $
                        bed_position:fltarr(15),        $
                        plane_separation:0.0,           $
                        septa_state:0.0,           $
                        scan:{MH_Scan_Info7}             $
                }

 	SH7 =    {   Scn_Subheader7,                      $
                        data_type:0,                    $
                        num_dimensions:0,               $
                        num_r_elements:0,               $
                        num_angles:0,                   $
                        corrections_applied:0,          $
                        num_z_elements:0,               $
                        ring_difference:0,              $
			axial_compression:0,		$
                        d0:0,                           $
                        x_resolution:0.0,               $
                        y_resolution:0.0,               $
                        z_resolution:0.0,               $
                        w_resolution:0.0,               $
                        gate_duration:0L,               $
                        r_wave_offset:0L,               $
                        num_acpt_beats:0L,              $ ; num_accepted_beats
                        scale_factor:0.0,               $
                        scan_min:0,                     $
                        scan_max:0,                     $
                        prompts:0L,                     $
                        delayed:0L,                     $
                        multiples:0L,                   $
                        net_trues:0L,                   $
    			cor_singles:fltarr(16),         $
                        uncor_singles:fltarr(16),       $
                        tot_avg_cor:0.0,                $
                        tot_avg_uncor:0.0,              $
                        total_coin_rate:0L,             $
                        frame_start_time:0L,            $
                        frame_duration:0L,              $
                        dt_corr_fctr:0.0,               $ ; deadtime_correction_factor
                        phy_planes:intarr(8)            $
                }

	 RTH7 =   {   Img_Rot_Info7,                 $
                        da_x_rot_angle:0.0,     $
                        da_y_rot_angle:0.0,     $
                        da_z_rot_angle:0.0,     $
                        da_x_trans:0.0,         $
                        da_y_trans:0.0,         $
                        da_z_trans:0.0,         $
                        da_x_scale_f:0.0,       $
                        da_y_scale_f:0.0,       $
                        da_z_scale_f:0.0        $
                }


  	IH7 =    {   Img_Subheader7,              $
                        data_type:0,            $
                        num_dimensions:0,       $
                        x_dimension:0,          $
                        y_dimension:0,          $
                        z_dimension:0,          $
                        d1:0,                   $ ; put on 4-byte boundary
                        z_offset:0.0,           $
                        x_offset:0.0,           $
                        y_offset:0.0,           $
                        recon_zoom:0.0,         $
                        scale_factor:0.0,       $
                        image_min:0,            $
                        image_max:0,            $
                        x_pixel_size:0.0,       $
                        y_pixel_size:0.0,       $
                        z_pixel_size:0.0,       $
                        frame_duration:0L,      $
                        frame_start_time:0L,    $
                        filter_code:0,          $
                        filter_resolution:0,          $
                        d2:0,                   $ ; put on 4-byte boundary
                        x_res:0.0,              $
                        y_res:0.0,              $
          		z_res:0.0,              $
                        x_rot_angle:0.0,        $
                        y_rot_angle:0.0,        $
                        z_rot_angle:0.0,        $
                        dec_corr_fctr:0.0,      $
                        corr_applied:0L,        $
                        gate_duration:0L,       $
                        r_wave_offset:0L,       $
                        num_acpt_beats:0L,      $
                        fil_cutoff_freq:0.0,    $
                        fil_dc_comp:0.0,        $
                        fil_rs:0.0,             $
                        fil_order:0,            $
                        d3:0,                   $
                        fil_sf:0.0,             $
                        fil_scat_slope:0.0,     $
                        annotation:bytarr(40),  $
                        loc:{Img_Rot_Info7}      $
                }

 	NH7 =    {   Nrm_Subheader7,                      $
                        data_type:0,                    $
                        num_dimensions:0,               $
                        num_r_elements:0,               $
                        num_angles:0.0,                 $
                        num_z_elements:0,               $
                        ring_difference:0,              $
                        scale_factor:0.0,               $
                        norm_min:0.0,                   $
                        norm_max:0.0,                   $
                        fov_source_width:0.0,           $
                        norm_quality_factor:0.0,        $
                        norm_qual_fctr_code:0           $
                }


  	AH7 =    {   Atn_Subheader7,                      $
                        data_type:0,                    $
                        num_dimensions:0,               $
                        atten_type:0,                   $
                        num_r_elements:0,               $
                        num_angles:0.0,                 $
                        num_z_elements:0,               $
                        ring_difference:0,              $
                        d0:0,                           $
                        x_resolution:0.0,               $
                        y_resolution:0.0,               $
                        z_resolution:0.0,               $
                        w_resolution:0.0,               $
                        scale_factor:0.0,               $
                        x_offset:0.0,                   $
                        y_offset:0.0,                   $
                        x_radius:0.0,                   $
                        y_radius:0.0,                   $
                        tilt_angle:0.0,                 $
                        atten_coeff:0.0,                $
                        atten_min:0.0,                  $
                        atten_max:0.0,                  $
                        skull_thickness:0.0,            $
       			num_add_coeff:0,                $
                        d1:0,                           $
                        add_atten_coeff:fltarr(8),      $
                        edge_finding_threshold:0.0      $
                }


      PM7 =    {   Pm_Subheader7,               $
                        data_type:0,            $
                        type:0,                 $
                        num_rings:0,            $
                        sect_per_ring:intarr(32), $
                        ring_pos:fltarr(32),    $
                        ring_angle:intarr(32),  $
                        start_angle:0,          $
                        long_axis_left:intarr(3),  $
                        long_axis_right:intarr(3), $
                        position_data:0,        $
                        image_min:0,            $
                        image_max:0,            $
                        scale_factor:0.0,       $
                        pixel_size:0.0,         $
                        frame_duration:0L,      $
                        frame_start_time:0L,    $
                        proc_code:0,            $
                        quant_units:0,          $
                        annotation:bytarr(40),  $
                        gate_duration:0L,       $
                        r_wave_offset:0L,       $
                        num_acpt_beats:0L,      $
                 	pm_protocol:bytarr(20), $
                        dbase_name:bytarr(30)   $
                }


;	delvar,AH6,FH6,IH6,MH6,NH6,PI6,RH6,SH6,SI6,PM6
;	delvar,AH7,FH7,IH7,MH7,NH7,PI7,RH7,SH7,SI7,PM7

defsysv,'!ECAT',{ECAT_DATA,                     $
                                file_types:[    "GENERIC",              $
                                "SCN",                  $
                                "IMG",                  $
                                "ATN",                  $
                                "NRM",                  $
                                "PM",                   $
                                "xxx",                  $
                                "V",                    $
                                "xxx",                  $
                                "xxx",                  $
                                "xxx",                  $
                                "S",                    $
                                "xxx",                  $
                                "N"                     $
                           		],		$
                                data_types:[    "GENERIC",              $
                                "BYTE",                 $
                                "VAX_I2",               $
                                "VAX_I4",               $
                                "VAX_R4",               $
                                "SUN_R4",               $
                                "SUN_I2",               $
                                "SUN_I4"                $
                           		],		$
                                unit_types:[    "TOTAL_CTS",            $
                                "UNKNOWN",              $
                                "ECAT",                 $
                                "mCi/ml",               $
                                "MRGlu",                $
                                "MRGlu_umol",           $
                                "MRGlu_mg",             $
                                "nCi/ml",               $
                                "WELL_COUNTS",          $
                                "BQ"                    $
                           		],  		$
                                acq_modes:[    "ACQM_NORMAL",          $
                                "ACQM_RODTRN",          $
                                "ACQM_RODTRN_KEEP",     $
                                "ACQM_DWIN_KEEP",       $
                                "ACQM_DWIN",            $
                                "ACQM_SIMULT"           $
                           		],	    	$
                                mh6:{Main_Header6},     $
                                sh_scn6:{Scn_Subheader6},$
                                sh_img6:{Img_Subheader6},$
                                sh_nrm6:{Nrm_Subheader6},$
                                sh_atn6:{Atn_Subheader6},$
                                sh_pm6:{Pm_Subheader6}, $
                                mh7:{Main_Header7},     $
                                sh_scn7:{Scn_Subheader7},$
                                sh_img7:{Img_Subheader7},$
                                sh_nrm7:{Nrm_Subheader7},$
                                sh_atn7:{Atn_Subheader7},$
                                sh_pm7:{Pm_Subheader7}, $
                                blksize:512L,           $
                                dirblk1:2L,             $
                                OK:0,                   $
                                ERROR:-1,               $
                                ECAT_ERROR:"",          $
                                ERROR_CODES:["ECAT ERROR",      $
		"IO ERROR",					$
		"UNMATCHED PARAMETERS",				$
		"NUMBER OF MATRICES = 0",			$
		"UNRECOGNIZED DATA TYPE",			$
		"INSUFFICIENT DIRECTORY SPACE",			$
		"FILE NOT OPEN",				$
		"NO MAIN HEADER PARAMETER",			$
		"NEW FILE WITH NO DATA",			$
		"NUMBER OF MATRICES NOT EQUAL TO NUMBER OF SUBHEADERS",	$
		"DATA DIMENSIONS NOT 2D OR 3D",			$
		"NUMBER OF DATA SLICES NOT EQUAL TO NUMBER OF MATRICES",$
		"FIRST MATRIX NOT FOUND",			$
		"OLD DATA DIMENSIONS NOT EQUAL TO NEW DATA DIMENSIONS",	$
		"MATRIX LIST DOES NOT MATCH DIRECTORY",		$
		"UNRECOGNIZED FILE TYPE",			$
		"NUMBER OF SCALE FACTORS NOT EQUAL TO NUMBER OF SUBHEADERS"] $
				}

