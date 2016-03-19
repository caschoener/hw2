# hw2

Chris Schoener and Gabe Stanton

As per the assignment, our C program uses multiple processes to implement the wordcount application. To achieve this, we create the runtime-specified 
number of child processes. Then each child process is fed 1/n of the file through its respective pipe. The children each run the generate function to
create a linked list of word nodes (identical to last project). These lists are then passed via pipe back to the parent. Because the data is passed back 
as a character stream, the parent must reassemble the list using the listFromChild function in O(n) time. Finally, the n child-lists are merged into a 
master list using the merge function. This master list is then outputted to the specified file.

We found working with the pipes to be the most challenging part of this assignment. While working on this program, we would often run into unexpected data
being read from the pipe. While we were able to fix these issues for the most part, sometimes the first couple elements in the list will have random, non-
alphanumeric characters interspersed. We also were not able to make a parent and a child write to the same pipe at different stages in the program, so we had
to make extra pipes, one for the parent to write and child to read and one for the child to write and the parent to read, I suppose this is both an issue and 
an interesting design choice. Also the program will fail if the number of processes exceeds the number of words in the doc.

We used system calls to:	FORK to create a number of child processes
							PIPE to create pipes for inter-process comunication
							READ and WRITE to use the pipes
							
Our only interesting design choice was our decision to use read and write system calls instead of dup2 when dealing with pipes. We made this decision because
we felt that it was a clearer method of dealing with the pipes. 



CITATIONS
http://www.tutorialspoint.com/c_standard_library/c_function_ftell.htm used to find size of the file and split into 1/n pieces
http://www.geeksforgeeks.org/merge-two-sorted-linked-lists/ used to merge sorted linked lists recursively
