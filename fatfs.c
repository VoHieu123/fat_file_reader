/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "fatfs.h"
#include "hal.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <error_code.h>
/*******************************************************************************
 * Defines
 ******************************************************************************/
#define BYTE_PER_ENTRY 32U
#define LONG_FILE_NAME_FLAG 0x0FU
#define SUB_DIR_BIT_MASK 0x10U
#define SHORT_NAME_LENGTH 11U
#define ENTRIES_PER_SECTOR 16U
#define MAX_ENTRIES_BUFFER 10000U
#define MAX_SHORT_NAME 11U
#define MAX_LONG_NAME 255U
#define FAT_12 0U
#define FAT_16 1U
#define FAT_32 2U
#define ENTRIES_PER_SECTOR 16U
#define MAX_NUM_OF_FAT_ELEMENTS 10000U
/*******************************************************************************
 * Enums
 ******************************************************************************/
enum fatValue
{
    FAT12_BAD_CLUSTER = 0xFF7,
    FAT12_EOF_CLUSTER = 0xFFF,
    FAT12_FREE_CLUSTER = 0x00,
    FAT16_BAD_CLUSTER = 0xFFF7,
    FAT16_EOF_CLUSTER = 0xFFFF,
    FAT16_FREE_CLUSTER = 0x00,
    FAT32_BAD_CLUSTER = 0x0FFFFFF7,
    FAT32_EOF_CLUSTER = 0x0FFFFFFF,
    FAT32_FREE_CLUSTER = 0x00,
};
enum rDETElementsOffset
{
    LOW_BYTE_OF_FIRST_LOGICAL_CLUSTER_OFFSET = 0X1A,
    HIGH_BYTE_OF_FIRST_LOGICAL_CLUSTER_OFFSET = 0X14,
    ATTRIBUTE_OFFSET = 0XB
};
enum bootSectorElementsOffset
{
    SECTOR_PER_FAT_OFFSET = 22,
    NUM_OF_FATS_OFFSET = 16,
    NUM_OF_ENTRIES_OFFSET = 17,
    NUM_OF_SECTORS_BEFORE_FAT_OFFSET = 14,
    SECTORS_PER_CLUSTER_OFFSET = 13,
    FAT_TYPE_OFFSET = 0x39,
    BYTES_PER_SECTORS_OFFSET = 0xB
};
/*******************************************************************************
 * APIs
 ******************************************************************************/
/**
 * @brief Convert two 8-bit bytes to on 16-bit byte
 *
 * @param[in] lowByte: The low byte.
 * @param[in] highByte: The high byte.
 * @param[out] None
 * @param[inout] None
 * @return The 16-bit byte.
 */
static uint16_t cvt2Bytes(uint8_t lowByte, uint8_t highByte);
/**
 * @brief Create a new node that contains information from the inputs.
 *
 * @param[in] isEmpty: The current folder is empty or not.
 * @param[in] inputShortName: Short name of the object.
 * @param[in] inputLongName: Long name of the object.
 * @param[in] inputType: Type of the object.
 * @param[in] inputFirstLogicalCluster: First logical cluster of the object.
 * @param[out] None
 * @param[inout] None
 * @return The pointer to a new node containing infomation form these inputs.
 */
static object_t *createNode(bool isEmpty, const uint8_t *inputShortName, const uint16_t *inputLongName, uint8_t inputType, uint16_t inputFirstLogicalCluster);
/**
 * @brief Insert a new node to the head of the list of objects
 *
 * @param[in] isEmpty: The current folder is empty or not.
 * @param[in] inputShortName: Short name of the object.
 * @param[in] inputLongName: Long name of the object.
 * @param[in] inputType: Type of the object.
 * @param[in] inputFirstLogicalCluster: First logical cluster of the object.
 * @param[out] None
 * @param[inout] None
 * @return The pointer to the head of the list of objects
 */
static object_t *insertHead(object_t *headObject, const uint8_t *inputShortName, const uint16_t *inputLongName, uint8_t inputType, uint16_t inputFirstLogicalCluster);
/*******************************************************************************
 * Codes
 ******************************************************************************/
static uint32_t s_fATTable[MAX_NUM_OF_FAT_ELEMENTS] = {0};
static boot_t *s_bootSector = NULL;

static object_t *insertHead(object_t *headObject, const uint8_t *inputShortName, const uint16_t *inputLongName, uint8_t inputType, uint16_t inputFirstLogicalCluster)
{
    object_t *tempPtr = NULL;

    if (NULL == headObject)
    {
        headObject = createNode(false, inputShortName, inputLongName, inputType, inputFirstLogicalCluster);
    }
    else
    {
        tempPtr = headObject;
        headObject = createNode(false, inputShortName, inputLongName, inputType, inputFirstLogicalCluster);
        headObject->nextObject = tempPtr;
    }

    return headObject;
}
static uint16_t cvt2Bytes(uint8_t lowByte, uint8_t highByte)
{
    uint32_t resultByte = 0;

    resultByte = lowByte + highByte * 0x100;

    return resultByte;
}
static object_t *createNode(bool isEmpty, const uint8_t *inputShortName, const uint16_t *inputLongName, uint8_t inputType, uint16_t inputFirstLogicalCluster)
{
    object_t *newNode = NULL;
    uint32_t iLoop = 0;
    uint32_t jLoop = 0;
    uint32_t kLoop = 0;
    uint32_t mLoop = 0;
    uint32_t lLoop = 0;

    newNode = malloc(sizeof(object_t));
    newNode->nextObject = NULL;
    newNode->isEmpty = isEmpty;
    /* Initialize short and long name values */
    for (mLoop = 0; mLoop < OBJECT_MAX_SHORT_NAME; mLoop++)
    {
        newNode->shortName[mLoop] = 0;
    }
    for (lLoop = 0; lLoop < OBJECT_MAX_LONG_NAME; lLoop++)
    {
        newNode->longName[lLoop] = 0;
    }
    /* Construct object long name */
    while (iLoop < OBJECT_MAX_LONG_NAME && inputLongName[iLoop] != 0x00)
    {
        newNode->longName[iLoop] = inputLongName[iLoop];
        iLoop++;
    }
    /* Construct object short name */
    while (jLoop < 8 && inputShortName[jLoop] != 0x20)
    {
        newNode->shortName[kLoop] = inputShortName[jLoop];
        kLoop++;
        jLoop++;
    }
    jLoop = 8;
    if (inputShortName[jLoop] != 0x20)
    {
        newNode->shortName[kLoop] = '.';
        kLoop++;
        for (iLoop = 0; iLoop < 3; iLoop++)
        {
            if (inputShortName[jLoop] != 0x20)
            {
                newNode->shortName[kLoop + iLoop] = inputShortName[jLoop + iLoop];
            }
        }
    }
    newNode->firstLogicalCluster = inputFirstLogicalCluster;
    newNode->type = inputType;

    return newNode;
}
uint8_t collectFileInformation(uint32_t firstLogicalCluster, uint8_t *fileBuffer)
{
    uint32_t currentLogicalCluster = 0;
    int32_t eOFCluster = 0;
    uint8_t errorCode = 0;
    int32_t badCluster = 0;
    int32_t freeCluster = 0;
    int32_t sectorToRead = 0;
    int32_t iLoop = 0;
    int32_t jLoop = 0;
    uint8_t clusterBuffer[1000000] = {0};

    if (s_bootSector->FATType == FAT_12)
    {
        eOFCluster = FAT12_EOF_CLUSTER;
        badCluster = FAT12_BAD_CLUSTER;
        freeCluster = FAT12_FREE_CLUSTER;
    }
    else if (s_bootSector->FATType == FAT_16)
    {
        eOFCluster = FAT16_EOF_CLUSTER;
        badCluster = FAT16_BAD_CLUSTER;
        freeCluster = FAT16_FREE_CLUSTER;
    }
    else if (s_bootSector->FATType == FAT_32)
    {
        eOFCluster = FAT32_EOF_CLUSTER;
        badCluster = FAT32_BAD_CLUSTER;
        freeCluster = FAT32_FREE_CLUSTER;
    }
    currentLogicalCluster = firstLogicalCluster;
    while (currentLogicalCluster != eOFCluster)
    {
        if (currentLogicalCluster != badCluster && currentLogicalCluster != freeCluster)
        {
            sectorToRead = s_bootSector->firstDataSector + (currentLogicalCluster - 2) * s_bootSector->sectorsPerCluster;
            if (kmc_read_multi_sector(sectorToRead, s_bootSector->sectorsPerCluster, clusterBuffer) == -1)
            {
                errorCode = ERROR_CODE_CANT_SEEK_POS;
            }
            else
            {
                for (iLoop = 0; iLoop < s_bootSector->clusterSize; iLoop++)
                {
                    if (clusterBuffer[iLoop] != 0)
                    {
                        fileBuffer[jLoop] = clusterBuffer[iLoop];
                        jLoop++;
                    }
                }
                /* Next cluster */
                currentLogicalCluster = s_fATTable[currentLogicalCluster];
            }
        }
    }

    return errorCode;
}
uint8_t readBootSector(void)
{
    uint8_t buff[100000] = {0}; /* ??? */
    uint8_t errorCode = 0;
    uint32_t fATTypeSignal = 0;

    if (kmc_read_sector(0, buff) == -1)
    {
        errorCode = ERROR_CODE_CANT_SEEK_POS;
    }
    else
    {
        s_bootSector = malloc(sizeof(boot_t));
        s_bootSector->bytesPerSector = cvt2Bytes(buff[BYTES_PER_SECTORS_OFFSET], buff[BYTES_PER_SECTORS_OFFSET + 1]);
        s_bootSector->numOfSectorsBeforeFAT = cvt2Bytes(buff[NUM_OF_SECTORS_BEFORE_FAT_OFFSET], buff[NUM_OF_SECTORS_BEFORE_FAT_OFFSET + 1]);
        s_bootSector->sectorsPerCluster = buff[SECTORS_PER_CLUSTER_OFFSET];
        s_bootSector->numOfFATS = buff[NUM_OF_FATS_OFFSET];
        s_bootSector->sectorsPerFAT = cvt2Bytes(buff[SECTOR_PER_FAT_OFFSET], buff[SECTOR_PER_FAT_OFFSET + 1]);
        s_bootSector->clusterSize = s_bootSector->bytesPerSector * s_bootSector->sectorsPerCluster;
        s_bootSector->maxNumOfRootDirEntries = cvt2Bytes(buff[NUM_OF_ENTRIES_OFFSET], buff[NUM_OF_ENTRIES_OFFSET + 1]);
        s_bootSector->firstDataSector = s_bootSector->numOfSectorsBeforeFAT + s_bootSector->numOfFATS * s_bootSector->sectorsPerFAT + s_bootSector->maxNumOfRootDirEntries / ENTRIES_PER_SECTOR;
        fATTypeSignal = cvt2Bytes(buff[FAT_TYPE_OFFSET], buff[FAT_TYPE_OFFSET + 1]);
        if (fATTypeSignal == 0x3231)
        {
            s_bootSector->FATType = FAT_12;
        }
        else if (fATTypeSignal == 0x3631)
        {
            s_bootSector->FATType = FAT_16;
        }
        else
        {
            s_bootSector->FATType = FAT_32;
        }
    }

    return errorCode;
}
uint8_t readFAT(void)
{
    int32_t maxFATElements = 0;
    uint8_t fATbuffer[MAX_NUM_OF_FAT_ELEMENTS] = {0};
    uint8_t errorCode = 0;
    int32_t iLoop = 0;

    if (kmc_read_multi_sector(s_bootSector->numOfSectorsBeforeFAT, s_bootSector->sectorsPerFAT, fATbuffer) == 0)
    {
        errorCode = ERROR_CODE_CANT_SEEK_POS;
    }
    else
    {
        if (s_bootSector->FATType == FAT_12)
        {
            maxFATElements = s_bootSector->bytesPerSector * s_bootSector->sectorsPerFAT * 2 / 3;
            for (iLoop = 0; iLoop < maxFATElements; iLoop += 2)
            {
                s_fATTable[iLoop] = fATbuffer[(iLoop / 2) * 3] + 0x100 * (fATbuffer[(iLoop / 2) * 3 + 1] & 0x0F);
                s_fATTable[iLoop + 1] = (fATbuffer[(iLoop / 2) * 3 + 1] & 0xF0) / 0x10 + 0x10 * fATbuffer[(iLoop / 2) * 3 + 2];
            }
        }
        else if (s_bootSector->FATType == FAT_16)
        {
            maxFATElements = s_bootSector->bytesPerSector * s_bootSector->sectorsPerFAT / 2;
            for (iLoop = 0; iLoop < maxFATElements; iLoop += 2)
            {
                s_fATTable[iLoop] = fATbuffer[(iLoop / 2)] + 0x100 * fATbuffer[(iLoop / 2) + 1];
            }
        }
        else if (s_bootSector->FATType == FAT_32)
        {
            maxFATElements = s_bootSector->bytesPerSector * s_bootSector->sectorsPerFAT / 4;
            for (iLoop = 0; iLoop < maxFATElements; iLoop += 2)
            {
                s_fATTable[iLoop] = fATbuffer[(iLoop / 4)] + 0x100 * fATbuffer[(iLoop / 4) + 1] + 0x10000 * fATbuffer[(iLoop / 4) + 2] + 0x1000000 * fATbuffer[(iLoop / 4) + 3];
            }
        }
    }

    return errorCode;
}

object_t *readRDET(void)
{
    int32_t firstSectorToRead = 0;
    int32_t currentEntry = 0;
    uint8_t entriesBuffer[MAX_ENTRIES_BUFFER] = {0};
    int32_t firstByteOfEntry = 0;
    int32_t attributeByte = 0;
    uint8_t type = 0;
    uint8_t shortName[MAX_SHORT_NAME] = {0};
    uint16_t longName[MAX_LONG_NAME] = {0};
    uint32_t firstLogicalCluster = 0;
    uint16_t firstLogicalClusterLowByte = 0;
    uint16_t firstLogicalClusterHighByte = 0;
    int32_t maxNumOfEntrySectors = 0;
    int32_t iLoop = 0;
    int32_t jLoop = 0;
    int32_t kLoop = 0;
    int32_t zLoop = 0;
    object_t *headOfRootObjectsList = NULL;
    bool lastWord = false;

    /* Gather RDET information */
    firstSectorToRead = s_bootSector->sectorsPerFAT * s_bootSector->numOfFATS + s_bootSector->numOfSectorsBeforeFAT;
    maxNumOfEntrySectors = s_bootSector->maxNumOfRootDirEntries / ENTRIES_PER_SECTOR;
    if (0 == kmc_read_multi_sector(firstSectorToRead, maxNumOfEntrySectors, entriesBuffer))
    {
        headOfRootObjectsList = NULL;
    }
    else
    {
        if (0x00 == entriesBuffer[0])
        {
            headOfRootObjectsList = createNode(true, shortName, longName, type, firstLogicalCluster);
        }
        else
        {
            for (currentEntry = 1; currentEntry <= maxNumOfEntrySectors; currentEntry++)
            {
                /* Reset values */
                iLoop = 0;
                jLoop = 0;
                kLoop = 0;
                zLoop = 0;
                firstLogicalCluster = 0;
                type = 0;
                for (iLoop = 0; iLoop < MAX_SHORT_NAME; iLoop++)
                {
                    shortName[iLoop] = 0;
                }
                for (iLoop = 0; iLoop < MAX_LONG_NAME; iLoop++)
                {
                    longName[iLoop] = 0;
                }
                firstByteOfEntry = (currentEntry - 1) * BYTE_PER_ENTRY;
                lastWord = false;
                /* Check if it is the last object in the root directory */
                if (0 == entriesBuffer[firstByteOfEntry])
                {
                    currentEntry = maxNumOfEntrySectors + 1; /* Break currentEntry loop */
                }
                else
                {
                    /* Collect all long-file-name entries and record the name */
                    attributeByte = firstByteOfEntry + ATTRIBUTE_OFFSET;
                    while (entriesBuffer[attributeByte] == LONG_FILE_NAME_FLAG)
                    {
                        for (iLoop = firstByteOfEntry + 1; (lastWord == false) && (iLoop < (firstByteOfEntry + BYTE_PER_ENTRY));)
                        {
                            if (attributeByte == iLoop)
                            {
                                iLoop += 3;
                            }
                            else if (iLoop == 26)
                            {
                                iLoop += 2;
                            }
                            else
                            {
                                longName[jLoop] = cvt2Bytes(entriesBuffer[iLoop], entriesBuffer[iLoop + 1]);
                                if (0 == longName[jLoop])
                                {
                                    lastWord = true;
                                }
                                jLoop++;
                                iLoop += 2;
                            }
                        }
                        currentEntry++;
                        firstByteOfEntry = (currentEntry - 1) * BYTE_PER_ENTRY;
                        attributeByte = firstByteOfEntry + ATTRIBUTE_OFFSET;
                    }
                    /* Start dealing with the main entry */
                    if ((entriesBuffer[attributeByte] & SUB_DIR_BIT_MASK) > 0)
                    {
                        type = FOLDER_TYPE;
                    }
                    else
                    {
                        type = FILE_TYPE;
                    }
                    for (zLoop = 0; zLoop < MAX_SHORT_NAME; zLoop++)
                    {
                        shortName[zLoop] = entriesBuffer[zLoop + firstByteOfEntry];
                    }
                    firstLogicalClusterLowByte = cvt2Bytes(entriesBuffer[firstByteOfEntry + LOW_BYTE_OF_FIRST_LOGICAL_CLUSTER_OFFSET], entriesBuffer[firstByteOfEntry + LOW_BYTE_OF_FIRST_LOGICAL_CLUSTER_OFFSET + 1]);
                    firstLogicalClusterHighByte = cvt2Bytes(entriesBuffer[firstByteOfEntry + HIGH_BYTE_OF_FIRST_LOGICAL_CLUSTER_OFFSET], entriesBuffer[firstByteOfEntry + HIGH_BYTE_OF_FIRST_LOGICAL_CLUSTER_OFFSET + 1]);
                    firstLogicalCluster = firstLogicalClusterLowByte + 0x10000 * firstLogicalClusterHighByte;
                    headOfRootObjectsList = insertHead(headOfRootObjectsList, shortName, longName, type, firstLogicalCluster);
                }
            }
        }
    }

    return headOfRootObjectsList; /* If headOfRootObjectsList == NULL -> ERROR_CODE_CANT_SEEK_POS */
}

object_t *readSDET(int32_t folderFirstLogicalCluster)
{
    int32_t firstSectorToRead = 0;
    int32_t currentEntry = 0;
    uint8_t entriesBuffer[MAX_ENTRIES_BUFFER] = {0};
    int32_t firstByteOfEntry = 0;
    int32_t attributeByte = 0;
    uint8_t type = {0};
    uint8_t shortName[MAX_SHORT_NAME] = {0};
    uint16_t longName[MAX_LONG_NAME] = {0};
    int32_t iLoop = 0;
    int32_t jLoop = 0;
    int32_t kLoop = 0;
    int32_t zLoop = 0;
    int32_t aLoop = 0;
    uint32_t objectFirstLogicalCluster = 0;
    uint16_t objectFirstLogicalClusterLowByte = 0;
    uint16_t objectFirstLogicalClusterHighByte = 0;
    uint32_t currentFolderLogicalCluster = 0;
    object_t *headOfSubDirObjectsList = NULL;
    bool lastWord = false;
    bool isEmpty = false;
    int32_t eOFCluster = 0;
    int32_t badCluster = 0;
    int32_t freeCluster = 0;

    if (s_bootSector->FATType == FAT_12) // Nen cho thang vao s_bootSector
    {
        eOFCluster = FAT12_EOF_CLUSTER;
        badCluster = FAT12_BAD_CLUSTER;
        freeCluster = FAT12_FREE_CLUSTER;
    }
    else if (s_bootSector->FATType == FAT_16)
    {
        eOFCluster = FAT16_EOF_CLUSTER;
        badCluster = FAT16_BAD_CLUSTER;
        freeCluster = FAT16_FREE_CLUSTER;
    }
    else if (s_bootSector->FATType == FAT_32)
    {
        eOFCluster = FAT32_EOF_CLUSTER;
        badCluster = FAT32_BAD_CLUSTER;
        freeCluster = FAT32_FREE_CLUSTER;
    }
    currentFolderLogicalCluster = folderFirstLogicalCluster;
    while (currentFolderLogicalCluster != eOFCluster)
    {
        if (currentFolderLogicalCluster != badCluster && currentFolderLogicalCluster != freeCluster)
        {
            /* Reset values */
            for (aLoop = 0; aLoop < MAX_ENTRIES_BUFFER; aLoop++)
            {
                entriesBuffer[aLoop] = 0;
            }
            /* Gather SDET information */
            firstSectorToRead = s_bootSector->firstDataSector + (currentFolderLogicalCluster - 2) * s_bootSector->sectorsPerCluster;
            if (kmc_read_multi_sector(firstSectorToRead, s_bootSector->sectorsPerCluster, entriesBuffer) == 0)
            {
                headOfSubDirObjectsList = NULL;
            }
            else
            {
                if (entriesBuffer[0x40] == 0x00)
                {
                    headOfSubDirObjectsList = createNode(true, shortName, longName, type, objectFirstLogicalCluster);
                }
                else
                {
                    /* Skip two first entries */
                    for (currentEntry = 3; currentEntry <= (s_bootSector->sectorsPerCluster * ENTRIES_PER_SECTOR); currentEntry++)
                    {
                        /* Reset values */
                        iLoop = 0;
                        jLoop = 0;
                        kLoop = 0;
                        zLoop = 0;
                        objectFirstLogicalCluster = 0;
                        type = 0;
                        for (iLoop = 0; iLoop < MAX_SHORT_NAME; iLoop++)
                        {
                            shortName[iLoop] = 0;
                        }
                        for (iLoop = 0; iLoop < MAX_LONG_NAME; iLoop++)
                        {
                            longName[iLoop] = 0;
                        }
                        firstByteOfEntry = (currentEntry - 1) * BYTE_PER_ENTRY;
                        lastWord = false;
                        /* Check if it is the last object in the root directory */
                        if (entriesBuffer[firstByteOfEntry] == 0)
                        {
                            currentEntry = (s_bootSector->sectorsPerCluster * ENTRIES_PER_SECTOR) + 1; /* Break currentEntry loop */
                        }
                        else
                        {
                            /* Collect all long-file-name entries and record the name */
                            attributeByte = firstByteOfEntry + ATTRIBUTE_OFFSET;
                            while (entriesBuffer[attributeByte] == LONG_FILE_NAME_FLAG)
                            {
                                for (iLoop = firstByteOfEntry + 1; (lastWord == false) && (iLoop < (firstByteOfEntry + BYTE_PER_ENTRY));)
                                {
                                    if (attributeByte == iLoop)
                                    {
                                        iLoop += 3;
                                    }
                                    else if (26 == iLoop)
                                    {
                                        iLoop += 2;
                                    }
                                    else
                                    {
                                        longName[jLoop] = cvt2Bytes(entriesBuffer[iLoop], entriesBuffer[iLoop + 1]);
                                        if (0 == longName[jLoop])
                                        {
                                            lastWord = true;
                                        }
                                        jLoop++;
                                        iLoop += 2;
                                    }
                                }
                                currentEntry++;
                                firstByteOfEntry = (currentEntry - 1) * BYTE_PER_ENTRY;
                                attributeByte = firstByteOfEntry + ATTRIBUTE_OFFSET;
                            }
                            /* Start dealing with the main entry */
                            if ((entriesBuffer[attributeByte] & SUB_DIR_BIT_MASK) > 0)
                            {
                                type = FOLDER_TYPE;
                            }
                            else
                            {
                                type = FILE_TYPE;
                            }
                            for (zLoop = 0; zLoop < MAX_SHORT_NAME; zLoop++)
                            {
                                shortName[zLoop] = entriesBuffer[zLoop + firstByteOfEntry];
                            }
                            objectFirstLogicalClusterLowByte = cvt2Bytes(entriesBuffer[firstByteOfEntry + LOW_BYTE_OF_FIRST_LOGICAL_CLUSTER_OFFSET], entriesBuffer[firstByteOfEntry + LOW_BYTE_OF_FIRST_LOGICAL_CLUSTER_OFFSET + 1]);
                            objectFirstLogicalClusterHighByte = cvt2Bytes(entriesBuffer[firstByteOfEntry + HIGH_BYTE_OF_FIRST_LOGICAL_CLUSTER_OFFSET], entriesBuffer[firstByteOfEntry + HIGH_BYTE_OF_FIRST_LOGICAL_CLUSTER_OFFSET + 1]);
                            objectFirstLogicalCluster = objectFirstLogicalClusterLowByte + 0x10000 * objectFirstLogicalClusterHighByte;
                            headOfSubDirObjectsList = insertHead(headOfSubDirObjectsList, shortName, longName, type, objectFirstLogicalCluster);
                        }
                    }
                }
            }
            currentFolderLogicalCluster = s_fATTable[currentFolderLogicalCluster];
        }
    }

    return headOfSubDirObjectsList; /* If headOfSubDirObjectsList == NULL -> ERROR_CODE_CANT_SEEK_POS */
}
/*******************************************************************************
 * End of file
 ******************************************************************************/
