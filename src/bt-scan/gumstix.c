#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <bt-scan-rssi.h>
#include <commands.h>

extern Inquiry_data inq_data;
extern Configuration config;

pthread_t btscan_thread, alive_thread;
pthread_mutex_t inquiry_sem;
struct sockaddr_in servaddr_console, servaddr_service, servaddr_inquiry;
int sd;

void* alive(void* args) {
	while(true) {
		sendCommand(sd, &servaddr_service, ALIVE, NULL);
		sleep(ALIVE_INTERVAL);
	}
}

int initParameter(int argc, char** argv) {

	int c, missingS = true;
	opterr = 0;

	// Default configuration
	strcpy(config.id_gumstix, "Gumstix");
	config.alarm_threshold = 4;
	config.scan_lenght = 8;
	config.auto_send = true;

	// i parametri seguiti da : richiedono un argomento obbligatorio
	while ((c = getopt(argc, argv, "hn:a:l:s:")) != -1)
		switch (c) {
		case 'n':
			strcpy(config.id_gumstix, optarg);
			break;
		case 'a':
			config.alarm_threshold = atoi(optarg);
			break;
		case 'l':
			config.scan_lenght = atoi(optarg);
			break;
		case 's':
			strcpy(config.server_ip, optarg); // questo deve essere obbligatorio
			missingS = false;
			break;
		case 'h':
			printf("Gumstix help\n");
			printf("Parameters:\n");
			printf("-s: set server address [required]\n");
			printf("-n: set gumstix identifier\n");
			printf("-l: set inquiry length\n");
			printf("-a: set alarm threshold\n");
			printf(".h: print this help\n");
			return 0;
			break;
		case '?':
			if (optopt == 'n' || optopt == 'a' || optopt == 'l' || optopt
					== 's')
				fprintf(stderr, "Option -%c requires an argument.\n", optopt);
			else if (isprint(optopt))
				fprintf(stderr, "Unknown option `-%c'.\n", optopt);
			else
				fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);

			fprintf(stderr, "Try 'gumstix -h' for more information.\n");
			return 0;

		default:
			abort();
		}

	if(missingS){
		fprintf(stderr, "-s arg is required\n");
		fprintf(stderr, "Try 'gumstix -h' for more information.\n");
		return 0;
	}

	return 1;
}

int main(int argc, char** argv) {

	Command command;
	struct hostent *host;
	int result;

	if (!initParameter(argc, argv))
		return 1;

	// Setting sockets
	memset((char *)&servaddr_console, 0, sizeof(struct sockaddr_in));
	memset((char *)&servaddr_service, 0, sizeof(struct sockaddr_in));
	memset((char *)&servaddr_inquiry, 0, sizeof(struct sockaddr_in));
	servaddr_console.sin_family = AF_INET;
	servaddr_service.sin_family = AF_INET;
	servaddr_inquiry.sin_family = AF_INET;
	printf("Server: %s\n", config.server_ip);
	host = gethostbyname(config.server_ip);
	if (host == NULL)
	{
		printf("%s not found in /etc/hosts\n", config.server_ip);
		exit(2);
	}
	else
	{
		servaddr_console.sin_addr.s_addr=((struct in_addr *)(host->h_addr))->s_addr;
		servaddr_console.sin_port = htons(63170);
		servaddr_service.sin_addr.s_addr=((struct in_addr *)(host->h_addr))->s_addr;
		servaddr_service.sin_port = htons(63171);
		servaddr_inquiry.sin_addr.s_addr=((struct in_addr *)(host->h_addr))->s_addr;
		servaddr_inquiry.sin_port = htons(63172);
	}

	sd = bindSocketUDP(0, 0);

	// Hello to server
	printf("Sending HELLO to server ...\n");
	result = sendCommand(sd, &servaddr_service, HELLO, config.id_gumstix);
	printf("HELLO sent to server ...\n");

	// Initialize semaphore
	pthread_mutex_init(&inquiry_sem, NULL);

	//pthread_create(&alive_thread, NULL, alive, NULL);

	// Create scanning thread
	pthread_create(&btscan_thread, NULL, executeInquire, NULL);

	// Receiving commands from server
	while (true) {

		command = receiveCommand(sd, NULL);
		switch (command.id_command) {
		case ERROR:
			printf("Received ERROR from server\n");
			break;
		case 'a':
			printf("Received 'a' from server\n");
			break;
		default:
			printf("Unknown command\n");
			break;
		}

	}

	return 0;
}
