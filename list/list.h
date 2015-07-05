//
//  List.h
//  ramfs
//
//  Created by Yunfei Ma on 12-3-25.
//  Copyright (c) 2012 yunfei. All rights reserved.
//

#ifndef list_h
#define list_h

#include <stdint.h>
#define MAX_LIST_SIZE          16

typedef void (*free_func_t)(void *data);

typedef struct _list_link_t{
    struct _list_link_t             *next;
    uint32_t                         data;
    uint8_t                          isUsed;
}list_link_t;

typedef struct _list_t{
    list_link_t    *head;
    list_link_t    *tail;
    list_link_t     pool[MAX_LIST_SIZE];
    uint32_t        count;
}list_t;

/*
 * @brief this function adds a data and a cresponding free handler
 *
 * @param pList  a vector instance
 * @param data     custom data
 *
 * returns error codes
 */
int listAdd(list_t *pList, uint32_t data);

/*
 * @brief this function removes a data and call cresponding free handler
 *      automatically if it was registered
 *
 * @param pList  a vector instance
 * @param data     custom data
 *
 * returns error codes
 */
int listRemoveData(list_t *pList, uint32_t data);

/*
 * @brief this function removes all data and call cresponding free handler
 *      automatically if it was registered
 *
 * @param pList  a vector instance
 *
 * returns error codes
 */
int listRemoveAll(list_t *pList);

/*
 * @brief this function caculates the count of vector
 *
 * @param pList  a vector instance
 * @param pCount   out put count value
 *
 * returns error codes
 */
int listCount(list_t *pList, uint32_t *pCount);

/*
 * @brief this function retreive data from vector by index
 *
 * @param pList  a vector instance
 * @param index    data index stored in vector
 * @param pdata    output data
 *
 * returns error codes
 */
  int listGetAt(list_t *pList, uint32_t index, uint32_t *pdata);


#endif
