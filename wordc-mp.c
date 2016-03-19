
#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct wordNode
{
	char* word;
	int count;
	struct wordNode* next;
}Node;

Node* merge(Node*, Node*);
Node* listFromChild(int);
Node* generate(int);
Node* searchAndPlace(char*, Node*);
void outputToPipe(int, Node*);




int main(int argc, char* argv[])
{
	FILE *timeF;

	FILE *file = fopen(argv[1], "r");

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

        int* writePipes2 = malloc(numberOfProcesses*sizeof(int));
        int* readPipes2 = malloc(numberOfProcesses*sizeof(int));

        int newFD[2];
        int newFD2[2];

        int pid = -1;
        int processNum = 0;
		int i;

        for(i = 0; (i<numberOfProcesses && pid !=0);i++) //parent generates children and pipes until limit, breaks if child
        {
            pipe(newFD);
            readPipes[i] = newFD[0];
            writePipes[i] = newFD[1];
            pipe2(newFD2);
            readPipes2[i] = newFD2[0];
            writePipes2[i] = newFD2[1];

            pid = fork();
            processNum = i;

        }
        //parent sends each child a 1/n chunk of infile, then handles first chunk

        if(pid != 0)//parent side
        {
        	int length;

        	fseek(file, 0, SEEK_END);
        	length = ftell(file);

        	rewind(file);

            for(i = 0; i<(numberOfProcesses); i++) //close read pipes and write chunks
            {
        		sleep(.1); //avoid race conditions
                close(readPipes[i]);
                char *c = malloc(sizeof(char));

                while(ftell(file) < (i+1)*length/numberOfProcesses)
                {
                	*c = fgetc(file);
                	write(writePipes[i], c, 1);

                }

                while (*c != ' ' && *c != '\n' && *c != '\t' && *c != EOF)
                {
                	*c = fgetc(file);

                	write(writePipes[i], c, 1);

                }

                close(writePipes[i]);
            }

            fclose(file);

			Node** headArray = malloc(numberOfProcesses * sizeof(struct wordNode));
			for(i = 0; i < numberOfProcesses; i++)
			{
				open(readPipes2[i]);

				headArray[i] = listFromChild(readPipes2[i]);

				printf("%s %i \n", headArray[i] -> word, headArray[i] -> count);
			}

			Node* finalHead = headArray[0];
			
			
			for(i = 1; i < numberOfProcesses; i++)
			{

				finalHead = merge(finalHead, headArray[i]);
			}
 
				do
				{
					//printf("%s %i \n", finalHead -> word, finalHead -> count);
					finalHead = finalHead -> next;
				}while(finalHead != NULL);



        }

        else//child side
        {
            close(writePipes[processNum]);

            Node * head = generate(readPipes[processNum]);
			close(readPipes[processNum]);

			outputToPipe(writePipes2[processNum], head);


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

Node* merge(Node* a, Node* b) 
{
	Node* result = NULL;
	 		printf("merge\n");

	  /* Base cases */
	if (a == NULL) 
		return(b);
	else if (b==NULL) 
		return(a);
	 
	/* Pick either a or b, and recur */
	if (strcmp(a->word, b->word) < 0) //string a < string b  
	{
		result = a;
		result->next = merge(a->next, b);
	}
	else if(strcmp(a->word, b->word) == 0) //string a = string b
	{

		a->count += b->count;
		result = a;
		result->next = merge(a->next, b->next);
			
	}
	else
	{

		result = b;
		result->next = merge(a, b->next);
	}
	
	return(result);
}


Node* listFromChild(int pipe)
{	
	char* oneWord;

	char buf;
	Node* head;
	Node* curr;
	int i = 0;
	int first = 1; //use as boolean, true if function is processing the first word of a text
		
	
		
	
	do //this loop runs until we reach the end of the input
	{ 
		
		i = 0;
		
		
		char *numArr = malloc(10*sizeof(char));
		char *oneWord = malloc(128*sizeof(char));
		do{ //this loop runs until we have a single word stored in "oneWord"			
			if(read(pipe, &buf, 1) == 0)
			{
				break;
			}			
			
			oneWord[i] = buf;
			i++;

		
			
		}while(buf != ' ' && buf!= '\n' && buf!= '\t'); //words are separated by space or newline
		
			while (buf == ' ')
				{
					read(pipe, &buf, 1);
				}	
		if(oneWord[0] != NULL) //handles multiple spaces/newlines
		{
			do{ //this loop runs until we have the corresponding number to "oneWord" word stored in "numArr"			
				

				
				numArr[i] = buf;
				i++;
				if(read(pipe, &buf, 1) == 0)
				{
					break;
				}
			
				
			}while(buf != ' ' && buf!= '\n' && buf!= '\t'); //words are separated by space or newline

			if(first == 1)
			{
				char wordcount[5];
				strcpy(wordcount, numArr);
				head = malloc(sizeof(struct wordNode));
				head -> word = oneWord;
				head -> count = atoi(wordcount);
				first = 0;
				curr = head;
			}
			else
			{
				char wordcount[5];
				strcpy(wordcount, numArr);
				Node *w = malloc(sizeof(*curr));
				w -> word = oneWord;
				w -> count = atoi(wordcount);
				curr -> next = w;
			}
		}
		
	}while (buf != EOF);
	
	return head;
	
}

Node* generate(int pipe)
{	
	char* oneWord;
	

	char c;
	Node* head;
	int i = 0;
	int first = 1; //use as boolean, true if function is processing the first word of a text
		
		
	while(read(pipe, &c, 1)!=0) 
	{ //this loop runs until we reach the end of the file
			i = 0;
			oneWord = malloc(128*sizeof(char));
			while(c != ' ' && c!= '\n' && c!= '\t')
			{						
				if(c >= 65 && c <= 90)
				{
					oneWord[i] = tolower(c);
					i++;
				}
				else if((c >= 97 && c <= 122) || (c >= 192 && c <= 255) || (c >= 48 && c <= 57))
				{
					oneWord[i] = c;
					i++;
				}
				read(pipe, &c, 1);
				
			}

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

void outputToPipe(int pipe, Node* head)
{
;	
	char space[1] = " ";
	char count[100];
	do
	{
		snprintf(count, 100, "%d", head->count);	

		write(pipe, head->word, strlen(head->word));
		write(pipe, space, 1);
		write(pipe, count, strlen(count));
		write(pipe, space, 1);

		head = head -> next;

	}while(head!=NULL);
	

	
	// do
	// {
	// 	printf("%s %i \n", head -> word, head -> count);
	// 	head = head -> next;
	// }while(head != NULL);

	close(pipe);
}
