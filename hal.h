/*******************************************************************************
 * Description: This layer of the program interacts directly with hardware to
 * take the information from it.
 ******************************************************************************/
#ifndef __HAL_H__
#define __HAL_H__
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
/*******************************************************************************
 * APIs
 ******************************************************************************/
/**
 * @brief Read all bytes in a specified sector.
 *
 * @param[in] index: The index of sector that need to be read.
 * @param[out] buff: Buffer to store the bytes that are read by this function.
 * @param[inout] None
 * @return The number of bytes that are succesfully read.
 */
int32_t kmc_read_sector(uint32_t index, uint8_t *buff);
/**
 * @brief Read all bytes in a sequence of specified sectors.
 *
 * @param[in] index: The index of sector that need to be read.
 * @param[in] num: The number of consecutive sectors that needs to be read.
 * @param[out] buff: Buffer to store the bytes that are read by this function.
 * @param[inout] None
 * @return The number of bytes that are succesfully read.
 */
int32_t kmc_read_multi_sector(uint32_t index, uint32_t num, uint8_t *buff);
/**
 * @brief Read input file.
 * @param[in] inputFileDir The directory to the input file.
 * @param[out] None
 * @param[inout] None
 * @return Error code
 */
uint8_t initialize_hal(const uint8_t *inputFileDir);
/**
 * @brief Close and reset the file pointer.
 *
 * @param[in] None
 * @param[out] None
 * @param[inout] None
 * @return Error code
 */
void deinitialize_hal(void);
#endif /* __HAL_H__ */
/*******************************************************************************
 * End of file
 ******************************************************************************/
