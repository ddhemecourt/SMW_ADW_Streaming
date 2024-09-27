#include <limits.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h> 
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include "adw_constructor.h"


#include <sys/timex.h>
#include <pigpio.h>

#define SA struct sockaddr
#define MAX 80
#define GPIO 4
#define GPIO_TRIGGER 17

#define PORT 49152

int GUI_socket_idx = 1;
struct sockaddr_in serv_addr;

static int g_gpio1 = GPIO;
static int g_gpio2 = GPIO_TRIGGER;

int TIME;
int ready_err;
struct period_info {
        struct timespec next_period;
        long period_ns;
};
 
static void inc_period(struct period_info *pinfo) 
{
        pinfo->next_period.tv_nsec += pinfo->period_ns;
 
        while (pinfo->next_period.tv_nsec >= 1000000000) {
                /* timespec nsec overflow */
                pinfo->next_period.tv_sec++;
                pinfo->next_period.tv_nsec -= 1000000000;
        }
}
 
static void periodic_task_init(struct period_info *pinfo)
{
        /* for simplicity, hardcoding a 1ms period */
        pinfo->period_ns = 1000000000;
 
        clock_gettime(CLOCK_MONOTONIC, &(pinfo->next_period));
}
 
static void do_rt_task(int* sock, char* adw, int ignore_adw_flag)
{
        /* Do RT stuff here. */
      	
	send(sock[0], adw, 32, 0);
	if(ignore_adw_flag == 0){
		ready_err = 1;
		while(gpioRead(g_gpio2)==0){}
		gpioWrite(g_gpio1,1);
		gpioWrite(g_gpio1,0);
		ready_err = 0;
	}
	
}
 
static void wait_rest_of_period(struct period_info *pinfo)
{
        inc_period(pinfo);
 
        /* for simplicity, ignoring possibilities of signal wakes */
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &pinfo->next_period, NULL);
}


void *simple_cyclic_task(int *sock)
{
        struct period_info pinfo;
	int val = 0;
/*
	//Commenting Out Hard Coded ADWs
	struct adw_s adw0 = {true, true, true, false, false, false, false, 0, 0, 0, 0, 0.00004,0};	
	char *adw_word_0 = malloc(sizeof(char *)*33);
	adw_constructor(adw_word_0,adw0);

	struct adw_s adw1 = {true, true, true, false, false, false, false, 0, 0, 0, 2, 0.00008, 0};	
	char *adw_word_1 = malloc(sizeof(char *)*33);
	adw_constructor(adw_word_1,adw1);
*/


	char *buff = malloc(sizeof(char)*10000);
	int z;
	struct adw_s *adw_input;
	char *adw_word = malloc(sizeof(char *)*33);
	int ignore_adw_flag = 0;
	TIME = 0;

	periodic_task_init(&pinfo);
 
        while (1) {
		adw_input = malloc(sizeof(struct adw_s)*1);				
		bzero(buff,MAX);
		z = recv(sock[GUI_socket_idx],buff,10000,0);
		printf("TIME = %d\n",TIME);
		if(z>10){
			process_adw_string(buff,adw_input);
			adw_constructor(adw_word,adw_input[0]);
			if(adw_input[0].IGNORE_ADW == 1){
				ignore_adw_flag = 1;
			}
			else{
				ignore_adw_flag = 0;
			}
			do_rt_task(sock,adw_word,ignore_adw_flag);
		}

		/*Commenting Out Hard Coded ADWs
		 * if(val == 0){
                	do_rt_task(sock,adw_word_0);
		}
		else{

                	do_rt_task(sock,adw_word_1);
		}*/
		val ^= (0x01);
         	TIME++;
		free(adw_input);
	 	wait_rest_of_period(&pinfo);
		if(ready_err == 1){
			printf("ERROR: SMW200A Ready Signal Not Activated During Control Loop. Exiting Program\n");
		}
        }
 
        return NULL;
}



int main(int argc, char* argv[])
{
	/*INITIATE GPIOS*/	
	if(gpioInitialise()<0) return -1;
	gpioSetMode(g_gpio1,PI_OUTPUT);
	gpioSetMode(g_gpio2,PI_INPUT);
	gpioSetPullUpDown(g_gpio2,PI_PUD_DOWN);
	gpioWrite(g_gpio1,0);
        
	struct sched_param param, param2;
        pthread_attr_t attr,attr2;
        pthread_t thread, thread2;
        int ret;
    	int *sock = malloc(sizeof(int)*(2)); 
	int client_fd;
	
	/* INITIATE UDP SOCKET*/

    	if ((sock[0] = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    	    printf("\n Socket creation error \n");
    	    return;
    	}

    	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
 
    	if (inet_pton(AF_INET, "192.168.58.39", &serv_addr.sin_addr)
    	    <= 0) {
    	    printf(
    	        "\nInvalid address/ Address not supported \n");
    	    return;
    	}

    	if ((client_fd
    	     = connect(sock[0], (struct sockaddr*)&serv_addr,
    	               sizeof(serv_addr)))
    	    < 0) {
    	    printf("\nConnection Failed \n");
    	    return;
    	}

	/*ESTABLISH SERVER SOCKET FOR USER INPUT*/
	
    	int sockfd, connfd, len;
    	struct sockaddr_in servaddr, cli;
   
    	// socket create and verification
    	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    	if (sockfd == -1) {
    	    printf("socket creation failed...\n");
    	    exit(0);
    	}
    	else
    	    printf("Socket successfully created..\n");
    	bzero(&servaddr, sizeof(servaddr));
   
    	// assign IP, PORT
    	servaddr.sin_family = AF_INET;
    	//servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_addr.s_addr = inet_addr("192.168.1.79");
    	servaddr.sin_port = htons(8080);
   
    	// Binding newly created socket to given IP and verification
    	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
    	    printf("socket bind failed...\n");
    	    exit(0);
    	}
    	else
    	    printf("Socket successfully binded..\n");
   
    	// Now server is ready to listen and verification
    	if ((listen(sockfd, 5)) != 0) {
    	    printf("Listen failed...\n");
    	    exit(0);
    	}
    	else
    	    printf("Server listening..\n");
    	len = sizeof(cli);
   
    	// Accept the data packet from client and verification
    	sock[GUI_socket_idx] = accept(sockfd, (SA*)&cli, &len);
    	if (sock[GUI_socket_idx] < 0) {
    	    printf("server accept failed...\n");
    	    exit(0);
    	}
    	else
    	    printf("server accept the client...\n");

	fcntl(sock[GUI_socket_idx], F_SETFL, O_NONBLOCK);
        
	
	/*ESTABLISH HIGH PRIORITY THREAD TO EXECUTE PROGRAM*/
	/* Lock memory */
        if(mlockall(MCL_CURRENT|MCL_FUTURE) == -1) {
                printf("mlockall failed: %m\n");
                exit(-2);
        }
 
        /* Initialize pthread attributes (default values) */
        ret = pthread_attr_init(&attr);
        if (ret) {
                printf("init pthread attributes failed\n");
                goto out;
        }
 
        ret = pthread_attr_init(&attr2);
        if (ret) {
                printf("init pthread attributes failed\n");
                goto out;
        }

        /* Set a specific stack size  */
        ret = pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);
        if (ret) {
            printf("pthread setstacksize failed\n");
            goto out;
        }

        ret = pthread_attr_setstacksize(&attr2, PTHREAD_STACK_MIN);
        if (ret) {
            printf("pthread setstacksize failed\n");
            goto out;
        }

        /* Set scheduler policy and priority of pthread */
        ret = pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
        if (ret) {
                printf("pthread setschedpolicy failed\n");
                goto out;
        }

        ret = pthread_attr_setschedpolicy(&attr2, SCHED_FIFO);
        if (ret) {
                printf("pthread setschedpolicy failed\n");
                goto out;
        }


	param.sched_priority = 99;
        ret = pthread_attr_setschedparam(&attr, &param);
        if (ret) {
                printf("pthread setschedparam failed\n");
                goto out;
        }


	param2.sched_priority = 99;
        ret = pthread_attr_setschedparam(&attr2, &param2);
        if (ret) {
                printf("pthread setschedparam failed\n");
                goto out;
        }

        /* Use scheduling parameters of attr */
        ret = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
        if (ret) {
                printf("pthread setinheritsched failed\n");
                goto out;
        }


        ret = pthread_attr_setinheritsched(&attr2, PTHREAD_EXPLICIT_SCHED);
        if (ret) {
                printf("pthread setinheritsched failed\n");
                goto out;
        }

        /* Create a pthread with specified attributes */
        ret = pthread_create(&thread, &attr, simple_cyclic_task, sock);
        if (ret) {
                printf("create pthread failed with code %d \n", ret);
		goto out;
        }

 
        ret = pthread_join(thread, NULL);
        if (ret)
                printf("join pthread failed: %m\n");
 
 
    	close(client_fd);
out:
        return ret;
}


