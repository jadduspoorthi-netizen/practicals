#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_ARGS 20
#define MAX_LEN 100

int main(){
char command [MAX_LEN];
char *args[MAX_ARGS];
int i=0;
printf("enter a Linex command: ");
fgets(command,sizeof(command),stdin);
command[strcspn(command,"\n")]='\0';
args[i]=strtok(command," ");
while (args[i] !=NULL && i<MAX_ARGS-1){
i++;
args[i]=strtok(NULL," ");
}
pid_t pid=fork();
if(pid<0){
printf("fork failed.\n");
exit(1);
}
else if(pid==0){
printf("\n Child process\n");
printf("child Pid: %d\n",getpid());
printf("Parent Pid: %d",getppid());
execvp(args[0],args);
perror ("Execution failed");
exit(1);
}
else{
printf("\nParent process\n");
printf("\n parent pid : %d\n",getpid());
printf("\n Child pid : %d\n",pid);
wait(NULL);
printf("\nChild process has completed execution.\n");
}
return 0;
}
