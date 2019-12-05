#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unisted.h>

#define MAX_BUFF 64

int main()
{
int i,sockfd;
struct sockaddr_in server;
char buf[MAX_BUFF];
char tmp[MAX_BUFF];

sockfd=socket(AF_INET,SOCK_STREAM,0);
if(sockfd<0)
{
perror("Error:");
exit(0);
}

server.sin_family=AF_INET;
server.sin_port=htons(110);
server.sin_addr.s_addr=inet_addr("192.168.10.55");

i=connect(sockfd,(struct sockaddr *)&server,sizeof(server));

if(i<0)
{
perror("Error:");
close(sockfd);
exit(0);
}

memset(buf,0,MAX);
read(sockfd,buf,MAX);
printf("\n%s\nuser name:",buf);
scanf("%s",tmp);
strcat(tmp,"\r\n");
sleep(1);
memset(buf,0,MAX);
strcpy(buf,"USER ");
strcat(buf,tmp);
write(sockfd,buf,strlen(buf));
memset(buf,0,MAX);
read(sockfd,buf,MAX);
printf("\n%s",buf);
scanf("%s",tmp);
strcat(tmp,"\r\n");
sleep(1);
memset(buf,0,MAX);
strcpy(buf,"PASS ");
strcat(buf,tmp);
write(sockfd,buf,strlen(buf));
memset(buf,0,MAX);
read(sockfd,buf,MAX);
printf("\n%s",buf);
sleep(1);
memset(buf,0,MAX);
strcpy(buf,"STAT\r\n");
write(sockfd,buf,strlen(buf));
memset(buf,0,MAX);
read(sockfd,buf,MAX);
printf("\n%s\nEnter message no:",buf);
scanf("%s",tmp);
strcat(tmp,"\r\n");
sleep(1);
memset(buf,0,MAX);
strcpy(buf,"RETR ");
strcat(buf,tmp);
write(sockfd,buf,strlen(buf));
memset(buf,0,MAX);
read(sockfd,buf,MAX);
printf("\n%s",buf);
sleep(1);
close(sockfd);
return 0;
}
