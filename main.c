/*******************************************************************************
 * Includes //Free malloc subjects?
 ******************************************************************************/
#include "application.h"
#include "hal.h"
#include "fatfs.h"
#include <stdio.h>
#include <string.h>
/*******************************************************************************
 * Defines
 ******************************************************************************/
#define MAX_FILE_NAME 50
/*******************************************************************************
 * APIs
 ******************************************************************************/
/**
 * @brief Push a value to the stack.
 *
 * @param[in] valueToPush: The value to be pushed in the stack
 * @param[out] None
 * @param[inout] None
 * @return None
 */
static void pushStack(object_t *valueToPush);
/**
 * @brief Pop a value out of stack and update the current directory.
 *
 * @param[in] None
 * @param[out] None
 * @param[inout] None
 * @return None
 */
static void popStack(void);
/**
 * @brief Print the current directory.
 *
 * @param[in] None
 * @param[out] None
 * @param[inout] None
 * @return None
 */
static void printCurrentDir(void);
/**
 * @brief Update current directory.
 *
 * @param[in] currentObject: The folder that is currently in.
 * @param[out] None
 * @param[inout] None
 * @return None
 */
static void updateDir(const object_t *currentObject);
/*******************************************************************************
 * Codes
 ******************************************************************************/
static uint16_t s_currentDir[10000] = {0};
static uint32_t s_currentNumOfDirChar = 0;
static object_t *s_stackOfFolders[10000] = {NULL};
static uint32_t s_numOfStackElements = 0;

int32_t main(int argc, char *argv[])
{
    uint8_t sourceFile[MAX_FILE_NAME] = {0};
    int32_t choice = 0;
    uint32_t fATTable[10000] = {0};
    object_t *headOfRootObjectsList = NULL;
    object_t *headOfCurrentObjectsList = NULL;
    object_t *chosenObject = NULL;

    if (argc > 2)
    {
        printf("Too much directories are given.\n");
    }
    else if (argc <= 1)
    {
        printf("No directory is given.\n");
    }
    else
    {
        strcpy(sourceFile, argv[1]);
        if (keepGoing(initialize_hal(sourceFile)) == true)
        {
            if (keepGoing(readBootSector()) == true)
            {
                if (keepGoing(readFAT()) == true)
                {
                    headOfRootObjectsList = readRDET();
                    if (headOfRootObjectsList == NULL)
                    {
                        printf("Can not seek the desired position in the file.\n"); /* ERROR 2 */
                    }
                    else
                    {
                        pushStack(headOfRootObjectsList);
                        updateDir(headOfRootObjectsList);
                        headOfCurrentObjectsList = headOfRootObjectsList;
                        do
                        {
                            chosenObject = NULL;
                            printCurrentDir();
                            printFolderObjects(headOfCurrentObjectsList);
                            printf("\n");
                            printf("Press 'x' to exit the program.\n");
                            printf("Press 'b' to back to the parent folder.\n");
                            printf("Press 'c' to choose a file/folder.\n\n");
                            charScan(&choice);
                            switch (choice)
                            {
                            case 'x':
                                printf("The program is terminated. Press any key exit.\n");
                                fflush(stdin);
                                getchar();
                                break;
                            case 'b':
                                if (headOfCurrentObjectsList == headOfRootObjectsList)
                                {
                                    printf("No parent folder. This is the root folder.\n");
                                    fflush(stdin);
                                    printf("Press any key to continue.\n");
                                    getchar();
                                }
                                else
                                {
                                    popStack();
                                    headOfCurrentObjectsList = s_stackOfFolders[s_numOfStackElements];
                                }
                                break;
                            case 'c':
                                if (false == headOfCurrentObjectsList->isEmpty)
                                {
                                    chosenObject = chooseObject(headOfCurrentObjectsList);
                                    if (chosenObject != NULL)
                                    {
                                        updateDir(chosenObject);
                                        headOfCurrentObjectsList = readSDET(chosenObject->firstLogicalCluster);
                                        pushStack(headOfCurrentObjectsList);
                                        fflush(stdin);
                                        printf("Folder is successfully chosen. Press any key to continue.\n");
                                        getchar();
                                    }
                                    else
                                    {
                                        fflush(stdin);
                                        printf("\nFile information is successfully printed. Press any key to go back to the current folder.\n");
                                        getchar();
                                    }
                                }
                                else
                                {
                                    printf("Folder is empty. No file or folder to choose. Press any key to continue.\n");
                                    fflush(stdin);
                                    getchar();
                                }
                                break;
                            default:
                                printf("No cases match your choice. Please retype.\n");
                                fflush(stdin);
                                printf("Press any key to continue.\n");
                                getchar();
                                break;
                            }
                            system("cls");
                        } while (choice != 'x');
                    }
                }
            }
        }
        deinitialize_hal();
    }

    return 0;
}
static void pushStack(object_t *valueToPush)
{
    s_numOfStackElements += 1;
    s_stackOfFolders[s_numOfStackElements] = valueToPush;
}
static void popStack(void)
{
    int32_t iLoop = 0;

    if (s_numOfStackElements != 0)
    {
        freeObject(s_stackOfFolders[s_numOfStackElements]);
        s_stackOfFolders[s_numOfStackElements] = NULL;
        s_numOfStackElements -= 1;
    }
    else
    {
        printf("No elements to pop.\n");
    }
    /* Start updating current directory */
    s_currentDir[s_currentNumOfDirChar] = 0;
    s_currentDir[s_currentNumOfDirChar - 1] = 0;
    s_currentNumOfDirChar--;
    for (iLoop = s_currentNumOfDirChar - 1; iLoop >= 0; iLoop--)
    {
        if ('>' != s_currentDir[iLoop])
        {
            s_currentDir[iLoop] = 0; /* Restored to default value */
            s_currentNumOfDirChar--;
        }
        else
        {
            s_currentDir[iLoop + 1] = '\0';
            iLoop = -1; /* Break iLoop */
        }
    }
}
static void printCurrentDir(void)
{
    int32_t iLoop = 0;

    while ('\0' != s_currentDir[iLoop])
    {
        printf("%c", s_currentDir[iLoop]);
        iLoop++;
    }
    printf("\n\n");
}
static void updateDir(const object_t *currentObject)
{
    int32_t iLoop = 0;

    if (0 == s_currentNumOfDirChar)
    {
        s_currentDir[0] = 'r';
        s_currentDir[1] = 'o';
        s_currentDir[2] = 'o';
        s_currentDir[3] = 't';
        s_currentDir[4] = '>';
        s_currentDir[5] = '\0';
        s_currentNumOfDirChar = 5;
    }
    else
    {
        if (0 == currentObject->longName[0])
        {
            while ((iLoop < OBJECT_MAX_SHORT_NAME) && (0 != currentObject->shortName[iLoop]))
            {
                s_currentDir[s_currentNumOfDirChar] = currentObject->shortName[iLoop];
                s_currentNumOfDirChar++;
                iLoop++;
            }
        }
        else
        {
            while ((iLoop < OBJECT_MAX_LONG_NAME) && (0 != currentObject->longName[iLoop]))
            {
                s_currentDir[s_currentNumOfDirChar] = currentObject->longName[iLoop];
                s_currentNumOfDirChar++;
                iLoop++;
            }
        }
        s_currentDir[s_currentNumOfDirChar] = '>';
        s_currentNumOfDirChar++;
        s_currentDir[s_currentNumOfDirChar] = '\0';
    }
}
/*******************************************************************************
 * End of file
 ******************************************************************************/
