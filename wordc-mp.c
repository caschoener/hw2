
#include<sys/time.h>
#include<time.h>
#include<stdio.h>

typedef struct wordNode
{
	char* word;
	int count;
	struct wordNode* next;
}Node;



Node* generate(FILE*);
Node* searchAndPlace(char*, Node*);
void outputToFile(FILE*, Node*);




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
		outputToFile(outfile, generate(infile));
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



Node* generate(FILE* file)
{
	char* oneWord;
	

	int c;
	Node* head;
	int i = 0;
	int first = 1; //use as boolean, true if function is processing the first word of a text
		
	
	
	
	
	
	if(file == NULL)
	{
		printf("Could not open file \n");
		return NULL;
	}
	else	
	{
		do{ //this loop runs until we reach the end of the file
			i = 0;
			
			oneWord = malloc(128*sizeof(char));
			do{ //this loop runs until we have a single word stored in "oneWord"
				c = fgetc(file);
				if(feof(file))
				{
					break;
				}				
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
				
			}while(c != ' ' && c!= '\n' && c!= '\t'); //words are separated by space or newline

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
			
		}while(c != EOF);
		
		fclose(file);
		return head;
	}
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

void outputToFile(FILE* file, Node* head)
{
	

	do
	{
		fprintf(file, "%s %d\n", head -> word, head -> count);
		head = head -> next;
	}while(head != NULL);

	fclose(file);
}
