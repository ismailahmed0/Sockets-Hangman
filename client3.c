#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 

int main(int argc, char *argv[])
{
	int sockfd = 0, n = 0;
    	char recvBuff[1024];
    	struct sockaddr_in serv_addr; 

    	if(argc != 3)
    	{
        	printf("\n Usage: %s <ip of server> <port #> \n",argv[0]);
        	return 1;
    	} 

    	memset(recvBuff, '0',sizeof(recvBuff));
    	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    	{
        	printf("\n Error : Could not create socket \n");
        	return 1;
    	} 

    	memset(&serv_addr, '0', sizeof(serv_addr)); 

    	serv_addr.sin_family = AF_INET;
	int port = atoi(argv[2]);
    	serv_addr.sin_port = htons(port); 

    	if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
    	{
        	printf("\n inet_pton error occured\n");
        	return 1;
    	} 

    	if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    	{
       		printf("\n Error : Connect Failed \n");
       		return 1;
    	} 

    	/*while ( (n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0)
    	{
        	recvBuff[n] = 0;
        	if(fputs(recvBuff, stdout) == EOF)
        	{
            		printf("\n Error : Fputs error\n");
        	}
    	}*/

	char guess[1]; // store letter to be guessed

	int count = 0; // use roundabout way to attempt end program
    	while (count == 0)
    	{
		// read message from client
		n = read(sockfd, recvBuff, sizeof(recvBuff)-1);        	
		recvBuff[n] = 0;
		//puts("Received word:");
		fputs(recvBuff, stdout); // display message from client to screen

        	//printf("%s %d %d\n", recvBuff, strlen(recvBuff), strncmp(recvBuff,"Completed!\n", 10));
                // if word guessed
                if (strncmp(recvBuff,"Completed!", 10) == 0) {
			// read message from client
			//n = read(sockfd, recvBuff, sizeof(recvBuff)-1);
                	//recvBuff[n] = 0;
					//puts("Completed word: ");
                	//fputs(recvBuff, stdout);

                        count++;
			//sleep(1);
			//puts("Just before return 0!");
			return 0;
                }

		// if workd not completely guessed
		if (count == 0) {
			gets(guess); // get input from user(only takes 1st character no matter lenghth)
			write(sockfd, guess, strlen(guess)); // send letter to server
		}
		//sleep(1);		
    	} 
	//sleep(1);
    	if(n < 0)
    	{
        	printf("\n Read error \n");
    	} 
	//exit(0);
    	return 0;
	//sleep(1);
	//kill(getpid());
}
