/*******************************************************************************
 * Includes Chú ý 2 hàm chính đang trả về -1 ??
 ******************************************************************************/
#include "hal.h"
#include <stdio.h>
#include <error_code.h>
/*******************************************************************************
 * Defines
 ******************************************************************************/
#define LENGTH_OF_BYTE_PER_SECTOR 2U
#define FIRST_BYTE_OF_BYTE_PER_SECTOR 11U
#define ELEMENT_SIZE 1U
/*******************************************************************************
 * Codes
 ******************************************************************************/
static FILE *s_filePtr = NULL;

uint8_t initialize_hal(const uint8_t *inputFileDir)
{
    int32_t errorCode = 0;

    s_filePtr = fopen(inputFileDir, "rb");
    if (s_filePtr == NULL)
    {
        errorCode = ERROR_CODE_CANT_OPEN;
    }

    return errorCode;
}
void deinitialize_hal(void)
{
    fclose(s_filePtr);
    s_filePtr = NULL;
}
int32_t kmc_read_sector(uint32_t index, uint8_t *buff)
{
    int32_t byteRead = -1;
    uint8_t arrOfBytePerSector[2] = {0};
    int32_t bytePerSector = -1;

    if (fseek(s_filePtr, FIRST_BYTE_OF_BYTE_PER_SECTOR, SEEK_SET) != 0)
    {
        byteRead = -1;
    }
    else
    {
        fread(arrOfBytePerSector, ELEMENT_SIZE, LENGTH_OF_BYTE_PER_SECTOR, s_filePtr);
        bytePerSector = arrOfBytePerSector[0] + 0x100 * arrOfBytePerSector[1];
        if (fseek(s_filePtr, (bytePerSector * index), SEEK_SET) != 0)
        {
            byteRead = -1;
        }
        else
        {
            byteRead = fread(buff, ELEMENT_SIZE, bytePerSector, s_filePtr);
        }

        return byteRead; /* If byteRead == -1 => ERROR_CODE2 */
    }
}
int32_t kmc_read_multi_sector(uint32_t index, uint32_t num, uint8_t *buff)
{
    int32_t byteRead = -1;
    uint8_t arrOfBytePerSector[2] = {0};
    int32_t bytePerSector = -1;

    if (fseek(s_filePtr, FIRST_BYTE_OF_BYTE_PER_SECTOR, SEEK_SET) != 0)
    {
        byteRead = -1;
    }
    else
    {
        fread(arrOfBytePerSector, ELEMENT_SIZE, LENGTH_OF_BYTE_PER_SECTOR, s_filePtr);
        bytePerSector = arrOfBytePerSector[0] + 0x100 * arrOfBytePerSector[1];
        if (fseek(s_filePtr, (bytePerSector * index), SEEK_SET) != 0)
        {
            byteRead = -1;
        }
        else
        {
            byteRead = fread(buff, ELEMENT_SIZE, (bytePerSector * num), s_filePtr);
        }
    }

    return byteRead; /* If byteRead == -1 => ERROR_CODE2 */
}
/*******************************************************************************
 * End of file
 ******************************************************************************/
