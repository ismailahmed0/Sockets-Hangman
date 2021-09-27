#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 
#include <sys/stat.h>
#include <stdbool.h>

char words[84095][25];

void filetoarray() {
        FILE *fp;
        char *line = NULL;
        fp = fopen("dictionary.txt", "r");
        char word[25];

        int count = 0;
        if (fp != NULL) {
                while (fscanf(fp, "%24s", word) > 0) {
                        strcpy(words[count], word);
                        count++;
                }
        }
	fclose(fp);
}


int randomNumber() {
        int randNum;
        srand(time(NULL));
        randNum = (rand()*64*1024+rand())%84095;
        if (randNum < 0) {
                randNum = randNum * -1;
        }
	return randNum;
}


void pexit(char *errmsg) {
	fprintf(stderr, "%s\n", errmsg);
	exit(1);
}

int main(int argc, char *argv[])
{
	int listenfd = 0, connfd = 0;
    	struct sockaddr_in serv_addr; 
    	char buffer[1025];
    	time_t ticks; 

	char recvBuff[1024]; // to store message receieved from the client

    	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		pexit("socket() error.");
		
    	memset(&serv_addr, '0', sizeof(serv_addr));
    	memset(buffer, '0', sizeof(buffer)); 

    	serv_addr.sin_family = AF_INET;
    	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	int port = 4999;
	do {
		port++;
    		serv_addr.sin_port = htons(port); 
    	} while (bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0);
	printf("bind() succeeds for port #%d\n", port);

    	if (listen(listenfd, 10) < 0)
		pexit("listen() error.");

	int counter=1;
    
    	filetoarray(); // put words from file into array    

    	while(1)
    	{
        	int randNum = randomNumber(); // generate a random number based on number of words
        	int length = strlen(words[randNum]); // length of word randomly chosen

        	// client guess word along with other parts of message to be sent        
		char gWord[100] = "(Guess) Enter a letter in word ";
        	int i;
		for (i = 31; i < (31+length); i++) {
        		gWord[i] = '*'; // asterisk the letters of the word to be guessed from the client
        	}
		gWord[i] = '\n';
        	
		int gCount = 0; // to count how many letters guessed
		size_t nbytes = strlen(gWord); // bytes size of what will be sent to client

    		connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);

			if (fork() != 0) //parent goes to while loop
				continue;

		printf("Word: %s, Length: %d\n", words[randNum], length); // have server print the word and its length for testing

		int n = 0;

		while (gCount < length) { // so long as word not completely guessed correctly
			write(connfd, gWord, strlen(gWord)); // write message containing the word and how much of it has been guessed(asterisk been replaced by actual letter) to the client		

			// read a message from the client
			n = read(connfd, recvBuff, sizeof(recvBuff)-1);
                	recvBuff[n] = 0;

			// if letter correctly guessed, change asterisk in word to letter			
                        for (int i = 0; i < length; i++) {
                                if (words[randNum][i] == *recvBuff) {
                                        if (gWord[(i+31)] == '*') { // so already revelaed letters are not counted again
                                                gWord[(i+31)] = *recvBuff;
                                                gCount++;
                                        }
                                }
                        }

			// if word completely guessed
                        if (gCount == length) {
				// send completed status to client
				char sendStr[11] = "Completed!\n";
				write(connfd, sendStr, strlen(sendStr));
				
				// send completed word to client
				int length2 = 6+length; // to account for "Word: " in the new array below
				char finishedWord[100] = "Word: ";
				int i, j;
				for (i = 6, j = 31; i < length2; i++) { // get the completed word from the gWord array
					finishedWord[i] = gWord[j];
					j++; // to account for "(Guess) Enter..." in the gWord array
				}
				finishedWord[i] = '\n';
				write(connfd, finishedWord, strlen(finishedWord)); // send message to client
                        }
		}

		//hardcode to send "ls" command output 
        	// snprintf(buffer, sizeof(buffer), "Client %d: %.24s\r\n", counter, ctime(&ticks));
		//FILE *fcommand = popen("ls -l", "r");
		
		//int n;
		//get the output of that command and forward it to client
		//while ((n = fread(buffer, 1, sizeof(buffer), fcommand)) > 0)
        	//	write(connfd, buffer, nbytes); 
	
		printf("served client %d.\n", counter++);
		//pclose(fcommand);
        	close(connfd);
        	sleep(1);
			return 0;
     	}
}
