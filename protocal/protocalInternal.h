#ifndef protocalInternal_h
#define protocalInternal_h

#define PROTOCAL_VERSION_MAJOR     0

#define PROTOCAL_VERSION_SUB       1

#define PROTOCAL_VERSION_MODIFY    0

#define PROTOCAL_VERSION_NUM       0

/*
 * @brief Do the protocal level command handler
 *
 * @param[i] pProtocal  instance of ProtocalStatusType
 * @param[i] cmdType    command type
 * @param[i] cmd        command id
 * @param[i] pData      data to be handled
 * @param[i] len        length of pData context
 *
 * @return
 *  0 : continue process
 *  1 : this is an internal command, no needer further process
 */
int32_t protocalPreprocessCmd(ProtocalStatusType *pProtocal,
    uint8_t cmdType, uint8_t cmd, uint8_t *pData, uint8_t len);



#endif
