
#include<sys/time.h>
#include<time.h>
#include<stdio.h>
#include<unistd.h>

typedef struct wordNode
{
	char* word;
	int count;
	struct wordNode* next;
}Node;



Node* generate(int);
Node* searchAndPlace(char*, Node*);
void outputToFile(int, Node*);




int main(int argc, char* argv[])
{
	FILE *timeF;
	
	FILE *infile = fopen(argv[1], "r");
	
	FILE *outfile = fopen(argv[2], "w");
	
	long time;	
	
	struct timeval start, end;	
	
	gettimeofday(&start, NULL);

	if (argc != 5)
	{
		printf("Incorrect number of arguments, please specify in order: input file, output file, runtime output file, and number of processes \n");
	}
    else
    {
		int numberOfProcesses = atoi(argv[4]);
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
            processNum = i;

        }
        //parent sends each child a 1/n chunk of infile, then handles first chunk
        //child waits for 

        if(pid != 0)//parent side
        {
        	FILE* file;
        	int length;
        	file = fopen(argv[1], "r");
        	fseek(file, 0, SEEK_END);
        	length = ftell(file);
        	rewind(file);

            for(i = 0; i<numberOfProcesses; i++) //close read pipes and write chunks
            {
                close(readPipes[i]);
                char c;

                while(ftell(file) < i*length/numberOfProcesses)
                {
                	c = fgetc(file)
                	write(writePipes[i], c, sizeof(char));
                	if(feof(file))
					{
						break;
					}
                }

                while (c != ' ' || '\n' || '\t')
                {
                	c = fgetc(file)
                	write(writePipes[i], c, sizeof(char));
                	if(feof(file))
					{
						break;
					}
                }
                fclose(file);
                close(writePipes[i]);
               
            }

            outputToFile(outfile, generate(infile));

        }

        else//child side
        {
            close(writePipes[processNum]);
            generate(readPipes[processNum])

        }

    }   
	
	
	

	gettimeofday(&end, NULL);
	time = (end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec);
	
	
	
	timeF = fopen(argv[3], "w");
	if(timeF == NULL)
	{
		printf("Could not open %s \n", argv[3]);
	}
	else	
	{
		fprintf(timeF,"%ld, ",time);
		
		fclose(timeF);	
	}
	return 0;
}



Node* generate(int pipe)
{	
	char* oneWord;
	

	int buf;
	Node* head;
	int i = 0;
	int first = 1; //use as boolean, true if function is processing the first word of a text
		
	
		
	
	while (read(pipe, buf, 1) != 0) //this loop runs until we reach the end of the section
	{ 
		
		i = 0;
		
		oneWord = malloc(128*sizeof(char));
		do{ //this loop runs until we have a single word stored in "oneWord"			
			if(read(pipe, buf, 1) != 0)
			{
				break;
			}				
			if(buf >= 65 && buf <= 90)
			{
				oneWord[i] = tolower(buf);
				i++;
			}
			else if((buf >= 97 && buf <= 122) || (buf >= 192 && buf <= 255) || (buf >= 48 && buf <= 57))
			{
				oneWord[i] = buf;
				i++;
			}
			
		}while(buf != ' ' && buf!= '\n' && buf!= '\t'); //words are separated by space or newline

		if(oneWord[0] != NULL) //handles multiple spaces/newlines
		{
		if(first == 1)
		{
			head = malloc(sizeof(struct wordNode));
			head -> word = oneWord;
			head -> count = 1;
			first = 0;
		}
		
		else
		{
			head = searchAndPlace(oneWord, head);
		}
		}
		
	}
	
	return head;
	
}

Node* searchAndPlace(char* key, Node* head)
{
	int found = 0; //use as boolean, false unless node containing key word is found
	int i = 0;
	Node* curr = head;
	
	while(curr != NULL && found == 0)
	{
		if(strcmp(curr -> word, key) > 0) //current word is too far alphabetically, i.e. if the word wasn't "new" we would have found it by now (for start of list)
		{
			Node *w = malloc(sizeof(*curr));
			w -> word = key;
			w -> count = 1;
			w -> next = curr;

			found = 1;
			head = w;
		}
		
		else if(strcmp(curr -> word, key) == 0) //current node contains the key word
		{
			curr -> count++;
			found = 1;
		}
	
		else if(curr -> next == NULL) //we've reached the end of the list
		{
			Node *w = malloc(sizeof(*curr));
			w -> word = key;
			w -> count = 1;
			curr -> next = w;
			
			found = 1;
		}

		else if(strcmp(curr -> next -> word, key) > 0) //the next word is too far alphabetically, i.e. if the word wasn't "new" we would have found it by now
		{
			Node *w = malloc(sizeof(*curr));
			w -> word = key;
			w -> count = 1;
			w -> next = curr -> next;
			curr -> next = w;
			
			found = 1;
		}
		
		curr = curr -> next;
	}
	return head;					
}

void outputToFile(int pipe, Node* head)
{
	

	do
	{
		fprintf(file, "%s %d\n", head -> word, head -> count);
		head = head -> next;
	}while(head != NULL);

	fclose(file);
}
