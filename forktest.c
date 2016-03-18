#include<sys/time.h>
#include<time.h>
#include<stdio.h>
#include<unistd.h>

int main(int argc, char* argv[])
{
        int numberOfProcesses = 2; //change back
        int* writePipes = malloc(numberOfProcesses*sizeof(int));
        int* readPipes = malloc(numberOfProcesses*sizeof(int));

        int newFD[2];
        int pid = -1;
        int processNum = 0;
		int i;

        for(i = 0; (i<numberOfProcesses && pid !=0);i++) //parent generates children and pipes until limit, breaks if child
        {
            pipe(newFD);
            readPipes[i] = newFD[0];
            writePipes[i] = newFD[1];

            pid = fork();
        }
        //parent sends each child a 1/n chunk of infile, then handles first chunk
        //child waits for 

        if(pid != 0)//parent side
        {
            printf("reached parent");
            for(i = 0; i<numberOfProcesses; i++) //close read pipes and write chunks
            {
                close(readPipes[i]);
                sleep(5); //wait for child pipe to close maybe
                char testmsg[] = "test write\n";
                write(writePipes[i], testmsg, sizeof(testmsg));
                sleep(5);
                write(writePipes[i], testmsg, sizeof(testmsg));
                close(writePipes[i]);
                processNum = i;
               
            }
        }

        else//child side
        {
            printf("reached child");
            close(writePipes[processNum]);

            printf(fgetc(readPipes[processNum]));
        }
}