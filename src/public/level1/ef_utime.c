/**
 * ********************************************************************************************************************
 *  @file     ef_utime.c
 *  @ingroup  group_eFAT_Public
 *  @author   ChaN
 *  @author   Emmanuel AMADIO
 *  @version  V0.1
 *  @brief    Change Timestamp
 *
 * ********************************************************************************************************************
 *  eFAT - embedded FAT Filesystem module
 * ********************************************************************************************************************
 *
 *  Copyright (C) 2021, Amadio Emmanuel, all right reserved.
 *  Copyright (C) 2019, ChaN, all right reserved.
 *
 *  eFAT module is an open source software. Redistribution and use of eFAT in source and binary forms, with or without
 *  modification, are permitted provided that the following condition is met:
 *
 *  Redistributions of source code must retain the above copyright notice, this condition and the following disclaimer.
 *
 *  This software is provided by the copyright holders and contributors "AS IS" and any warranties related to this
 *  software are DISCLAIMED.
 *  The copyright owners or contributors be NOT LIABLE for any damages caused by use of this software.
 * ********************************************************************************************************************
 */

/* START OF FILE *************************************************************************************************** */
/* ***************************************************************************************************************** */

/* Includes -------------------------------------------------------------------------------------------------------- */

#include <ef_port_load_store.h>
#include <efat.h>
#include "ef_prv_def.h"
#include <ef_prv_fat.h>
#include <ef_prv_lfn.h>
#include "ef_prv_directory.h"
#include "ef_prv_dirfunc.h"
#include "ef_prv_fs_window.h"
#include "ef_prv_lock.h"
#include "ef_prv_path_follow.h"
#include <ef_prv_volume_mount.h>
#include "ef_prv_volume.h"

/* Local constant macros ------------------------------------------------------------------------------------------- */
/* Local function macros ------------------------------------------------------------------------------------------- */
/* Local typedefs, structures, unions and enums -------------------------------------------------------------------- */
/* Local variables ------------------------------------------------------------------------------------------------- */
/* Public variables ------------------------------------------------------------------------------------------------ */
/* Local function prototypes---------------------------------------------------------------------------------------- */
/* Local functions ------------------------------------------------------------------------------------------------- */
/* Public functions ------------------------------------------------------------------------------------------------ */

ef_return_et eEF_utime (
  const TCHAR           * pxPath,
  const ef_file_info_st * pxFileInfo
)
{
  EF_ASSERT_PUBLIC( 0 != pxPath );
  EF_ASSERT_PUBLIC( 0 != pxFileInfo );

//  ef_return_et      eRetVal;
//  ef_directory_st   xDir;
//  ef_fs_st        * pxFS;
//  EF_LFN_BUFFER_DEFINE
//
//
//  /* Get logical drive */
//  eRetVal = eEFPrvVolumeMountCheck( &pxPath, &pxFS );
//  if ( EF_RET_OK == eRetVal )
//  {
//    xDir.xObject.pxFS = pxFS;
//    eRetVal = EF_LFN_BUFFER_SET( pxFS );
//    /* Follow the file path */
//    eRetVal = eEFPrvPathFollow( pxPath, &xDir );
//
//    /* Check object validity */
//    if (    ( EF_RET_OK == eRetVal )
//         && ( 0 != ( ( EF_NS_DOT | EF_NS_NONAME ) & xDir.u8Name[ EF_NSFLAG ] ) ) )
//    {
//      eRetVal = EF_RET_INVALID_NAME;
//    }
//    if ( EF_RET_OK == eRetVal )
//    {
//      vEFPortStoreu32( xDir.pu8Dir + EF_DIR_TIME_MODIFIED, (ef_u32_t)pxFileInfo->u16Date << 16 | pxFileInfo->u16Time );
//      pxFS->u8WinFlags = EF_FS_WIN_DIRTY;
//      eRetVal = eEFPrvFSSync( pxFS );
//    }
//    EF_LFN_BUFFER_FREE( );
//  }

  ef_return_et  eRetVal = EF_RET_OK;
  ef_fs_st    * pxFS;

  if ( EF_RET_OK != eEFPrvVolumeMountCheck( &pxPath, &pxFS ) )
  {
    eRetVal = EF_RETURN_CODE_HANDLER( EF_RET_ERROR );
  }
  else
  {
    ef_directory_st xDir;

    EF_LFN_BUFFER_DEFINE

    xDir.xObject.pxFS = pxFS;

    /* If LFN BUFFER initialization failed */
    if ( EF_RET_OK != EF_LFN_BUFFER_SET( pxFS ) )
    {
      eRetVal = EF_RETURN_CODE_HANDLER( EF_RET_ERROR );
    }
    /* Else, if following file path failed */
    else if ( EF_RET_OK != eEFPrvPathFollow( pxPath, &xDir ) )
    {
      eRetVal = EF_RETURN_CODE_HANDLER( EF_RET_INVALID_NAME );
    }
    /* Else, if it is dot entry or no name */
    else if ( 0 != ( (EF_NS_DOT | EF_NS_NONAME) & xDir.u8Name[ EF_NSFLAG ] ) )
    {
      eRetVal = EF_RETURN_CODE_HANDLER( EF_RET_INVALID_NAME );
    }
    else
    {
      vEFPortStoreu32( xDir.pu8Dir + EF_DIR_TIME_MODIFIED, (ef_u32_t)pxFileInfo->u16Date << 16 | pxFileInfo->u16Time );
      pxFS->u8WinFlags = EF_FS_WIN_DIRTY;
      if ( EF_RET_OK != eEFPrvFSSync( pxFS ) )
      {
        eRetVal = EF_RETURN_CODE_HANDLER( EF_RET_ERROR );
      }
      else
      {
        EF_CODE_COVERAGE( );
      }
    }
    EF_LFN_BUFFER_FREE( );
  }

  (void) eEFPrvFSUnlock( pxFS, eRetVal );
  return eRetVal;
}

/* ***************************************************************************************************************** */
/* END OF FILE ***************************************************************************************************** */