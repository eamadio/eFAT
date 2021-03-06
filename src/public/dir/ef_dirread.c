/**
 * ********************************************************************************************************************
 *  @file     ef_dirread.c
 *  @ingroup  group_eFAT_Public
 *  @author   ChaN
 *  @author   Emmanuel AMADIO
 *  @version  V0.1
 *  @brief    Read Directory Entries in Sequence
 *
 * ********************************************************************************************************************
 */

/* START OF FILE *************************************************************************************************** */
/* ***************************************************************************************************************** */

/* Includes -------------------------------------------------------------------------------------------------------- */

#include <efat.h>
#include <ef_prv_def.h>
#include <ef_prv_fat.h>
#include <ef_prv_lfn.h>
#include "ef_prv_directory.h"
#include "ef_prv_dirfunc.h"
#include "ef_prv_file.h"
#include "ef_prv_lock.h"
#include "ef_prv_validate.h"
#include "ef_prv_volume_nb.h"

/* Local constant macros ------------------------------------------------------------------------------------------- */
/* Local function macros ------------------------------------------------------------------------------------------- */
/* Local typedefs, structures, unions and enums -------------------------------------------------------------------- */
/* Local variables ------------------------------------------------------------------------------------------------- */
/* Public variables ------------------------------------------------------------------------------------------------ */
/* Local function prototypes---------------------------------------------------------------------------------------- */
/* Local functions ------------------------------------------------------------------------------------------------- */
/* Public functions ------------------------------------------------------------------------------------------------ */

ef_return_et eEF_dirread (
  EF_DIR          * pxDir,
  ef_file_info_st * pxFileInfo
)
{
  EF_ASSERT_PUBLIC( 0 != pxDir );
  EF_ASSERT_PUBLIC( 0 != pxFileInfo );

  ef_return_et    eRetVal = EF_RET_INVALID_OBJECT;
  ef_fs_st      * pxFS;
  EF_LFN_BUFFER_DEFINE


  /* Check validity of the directory object */
  if ( EF_RET_OK != eEFPrvValidateObject( &pxDir->xObject, &pxFS ) )
  {
    eRetVal = EF_RET_INVALID_OBJECT;
  }
  else
  {
    if ( 0 == pxFileInfo )
    {
      /* Rewind the directory object */
      eRetVal = eEFPrvDirectoryIndexSet( pxDir, 0 );
    }
    else
    {
      /* If LFN BUFFER initialization failed */
      if ( EF_RET_OK != EF_LFN_BUFFER_SET( pxFS ) )
      {
        eRetVal = EF_RETURN_CODE_HANDLER( EF_RET_ERROR );
      }
      else
      {
        ef_bool_t bEmpty = EF_BOOL_FALSE;
        /* Read an item */
        if ( EF_RET_OK != eEFPrvDirRead( pxDir, &bEmpty ) )
        {
          eRetVal = EF_RETURN_CODE_HANDLER( EF_RET_ERROR );
        }
        else if ( EF_BOOL_TRUE == bEmpty )
        {
          /* Ignore end of directory */
          eRetVal = EF_RET_OK;
        }
        /* A valid entry is found */
        else if ( EF_BOOL_FALSE == bEmpty )
        {
          ef_bool_t     bStretched = EF_BOOL_FALSE;
          ef_bool_t     bMoved = EF_BOOL_FALSE;
          /* Get the object information */
          if ( EF_RET_OK != eEFPrvDirFileInfosGet( pxDir, pxFileInfo ) )
          {
            eRetVal = EF_RETURN_CODE_HANDLER( EF_RET_ERROR );
          }
          /* Increment index for next */
          else if ( EF_RET_OK != eEFPrvDirectoryIndexNext( pxDir, EF_BOOL_FALSE, &bStretched, &bMoved ) )
          {
            eRetVal = EF_RETURN_CODE_HANDLER( EF_RET_ERROR );
          }
          eRetVal = eEFPrvDirectoryIndexNext( pxDir, EF_BOOL_FALSE, &bStretched, &bMoved );
          if ( EF_RET_NO_FILE == eRetVal )
          {
            /* Ignore end of directory now */
            eRetVal = EF_RET_OK;
          }
        }
      }
      EF_LFN_BUFFER_FREE();
    }
  }
  (void) eEFPrvFSUnlock( pxFS, eRetVal );
  return eRetVal;
}

/* ***************************************************************************************************************** */
/* END OF FILE ***************************************************************************************************** */
