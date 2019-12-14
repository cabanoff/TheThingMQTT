/**
 * @file
 * parsing mqtt message
 */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <mqtt.h>
//#include <posix_sockets.h>
//#include <parse.h>

#define DEBUG_PUBLISH "[{\"key\":\"MartaRum\",\"value\":1}]"
#define MAX_JSON_SIZE 1000
#define MAX_MESSAGES 50
#define CURRENT sensorMess[messCounter]

/**
 * @brief The function saves published message
 * @param[in] message reference to received message.
 *
 * @param[in] size size of the received message.
 *
 * @returns none
 */
typedef struct{
    char jsonStr[MAX_JSON_SIZE];    // 1 - activity1, 2 - Rum1, 3 - Chew1, 4 - Rest1
    uint32_t deviceID;
    uint8_t activity1;
    uint8_t rumination1;
    uint8_t rumination2;
    uint8_t rumination3;
    uint8_t chewing1;
    uint8_t chewing2;
    uint8_t chewing3;
    uint8_t rest1;
    uint8_t rest2;
    uint8_t rest3;
    char time[20];
    //uint8_t counter;
}sensorMess_t;

size_t messageSize;
sensorMess_t sensorMess[MAX_MESSAGES];           //reserv memory for 50 messages
uint16_t messCounter = 0;
//char* messToSend = NULL;

/* --- PRIVATE FUNCTIONS DECLARATION ---------------------------------------- */
int getDevInfo(char*,sensorMess_t*);
void formJsonStrings(sensorMess_t*);

 void parse_save(const char* message, size_t size)
 {
    char debMessage[MAX_JSON_SIZE];
    time_t timer;
    time(&timer);
    struct tm* tm_info = gmtime(&timer);
    char timebuf[20];
    strftime(timebuf, 20, "%Y%m%d%H%M%S", tm_info);     /*get UTC time*/
    char* startSensorInfo = strchr(message, ',') + 1; //next sympol after comma should be sensor message
    uint32_t deviceInfo = getDevInfo(startSensorInfo,&CURRENT);
    if(deviceInfo == 0)return; //message is not vallid;

    memcpy(CURRENT.time,timebuf,20);  // save time
    formJsonStrings(&CURRENT);
    //snprintf(debMessage,MAX_JSON_SIZE,"[{\"key\":\"MartaRum\",\"value\":1,\"datetime\":\"%s\"}]", timebuf);

    if(messCounter < (MAX_MESSAGES-1))messCounter++;

 }


/**
 * @brief returns prepared message
 *
 * @param[in] none
 *
 * @returns message to be sent
 */
  char* parse_get_mess(void)
  {
    if(messCounter){
        messCounter--;
        return(sensorMess[messCounter].jsonStr);
        //return(sensorMess[messCounter].time);
    }else return NULL;
  }


 /* --- PRIVATE FUNCTIONS DEFINITION ----------------------------------------- */

 /**
 * @brief returns device ID in string
 *  first 4 bytes of message is device ID
 *  4bytes = 8 digits
 *  01000000 970F0000 00 01 9F000000 00 00 00000001000000000000000000000000
 * @param[in] message received from sensor,
 * @param[in] sensorMess struct for store sensor information,
 *
 * @returns 0 if not apropiate format of the input messsage
 */
#define IS_DIGIT(x)  ((x>='0')&&(x<='9'))||((x>='a')&&(x<='f'))||((x>='A')&&(x<='F'))


 int getDevInfo(char* message,sensorMess_t* sensorMess)
{
    for(int i = 0; i < 64; i++)if(!(IS_DIGIT(message[i])))return 0;  //message should contain only hex digits

    /*find sensor ID*/
    char IDstring[9]; //8 digits and \0
    IDstring[0] = message[6];
    IDstring[1] = message[7];
    IDstring[2] = message[4];
    IDstring[3] = message[5];
    IDstring[4] = message[2];
    IDstring[5] = message[3];
    IDstring[6] = message[0];
    IDstring[7] = message[1];
    IDstring[8] = 0;
    uint32_t number = (uint32_t)strtol(IDstring, NULL, 16);
    sensorMess->deviceID = number;
    /*find condition 1 byte N*/
    uint8_t rumination, chewing, rest;
    switch(message[19]){
        case '0':
        rest = 1;rumination = 0; chewing = 0;
        break;
        case '1':
        rest = 0;rumination = 0; chewing = 1;
        break;
        case '2':
        rest = 0;rumination = 1; chewing = 0;
        break;
        default:
        rest = 0; rumination = 0; chewing = 0;
    }
    sensorMess->chewing1 = chewing;
    sensorMess->rumination1 = rumination;
    sensorMess->rest1 = rest;
    switch(message[29]){
        case '0':
        rest = 1;rumination = 0; chewing = 0;
        break;
        case '1':
        rest = 0;rumination = 0; chewing = 1;
        break;
        case '2':
        rest = 0;rumination = 1; chewing = 0;
        break;
        default:
        rest = 0; rumination = 0; chewing = 0;
    }
    sensorMess->chewing2 = chewing;
    sensorMess->rumination2 = rumination;
    sensorMess->rest3 = rest;
    switch(message[31]){
        case '0':
        rest = 1;rumination = 0; chewing = 0;
        break;
        case '1':
        rest = 0;rumination = 0; chewing = 1;
        break;
        case '2':
        rest = 0;rumination = 1; chewing = 0;
        break;
        default:
        rest = 0; rumination = 0; chewing = 0;
    }
    sensorMess->chewing3 = chewing;
    sensorMess->rumination3 = rumination;
    sensorMess->rest3 = rest;
    /*find activity 1*/
    char activity1Str[3];
    activity1Str[0] = message[20];
    activity1Str[1] = message[21];
    activity1Str[2] = 0;
    sensorMess->activity1 = (uint8_t)strtol(activity1Str, NULL, 16);

    return 1;

}
/**
 * @brief definition of json strings.
 *        index must not be more than X (char jsonStr[X][MAX_JSON_SIZE];)
 * json string example: [{"key":"MartaRum","value":0,"datetime":"20190915193200"}]
 *
 *
 *   {
 *     "values": [
 *       {
 *         "key": "temp1",
 *         "value": 41.1,
 *         "datetime": "20190915193200"
 *       },
 *       {
 *         "key": "temp2",
 *         "value": 50,
 *         "datetime": "20190915193200"
 *       }
 *     ]
 *   }
 *
 *
 * @param[in] sensorMess struct for store sensor information,
 *
 * @returns none
 */
/* */

void formJsonStrings(sensorMess_t* sensorMess)
{
   // snprintf(sensorMess->jsonStr[0],MAX_JSON_SIZE,"[{\"key\":\"Dev%d_Act1\",\"value\":%d,\"datatime\":\"%s\"}]",
    //                                        sensorMess->deviceID, sensorMess->activity1, sensorMess->time);
//    snprintf(sensorMess->jsonStr[1],MAX_JSON_SIZE,"[{\"key\":\"Dev%d_Rum1\",\"value\":%d,\"datatime\":\"%s\"}]",
//                                            sensorMess->deviceID, sensorMess->rumination1, sensorMess->time);
//    snprintf(sensorMess->jsonStr[2],MAX_JSON_SIZE,"[{\"key\":\"Dev%d_Chew1\",\"value\":%d,\"datatime\":\"%s\"}]",
//                                            sensorMess->deviceID, sensorMess->chewing1, sensorMess->time);
//    snprintf(sensorMess->jsonStr[3],MAX_JSON_SIZE,"[{\"key\":\"Dev%d_Rest1\",\"value\":%d,\"datatime\":\"%s\"}]",
//                                            sensorMess->deviceID, sensorMess->rest1, sensorMess->time);
//    snprintf(sensorMess->jsonStr[4],MAX_JSON_SIZE,"[{\"key\":\"Dev%d_Rum2\",\"value\":%d,\"datatime\":\"%s\"}]",
//                                            sensorMess->deviceID, sensorMess->rumination2, sensorMess->time);
//    snprintf(sensorMess->jsonStr[5],MAX_JSON_SIZE,"[{\"key\":\"Dev%d_Chew2\",\"value\":%d,\"datatime\":\"%s\"}]",
//                                            sensorMess->deviceID, sensorMess->chewing2, sensorMess->time);
//    snprintf(sensorMess->jsonStr[6],MAX_JSON_SIZE,"[{\"key\":\"Dev%d_Rest2\",\"value\":%d,\"datatime\":\"%s\"}]",
//                                            sensorMess->deviceID, sensorMess->rest2, sensorMess->time);
//    snprintf(sensorMess->jsonStr[7],MAX_JSON_SIZE,"[{\"key\":\"Dev%d_Rum3\",\"value\":%d,\"datatime\":\"%s\"}]",
//                                            sensorMess->deviceID, sensorMess->rumination3, sensorMess->time);
//    snprintf(sensorMess->jsonStr[8],MAX_JSON_SIZE,"[{\"key\":\"Dev%d_Chew3\",\"value\":%d,\"datatime\":\"%s\"}]",
//                                            sensorMess->deviceID, sensorMess->chewing3, sensorMess->time);
//    snprintf(sensorMess->jsonStr[9],MAX_JSON_SIZE,"[{\"key\":\"Dev%d_Rest3\",\"value\":%d,\"datatime\":\"%s\"}]",
//                                            sensorMess->deviceID, sensorMess->rest3, sensorMess->time);

 //   sensorMess->counter = 10; // 10 messages to send;
    //messToSend = sensorMess->jsonStr[3];
    snprintf(sensorMess->jsonStr,MAX_JSON_SIZE,\
"{\"values\":[{\"key\":\"Dev%d_Act1\",\"value\":%d,\"datatime\":\"%s\"},\
{\"key\":\"Dev%d_Rum1\",\"value\":%d,\"datatime\":\"%s\"},\
{\"key\":\"Dev%d_Chew1\",\"value\":%d,\"datatime\":\"%s\"},\
{\"key\":\"Dev%d_Rest1\",\"value\":%d,\"datatime\":\"%s\"},\
{\"key\":\"Dev%d_Rum2\",\"value\":%d,\"datatime\":\"%s\"},\
{\"key\":\"Dev%d_Chew2\",\"value\":%d,\"datatime\":\"%s\"},\
{\"key\":\"Dev%d_Rest2\",\"value\":%d,\"datatime\":\"%s\"},\
{\"key\":\"Dev%d_Rum3\",\"value\":%d,\"datatime\":\"%s\"},\
{\"key\":\"Dev%d_Chew3\",\"value\":%d,\"datatime\":\"%s\"},\
{\"key\":\"Dev%d_Rest3\",\"value\":%d,\"datatime\":\"%s\"}]}",\
sensorMess->deviceID, sensorMess->activity1, sensorMess->time,\
sensorMess->deviceID, sensorMess->rumination1, sensorMess->time,\
sensorMess->deviceID, sensorMess->chewing1, sensorMess->time,\
sensorMess->deviceID, sensorMess->rest1, sensorMess->time,\
sensorMess->deviceID, sensorMess->rumination2, sensorMess->time,\
sensorMess->deviceID, sensorMess->chewing2, sensorMess->time,\
sensorMess->deviceID, sensorMess->rest2, sensorMess->time,\
sensorMess->deviceID, sensorMess->rumination3, sensorMess->time,\
sensorMess->deviceID, sensorMess->chewing3, sensorMess->time,\
sensorMess->deviceID, sensorMess->rest3, sensorMess->time);
}
