#include "list.h"
#include "protocalTypes.h"

/*
 * @brief find an empty link in list pool
 *
 * @param[i] pList   an instance of list_t
 * @param[o] ppLink  out put link entity
 *
 * @return @ERROR_TYPE
 */
static int listFindEmptyLink(list_t *pList, list_link_t **ppLink)
{
    int i;

    *ppLink = NULL;
    for (i = 0; i < MAX_LIST_SIZE; i++)
    {
        if (pList->pool[i].isUsed == 0)
            break;
    }

    if (i >= MAX_LIST_SIZE)
    {
        return ERROR_NO_RESOURCE;
    }

    *ppLink = &pList->pool[i];
    pList->pool[i].isUsed = 1;

    return EV_SUCCESS;
}

/*
 * @brief this function adds a data and a cresponding free handler
 *
 * @param pList  a vector instance
 * @param data     custom data
 *
 * returns error codes
 */
int listAdd(list_t *pList, uint32_t data)
{
    list_link_t  *plink = NULL;

    listFindEmptyLink(pList, &plink);
    if(NULL == plink) {
        return ERROR_NO_RESOURCE;
    }

    plink->next = NULL;
    plink->data = data;

    // add first plink
    if((pList->head == pList->tail)
               && (pList->head == NULL)) {
        pList->head = plink;
        pList->tail = plink;
        pList->count = 1;
        return 0;
    }

    // add plink to tail, and update new tail
    pList->tail->next = plink;
    pList->tail = plink;
    pList->count ++;
    return 0;
}

/*
 * @brief this function removes a data and call cresponding free handler
 *      automatically if it was registered
 *
 * @param pList  a vector instance
 * @param data     custom data
 *
 * returns error codes
 */
int listRemoveData(list_t *pList, uint32_t data)
{
   list_link_t *plink = NULL, *plinkPre = NULL;

   if (NULL == pList) {
        return -1;
   }

    plink = pList->head;
    while(plink){
        // find the link
        if (data == plink->data){
            // update head if link is head
            if(plink == pList->head) {
                pList->head = plink->next;
            }
            // update tail if the link is tail
            if (plink == pList->tail) {
                pList->tail = plinkPre;
            }
            // update previous link if the link has previous
            if (plinkPre) {
                plinkPre->next = plink->next;
            }
            plink->isUsed = 0;
            pList->count --;
        } else {
            // update previous link
            plinkPre = plink;
        }
        plink = plink->next;
    }

    return 0;
}

/*
 * @brief this function removes all data and call cresponding free handler
 *      automatically if it was registered
 *
 * @param pList  a vector instance
 *
 * returns error codes
 */
int listRemoveAll(list_t *pList)
{
    list_link_t *plink = NULL, *plinkTmp = NULL;

    if (NULL == pList) {
        return -1;
    }

    plink = pList->head;

    while (plink) {
        plinkTmp = plink;
        plink = plink->next;
        plink->isUsed = 0;
    }

    pList->head = NULL;
    pList->tail = NULL;
    pList->count = 0;
    return 0;
}

/*
 * @brief this function caculates the count of vector
 *
 * @param pList  a vector instance
 * @param pCount   out put count value
 *
 * returns error codes
 */
int listCount(list_t *pList, uint32_t *pCount)
{
    if (NULL == pList || (NULL == pCount)) {
        return -1;
    }

    *pCount = pList->count;
    return 0;
}

/*
 * @brief this function retreive data from vector by index
 *
 * @param pList  a vector instance
 * @param index    data index stored in vector
 * @param pdata    output data
 *
 * returns error codes
 */
int listGetAt(list_t *pList,
                         uint32_t index, uint32_t *pdata)
{
    int i;
    list_link_t *plink = NULL;

    if (NULL == pList) {
        return -1;
    }

    if (pList->count <= index) {
        return -1;
    }

    for (i = 0, plink = pList->head; i < index; i ++) {
        plink = plink->next;
    }
    if (plink) {
        *pdata = plink->data;
    }

    return 0;
}

/*
int test_vector()
{
    list_t *aVector = NULL;
    uint32_t value = 0;
    int      index = 9;
    int      ret;

    printf("start test ...\n");
    ret = list_create(&aVector);
    if (0 != ret) {
        printf("\t-- create vector failed%d\n", ret);
        return -1;
    }
    printf ("add value...\n");
    while (value <= 9) {
        list_add(aVector, value++, NULL);
    }
    printf("\n");

    printf("dump...\n");
    list_count(aVector, &index);
    while (index >= 0) {
        ret = list_get_at(aVector, index, &value);
        printf("\t--get value %d(%d), ret%d\n", value, index, ret);
        index --;
    }

    printf("remove first and dump...\n");
    list_remove_data(aVector, 0);
    list_count(aVector, &index);
    while (index >= 0) {
        ret = list_get_at(aVector, index, &value);
        printf("\t--get value %d(%d) ret%d\n", value, index, ret);
        index --;
    }

    printf("remove last and dump...\n");
    list_remove_data(aVector, 9);
    list_count(aVector, &index);
    while (index > 0) {
        ret = list_get_at(aVector, index, &value);
        printf("\t--get value %d(%d), ret%d\n", value, index, ret);
        index --;
    }

    printf("remove middle and dump...\n");
    list_remove_data(aVector, 4);
    list_count(aVector, &index);
    while (index > 0) {
        ret = list_get_at(aVector, index, &value);
        printf("\t--get value %d(%d), ret%d\n", value, index, ret);
        index --;
    }

    printf("remove all\n");
    ret = list_remove_all(aVector);
    if (0 != ret) {
        printf("remove all failed%d\n", ret);
        return -1;
    }
    ret = list_count(aVector, &index);
    printf("get count %d, ret%d\n", index, ret);

    return 0;
}
*/
