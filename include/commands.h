#ifndef COMMANDS_H
#define COMMANDS_H
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <struct.h>

#define PARAM_LENGHT 11
#define ALIVE_INTERVAL 10
#define ISALIVE(gumstix, nowtime) ((nowtime.tv_sec - gumstix->lastseen.tv_sec) < ALIVE_INTERVAL * 3)

enum cmd_id {
    ERROR,
    HELLO,
    HELLO_ACK,
    HELLO_ERR,
    ALIVE,
    ALARM,
    PARAM_ACK,
    PARAM_VALUE,
    GET_INQUIRY,
    SET_AUTO_SEND_INQUIRY,
    GET_AUTO_SEND_INQUIRY,
	GET_IMAGE,
    SET_AUTO_SEND_IMAGES,
    GET_AUTO_SEND_IMAGES,
    GET_IMAGE_RESOLUTION,
	SET_SCAN_INTERVAL,
    GET_SCAN_INTERVAL,
    SET_SCAN_LENGTH,
    GET_SCAN_LENGTH,
    SET_ALARM_THRESHOLD,
    GET_ALARM_THRESHOLD,
    SET_COLOR_THRESHOLD,
    GET_COLOR_THRESHOLD
};

typedef struct{
	enum cmd_id id_command;
	char param[PARAM_LENGHT];
} Command;

int bindSocketUDP(int localPort, int timeoutSeconds);

int sendCommand(int sd, struct sockaddr_in* destinationaddr, enum cmd_id id_command, char* param);
Command receiveCommand(int sd, struct sockaddr_in* sender);

int sendInquiryData(int sd, struct sockaddr_in* destinationaddr, Inquiry_data inq);
Inquiry_data receiveInquiryData(int sd, struct sockaddr_in* sender);

#endif
