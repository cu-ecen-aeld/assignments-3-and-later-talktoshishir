#include<stdio.h>
#include<stdlib.h>
#include<syslog.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>


/* 
 Take input : 
 1. Filename along with path (Assumption: Path already present)
 2. String to be written to file 
 Check if argc is not equal to 3, give error and exit(1)
 Now create file in argv[1], and error(1) incase of unseccessful attempt.
 Write the strting to the file and again error(10, in case of unsuccessful attempt.
*/



int main(int argc, char* argv[]){

	openlog(NULL,0,LOG_USER);
	if(argc != 3){
		syslog(LOG_ERR,"Invalid number of arguments: %d",argc);	
		exit(1);
	}
	else{
		while(--argc > 0){
			printf("argv[%d] = %s\n",argc, argv[argc]);
		}
	}

	int fd;
	fd = open(argv[1],O_CREAT|O_WRONLY|O_APPEND, S_IRWXU|S_IRWXG|S_IRWXO);
	if(fd == -1){
		syslog(LOG_ERR,"Not able to open the file %s", argv[1]);
		exit(1);
	}
	else
	{
		syslog(LOG_DEBUG,"Writing %s to %s", argv[2], argv[1]);
		const char* buf = argv[2];
		ssize_t nr = write(fd, buf, strlen(buf));
		if(nr == -1){
			syslog(LOG_ERR,"Not able to write %s to the file %s", buf, argv[1]);
			exit(1);
		}
		else{
			if(close(fd) == -1)
				perror("close");
		}
	}	



	return 0;
}
