#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>

#define NUM_OF_ARGS (5)
#define MAX_OF_ARGS (20)

#define MEM_DEV_PATH "/dev/mem"
#define MAX_NUM_OF_VALUE (16)
#define CONV_BASE (16)

void printHelp(void);
int checkArgs(int argc,char* argv[]);

char wr = '\0';
off_t base = -1;
off_t offset = -1;
long num_to_read = -1;
int values[MAX_NUM_OF_VALUE];

int main(int argc,char* argv[]){
	int fd = -1;
	int i;
	int res = -1;
	void* ptr = NULL;
	
	res = checkArgs(argc,argv);
	if(res != 0){
		printHelp();
		return -1;
	}
	
	fd = open(MEM_DEV_PATH, O_RDWR|O_SYNC);
	if(fd < 0){
		printf("%s open error! | %s\n",MEM_DEV_PATH,strerror(errno));
		return -1;
	}
	if(wr == 'r'){
		ptr = mmap(NULL,num_to_read,PROT_READ,MAP_SHARED,fd,base);
		if(ptr == MAP_FAILED){
			printf("%s map for read error! | %s\n",MEM_DEV_PATH,strerror(errno));
			
			close(fd);

			return -1;
		}
		for(i = 0;i < num_to_read;i++){
			printf("0x%08lX ",base+4*i+offset);
			printf("0x%08lX",*((int*)(ptr + offset) + i));
			printf("\n");
		}
		munmap(ptr,num_to_read);
	}else if(wr == 'w'){
		i = 0;
		while(values[i] != 0) printf("0x%08X ",values[i++]);
		printf("\n");
		ptr = mmap(NULL,argc - 4,PROT_WRITE,MAP_SHARED,fd,base);
		if(ptr == MAP_FAILED){
			printf("%s map for write error! | %s\n",MEM_DEV_PATH,strerror(errno));
			
			close(fd);
		
			return -1;
		}
		for(i = 0;i < argc - 4;i++){
			*((int*)(ptr + offset) + i) = values[i];
		}
		munmap(ptr,argc - 4);
	}else{
		printf("Cowardly do nothing!\n");
		return -1;
	}
	return 0;
}

void printHelp(void){
	printf("\n--------------Help for MemManipulator--------------\n");
	printf("\nmemtool <r> <base> <offset> <num_of_words>\n");
	printf("\nmemtool <w> <base> <offset> <value1> <value2> <value3> ...\n");
	printf("\nwhere\t\"r/w\" r for read or w for write\n");
	printf("\n\t\"base\" is for base address\n");
	printf("\n\t\"offset\" is for offset value\n");
	printf("\n\t\"num_of_words\" is for number of words to read\n");
	printf("\n\t\"value 1 2 3 ... 16(max)\" is for a series of words to write\n");
	printf("\n--------------END OF HELP INFORMATION--------------\n");
}

int checkArgs(int argc,char* argv[]){
	int i,j;
	if(argc < NUM_OF_ARGS){
		printf("insufficient arguments supplied!\n");
		return -1;
	}
	errno = 0;
	base = (off_t)strtol(argv[2],NULL,CONV_BASE);
	if(errno != 0){
		printf("wrong base value!%s\n",strerror(errno));
		return -1;
	}
	errno = 0;
	offset = (off_t)strtol(argv[3],NULL,CONV_BASE);
	if(errno != 0){
		printf("wrong offset value!%s\n",strerror(errno));
		return -1;
	}
	if(strlen(argv[1]) != 1){
		printf("only single letter of \"r/w\" is allowed!\n");
		return -1;
	}else if(argv[1][0] == 'r'){
		wr = 'r';
		if(argc != NUM_OF_ARGS){
			printf("invalid number (%d) of arguments for read!\n",argc);
			return -1;
		}
		errno = 0;
		num_to_read = strtol(argv[4],NULL,10);
		if(errno < 0){
			printf("invalid number of bytes to read!%s\n",strerror(errno));
			return -1;
		}
		if(num_to_read <= 0){
			printf("invalid num_to_read = %ld!\n",num_to_read);
			return -1;
		}
	}else if(argv[1][0] == 'w'){
		wr = 'w';
		if(argc > MAX_OF_ARGS){
			printf("too many values to write!\n");
			return -1;
		}
		memset(values,0,sizeof(values));
		for(i = 4,j = 0;i < argc;i++,j++){
			errno = 0;
			values[j] = strtol(argv[i],NULL,CONV_BASE);
			if(errno < 0){
				printf("invalid long value for %s!(%s)\n",argv[i],strerror(errno));
				return -1;
			}
		}
	}else{
		printf("only a letter of \"r/w\" is allowed!\n");
		return -1;
	}
	return 0;
}

