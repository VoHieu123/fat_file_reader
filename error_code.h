/*******************************************************************************
 * Description: Contains the error code definitions for this project.
 ******************************************************************************/
#ifndef __ERROR_CODE_H__
#define __ERROR_CODE_H__
/*******************************************************************************
 * Enums
 ******************************************************************************/
enum errorCode
{
    ERROR_CODE_CANT_OPEN = 1U,      /* Cannot open the input FAT file */
    ERROR_CODE_CANT_SEEK_POS = 2U   /* Cannot seek the desired position in the input FAT file */
};
#endif /* __ERROR_CODE_H__ */
/*******************************************************************************
* End of file
******************************************************************************/