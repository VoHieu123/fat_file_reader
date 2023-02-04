/*******************************************************************************
 * Descriptions:
 ******************************************************************************/
/*******************************************************************************
 * Includes:
 ******************************************************************************/
#ifndef __APPLICATION_H__
#define __APPLICATION_H__
#include "fatfs.h"
#include "stdbool.h"
/*******************************************************************************
 * APIs
 ******************************************************************************/
/**
 * @brief Convert a substring of hexadecimal values in input string to integer.
 * Substring is extracted starting from the index "startIndex" to "startIndex - 1 + subStrLen" of the "inputStr".
 *
 * @param[in] startingByte: Starting byte in the file of the value that needs to be found.
 * @param[in] lengthOfValue: Length in bytes of the value that needs to be found.
 * @param[in] filePtr: Pointer to the file which contains the value needs to be found.
 * @param[out] ptr_errorCode: Pointer to the variable that contains error code.
 * @param[inout] None
 * @return The found value
 */
bool keepGoing(uint8_t errorCode);
/**
 * @brief Convert a substring of hexadecimal values in input string to integer.
 * Substring is extracted starting from the index "startIndex" to "startIndex - 1 + subStrLen" of the "inputStr".
 *
 * @param[in] startingByte: Starting byte in the file of the value that needs to be found.
 * @param[in] lengthOfValue: Length in bytes of the value that needs to be found.
 * @param[in] filePtr: Pointer to the file which contains the value needs to be found.
 * @param[out] ptr_errorCode: Pointer to the variable that contains error code.
 * @param[inout] None
 * @return The found value
 */
object_t *chooseObject(object_t *headOfCurrentObjectsList);
/**
 * @brief Convert a substring of hexadecimal values in input string to integer.
 * Substring is extracted starting from the index "startIndex" to "startIndex - 1 + subStrLen" of the "inputStr".
 *
 * @param[in] startingByte: Starting byte in the file of the value that needs to be found.
 * @param[in] lengthOfValue: Length in bytes of the value that needs to be found.
 * @param[in] filePtr: Pointer to the file which contains the value needs to be found.
 * @param[out] ptr_errorCode: Pointer to the variable that contains error code.
 * @param[inout] None
 * @return The found value
 */
void printFolderObjects(object_t *headOfObjectsList);
/**
 * @brief Convert a substring of hexadecimal values in input string to integer.
 * Substring is extracted starting from the index "startIndex" to "startIndex - 1 + subStrLen" of the "inputStr".
 *
 * @param[in] startingByte: Starting byte in the file of the value that needs to be found.
 * @param[in] lengthOfValue: Length in bytes of the value that needs to be found.
 * @param[in] filePtr: Pointer to the file which contains the value needs to be found.
 * @param[out] ptr_errorCode: Pointer to the variable that contains error code.
 * @param[inout] None
 * @return The found value
 */
void printFile(const uint8_t *fileBuffer);
/**
 * @brief Convert a substring of hexadecimal values in input string to integer.
 * Substring is extracted starting from the index "startIndex" to "startIndex - 1 + subStrLen" of the "inputStr".
 *
 * @param[in] startingByte: Starting byte in the file of the value that needs to be found.
 * @param[in] lengthOfValue: Length in bytes of the value that needs to be found.
 * @param[in] filePtr: Pointer to the file which contains the value needs to be found.
 * @param[out] ptr_errorCode: Pointer to the variable that contains error code.
 * @param[inout] None
 * @return The found value
 */
void integerScan(int32_t *userInput, uint32_t startRange, uint32_t endRange);
/**
 * @brief Convert a substring of hexadecimal values in input string to integer.
 * Substring is extracted starting from the index "startIndex" to "startIndex - 1 + subStrLen" of the "inputStr".
 *
 * @param[in] startingByte: Starting byte in the file of the value that needs to be found.
 * @param[in] lengthOfValue: Length in bytes of the value that needs to be found.
 * @param[in] filePtr: Pointer to the file which contains the value needs to be found.
 * @param[out] ptr_errorCode: Pointer to the variable that contains error code.
 * @param[inout] None
 * @return The found value
 */
void charScan(int32_t *userInput);
/**
 * @brief Free an list of objects
 *
 * @param[in] objectToBeFreed: The pointer to the list of objects to be freed.
 * @param[out] None
 * @param[inout] None
 * @return None
 */
void freeObject(object_t *objectToBeFreed);
#endif /* __APPLICATION_H__ */
/*******************************************************************************
 * End of file
 ******************************************************************************/
