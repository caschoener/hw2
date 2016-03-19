# hw2

Chris Schoener and Gabe Stanton

As per the assignment, our C program uses multiple processes to implement the wordcount application. To achieve this, we create the runtime-specified 
number of child processes. Then each child process is fed 1/n of the file through its respective pipe. The children each run the generate function to
create a linked list of word nodes (identical to last project). These lists are then passed via pipe back to the parent. Because the data is passed back 
as a character stream, the parent must reassemble the list using the listFromChild function in O(n) time. Finally, the n child-lists are merged into a 
master list using the merge function. This master list is then outputted to the specified file.

We found working with the pipes to be the most challenging part of this assignment. While working on this program, we would often run into unexpected data
being read from the pipe. While we were able to fix these issues for the most part, sometimes the first couple elements in the list will have random corrupt 
data. We believe this error, though minor, may cause us to fail some of the unit tests. Unfortunately, to avoid writing corrupt data to the file, we have 
output our results to the terminal. We also were not able to make a parent and a child write to the same pipe at different stages in the program, so we 
had to make extra pipes, one for the parent to write and child to read and one for the child to write and the parent to read, I suppose this is both an 
issue and an interesting design choice.

Performance Evaluation: Pangur Ban- 8 processes  		2 processes							4 processes
						 	mean:	.635 				mean:	.570						mean:	.334
							standard deviation:	.0754	standard deviation:	.0238			standard deviation:	.0399
						Hamlet-
							mean: 	.969				mean:		.656					mean:	6.023
							standard deviation: .0539	standard deviation:	.0884			standard deviation:	.992
						Arabian Nights-
							mean:	5.628				mean:	25.43						mean:	8.034
							standard deviation:	.1032	standard deviation: .883			standard deviation:	.7543

We used system calls to:	FORK to create a number of child processes
							PIPE to create pipes for inter-process comunication
							READ and WRITE to use the pipes
							
Our only interesting design choice was our decision to use read and write system calls instead of dup2 when dealing with pipes. We made this decision because
we felt that it was a clearer method of dealing with the pipes. 



CITATIONS
http://www.tutorialspoint.com/c_standard_library/c_function_ftell.htm used to find size of the file and split into 1/n pieces
http://www.geeksforgeeks.org/merge-two-sorted-linked-lists/ used to merge sorted linked lists recursively
