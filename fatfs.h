/*******************************************************************************
* Descriptions: Collect and decode information from the disk.
******************************************************************************/
#ifndef __FATFS_H__
#define __FATFS_H__
/*******************************************************************************
* Includes
******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
/*******************************************************************************
* Defines
******************************************************************************/
#define OBJECT_MAX_SHORT_NAME 12U
#define OBJECT_MAX_LONG_NAME 255U
#define FILE_TYPE 1U
#define FOLDER_TYPE 0U
/*******************************************************************************
 * Structs
 ******************************************************************************/
typedef struct object
{
    uint8_t type;
    uint32_t firstLogicalCluster;
    uint8_t shortName[OBJECT_MAX_SHORT_NAME];
    uint16_t longName[OBJECT_MAX_LONG_NAME];
    uint32_t firstClusterOfParentObject;
    bool isEmpty;
    struct object* nextObject;
} object_t;
typedef struct boot
{
    uint32_t numOfFATS;
    uint32_t sectorsPerFAT;
    uint32_t maxNumOfRootDirEntries;
    uint32_t numOfSectorsBeforeFAT;
    uint8_t FATType;
    uint32_t sectorsPerCluster;
    uint32_t bytesPerSector;
    uint32_t clusterSize;
    uint32_t firstDataSector;
} boot_t;
/*******************************************************************************
* APIs
******************************************************************************/
/**
 * @brief Read file information and write it in an input array(fileBuffer).
 *
 * @param[in] firstLogicalCluster: The first logical cluster of the file.
 * @param[out] fileBuffer: Buffer to contains information of the given file.
 * @param[inout] None
 * @return Error code
 */
uint8_t collectFileInformation(uint32_t firstLogicalCluster, uint8_t *fileBuffer);
/**
 * @brief Read boot sector information
 *
 * @param[in] None
 * @param[out] None
 * @param[inout] None
 * @return Error code.
 */
uint8_t readBootSector(void);
/**
 * @brief Construct a list of objects in the root directory.
 *
 * @param[in] None
 * @param[out] None
 * @param[inout] None
 * @return The pointer to the list of objects in the root directory.
 */
object_t *readRDET(void);
/**
 * @brief Read FAT table.
 *
 * @param[in] None
 * @param[out] None
 * @param[inout] None
 * @return Error code.
 */
uint8_t readFAT(void);
/**
 * @brief Read a subdirectory given its first logical cluster
 *
 * @param[in] folderFirstLogicalCluster: The subdirectory's first logical cluster
 * @param[out] None
 * @param[inout] None
 * @return The found value
 */
object_t *readSDET(int32_t folderFirstLogicalCluster);
#endif /* __FATFS_H__ */
/*******************************************************************************
* End of file
******************************************************************************/
