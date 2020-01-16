#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <arpa/inet.h>

#define ERR -1
#define MAX 64
#define SERVER_IP "127.0.0.1" //local loopback
#define SMTP 1900
#define POP 110

int main(){
    int sock, cli, k;
    unsigned int len;
    int sent;
    struct sockaddr_in server, client;
    char buff[MAX], msg[MAX], to[MAX], Cc[MAX], txt[]="rec.txt", file_dir[MAX];
    FILE *fp;

	//ソケットの作成
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) == ERR)
    {
        perror("socket: ");
        exit(-1);
    }


    server.sin_family = AF_INET;
    server.sin_port = htons(SMTP);
    server.sin_addr.s_addr = INADDR_ANY;

	//bind
len = sizeof(struct sockaddr_in);
if(bind(sock, (struct sockaddr *)&server, sizeof(server)) == ERR)
    {
        perror("bind");
        exit(-1);
    }

//listen
if((listen(sock, 10)) == ERR)
    {
        perror("listen");
        exit(-1);
    }

while(1)
    {

    memset(buff, 0, sizeof(buff));
    memset(msg, 0, sizeof(msg));
    memset(to, 0, sizeof(to));
    memset(Cc, 0, sizeof(Cc));
    printf("waiting for connection\n");
    //accept
    if((cli = accept(sock, (struct sockaddr *)&client, &len)) == ERR)
    {
        perror("accept");
        exit(-1);
    }
    //receive from client
    recv(cli, to, sizeof(msg), 0);
    printf("connection request to %s\n",to);

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
    //Cc
    //init buff and msg
    memset(buff, 0, sizeof(buff));
    memset(msg, 0, sizeof(msg));
    //proccess
    recv(cli, Cc, sizeof(msg), 0);
    printf("connection request to %s\n",Cc);

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
    }
close(sock);
return 0;
}  
