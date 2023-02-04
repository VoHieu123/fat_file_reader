/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdio.h>
#include "application.h"
#include <ctype.h>
#include <stdlib.h>
#include <error_code.h>
/*******************************************************************************
 * Codes
 ******************************************************************************/
void freeObject(object_t *objectToBeFreed)
{
    object_t *tempPtr = NULL;
    object_t *tobeFreed = NULL;

    tempPtr = objectToBeFreed;
    while (tempPtr != NULL)
    {
        tobeFreed = tempPtr;
        tempPtr = tempPtr->nextObject;
        free(tobeFreed);
    }
}
bool keepGoing(uint8_t errorCode)
{
    bool keepGoing = true;

    switch (errorCode)
    {
    case ERROR_CODE_CANT_OPEN:
        printf("Can not open the file.\n");
        keepGoing = false;
        break;
    case ERROR_CODE_CANT_SEEK_POS:
        printf("Can not seek the desired position in the file.\n");
        keepGoing = false;
        break;
    default:
        break;
    }

    return keepGoing;
}
void printFolderObjects(object_t *headOfObjectsList)
{
    object_t *tempPtr = NULL;
    uint8_t count = 0;
    int32_t iLoop = 0;
    int32_t jLoop = 0;
    int32_t kLoop = 0;

    tempPtr = headOfObjectsList;
    if (tempPtr->isEmpty == true)
    {
        printf("Empty folder.\n");
    }
    else
    {
        while (tempPtr != NULL)
        {
            iLoop = 0;
            jLoop = 0;
            count++;
            /* Print type */
            printf("%d. ", count);
            if (tempPtr->type == FILE_TYPE)
            {
                printf("File: ");
            }
            else
            {
                printf("Folder: ");
            }
            if (tempPtr->longName[0] == 0)
            {
                while ((iLoop < OBJECT_MAX_SHORT_NAME) && (tempPtr->shortName[iLoop] != 0))
                {
                    printf("%c", tempPtr->shortName[iLoop]);
                    iLoop++;
                }
            }
            else
            {
                while ((iLoop < OBJECT_MAX_LONG_NAME) && (tempPtr->longName[iLoop] != 0))
                {
                    printf("%c", tempPtr->longName[iLoop]);
                    iLoop++;
                }
            }
            printf("\n");
            tempPtr = tempPtr->nextObject;
        }
    }
}
void printFile(const uint8_t *fileBuffer)
{
    int32_t iLoop = 0;

    printf("\nStart printing the chosen file information:\n\n");
    while (fileBuffer[iLoop] != 0)
    {
        printf("%c", fileBuffer[iLoop]);
        iLoop++;
    }
    printf("\n");
}
object_t *chooseObject(object_t *headOfCurrentObjectsList)
{
    int32_t choice = 0;
    object_t *chosenObject = NULL;
    object_t *tempPtr = NULL;
    uint8_t count = 1;
    uint32_t numOfObjects = 0;
    uint8_t fileBuffer[1000000] = {0};

    tempPtr = headOfCurrentObjectsList;
    while (NULL != tempPtr)
    {
        numOfObjects++;
        tempPtr = tempPtr->nextObject;
    }
    printf("Enter the number of file/folder you want to choose.\n");
    integerScan(&choice, 1, numOfObjects);
    chosenObject = headOfCurrentObjectsList;
    while (chosenObject != NULL && count != choice)
    {
        count++;
        chosenObject = chosenObject->nextObject;
    }

    if (chosenObject->type == FILE_TYPE)
    {
        if (keepGoing(collectFileInformation(chosenObject->firstLogicalCluster, fileBuffer)) == true)
        {
            printFile(fileBuffer);
            chosenObject = NULL;
        }
        else
        {
            printf("Can not open the file.\n");
        }
    }

    return chosenObject;
}
void integerScan(int32_t *userInput, uint32_t startRange, uint32_t endRange)
{
    uint8_t enterKey = 0U;
    uint8_t scanfReturn = 0U;

    printf("Please enter an integer: ");
    fflush(stdin);
    scanfReturn = scanf("%d%c", userInput, (&enterKey)); /* User should type two things: an integer and an enter key "\n" */
    while (2 != scanfReturn || enterKey != '\n' || (*userInput) > endRange || (*userInput) < startRange)
    {
        printf("Your input value is not valid or out of range.\nPlease re-enter the value: ");
        fflush(stdin);
        scanfReturn = scanf("%d%c", userInput, (&enterKey));
    }
}
void charScan(int32_t *userInput)
{
    uint8_t enterKey = 0U;
    uint8_t scanfReturn = 0U;

    printf("Please enter an character: ");
    fflush(stdin);
    scanfReturn = scanf("%c%c", userInput, (&enterKey)); /* User should type two things: an character and an enter key "\n" */
    while (2 != scanfReturn || enterKey != '\n' || isalpha(*userInput) == 0)
    {
        printf("Your input value is not valid.\nPlease re-enter the value: ");
        fflush(stdin);
        scanfReturn = scanf("%c%c", userInput, (&enterKey));
    }
}
/*******************************************************************************
 * End of file
 ******************************************************************************/
