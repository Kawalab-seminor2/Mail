#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define ERR -1
#define MAX 64
#define SERVER_IP "192.168.29.105" //local loopback
#define SMTP 1900
#define POP 1901

int main(){
	int sock, cli, k;
	unsigned int len;
	int sent;
	struct sockaddr_in server, client;
	char buff[MAX], msg[MAX], to[MAX], Cc[MAX], txt[]="rec.txt", file_dir[MAX];
	FILE *fp;
	pid_t child_pid;

	system("clear");
	// ソケットの作成
	if((sock = socket(AF_INET, SOCK_STREAM, 0)) == ERR){
		perror("socket: ");
		exit(-1);
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(SMTP);
	server.sin_addr.s_addr = INADDR_ANY;

	// bind
	len = sizeof(struct sockaddr_in);
	int yes=1;
    if(setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(const char *)&yes,sizeof(yes))<0){
    perror("ERROR on setsocketopt");
    exit(1);
    }
	
	if(bind(sock, (struct sockaddr *)&server, sizeof(server)) == ERR){
		perror("bind");
		exit(-1);
	}

	// listen
	if((listen(sock, 10)) == ERR){
		perror("listen");
		exit(-1);
	}

	while(1){
		memset(buff, 0, sizeof(buff));
		memset(msg, 0, sizeof(msg));
		memset(to, 0, sizeof(to));
		memset(Cc, 0, sizeof(Cc));

		printf("(SMTP) Waiting for connection\n");
		if((cli = accept(sock, (struct sockaddr *)&client, &len)) == ERR){
			perror("accept");
			exit(-1);
		}

		// create child process
		if ((child_pid = fork()) < 0) {	
			perror("fork");
			exit(-1);
		}
		// child
		else if (child_pid == 0) {
			close(sock);		// close child copy
			// receive from client
			recv(cli, to, sizeof(msg), 0);
			printf("Connection request to \"%s\"\n",to);
			
			for(k=0;;k++){
				sprintf(file_dir,"%s/mail%d.txt", to, k+1);
				if ((fp = fopen(file_dir, "r")) == NULL) break;
			}
	    			fp = fopen(file_dir, "w");
				while(strcmp(msg,".") != 0){
					recv(cli, msg, sizeof(msg),0);
					if(strcmp(msg,".") == 0){
						strcpy(buff, "1");
					}
					else{
						strcpy(buff, "0");
					}
				fprintf(fp, "%s\n", msg);
				send(cli, buff, sizeof(buff), 0);
				}
			fclose(fp);

			// Cc
			// init buff and msg
			memset(buff, 0, sizeof(buff));
			memset(msg, 0, sizeof(msg));
			// process
			recv(cli, Cc, sizeof(msg), 0);
			printf("Connection request to \"%s\"\n\n",Cc);
	
			for(k=0;;k++){
				sprintf(file_dir,"%s/mail%d.txt", Cc, k+1);
				if ((fp = fopen(file_dir, "r")) == NULL) break;
			}
				fp = fopen(file_dir, "w");
				while(strcmp(msg,".") != 0){
					recv(cli, msg, sizeof(msg),0);
					if(strcmp(msg,".") == 0){
						strcpy(buff, "1");
					}
					else{
						strcpy(buff, "0");
					}
					fprintf(fp, "%s\n", msg);
					send(cli, buff, sizeof(buff), 0);
				}
			fclose(fp);
			close(cli);
			exit(0);		// child exits here
		}
		// parent
		else{
			close(cli);		// close parent copy
			waitpid(child_pid, NULL, 0);	// waits for child to exit
		}
	}
	close(sock);
	return 0;
}  
