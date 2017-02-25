#ifndef _Queue_H
#define _Queue_H

#include <stdio.h>
#include <stdlib.h>
 
struct node
{
    int processID,remainingbursttime;
    struct node *ptr;
}*front,*rear,*temp,*front1,*beforemin,*replacebeforemin;
 
void enq(int data_1,int data_2);
int empty();
void display();
void create();
int queuesize();
int addall();
int peekprocessID();
int peekburst();
int findndel_min();
void swap();
void SelectionSort();
void deq();
int count = 0;
int previousprocessID;

void create()
{
    front = rear = NULL;
}

int queuesize()
{
    return count;
}
 
/* Enqueing the queue */
void enq(int data_1,int data_2)
{
    if (rear == NULL)
    {
        rear = (struct node *)malloc(1*sizeof(struct node));
        rear->ptr = NULL;
        rear->processID = data_1;
        rear->remainingbursttime = data_2;
        front = rear;
    }
    else
    {
        temp=(struct node *)malloc(1*sizeof(struct node));
        rear->ptr = temp;
        temp->processID = data_1;
        temp->remainingbursttime = data_2;
        temp->ptr = NULL;
        rear = temp;
    }
    count++;
}
 
void deq()
{
    front1 = front;
 
    if (front1 == NULL)
    {
        return;
    }
    else
        if (front1->ptr != NULL)
      {
            front1 = front1->ptr;
            //printf("\n Dequed value : %d", front->info);
            free(front);
            front = front1;
        }
        else
        {
            //printf("\n Dequed value : %d", front->info);
            free(front);
            front = NULL;
            rear = NULL;
        }
        count--;
}

void SelectionSort()
{
	struct node *start = front;
	struct node *traverse;
	struct node *min;
	
	while(start->ptr)
	{
		min = start;
		traverse = start->ptr;
		
		while(traverse)
		{
			/* Find minimum element from array */ 
			if(min->remainingbursttime > traverse->remainingbursttime )
			{
				min = traverse;
			}
                    traverse = traverse->ptr;
		}
		swap(start,min);			// Put minimum element on starting location
                //temp=start;
		start = start->ptr;
	}
} 
 
/* swap data field of linked list */
void swap(struct node *p1, struct node *p2)
{
	int temp = p1->remainingbursttime;
        int temp2= p1->processID;
	p1->remainingbursttime = p2->remainingbursttime;
        p1->processID = p2->processID;
	p2->remainingbursttime = temp;
        p2->processID = temp2;
}

void display()
{
    front1 = front;
 
    if ((front1 == NULL) && (rear == NULL))
    {
        printf("Queue is empty\n");
        return;
    }
    while (front1 != rear)
    {
        printf("(%d,%d)\n", front1->processID,front1->remainingbursttime);
        front1 = front1->ptr;
    }
    if (front1 == rear)
        printf("(%d,%d)\n", front1->processID,front1->remainingbursttime);
}
 
int addall()
{
  int cummulativeburst=0;
   if ((front == NULL) && (rear == NULL))
    {
        return 0;
    }
   for(front1=front;front1!=rear;front1=front1->ptr)
    {
      cummulativeburst = cummulativeburst + front1->remainingbursttime;
    }
   cummulativeburst = cummulativeburst + front1->remainingbursttime;
  return cummulativeburst;
} 

int findndel_min()
{
   if ((front == NULL) && (rear == NULL))
    {
        return 0;
    }   
   int min=front->remainingbursttime;
   replacebeforemin=front;
   temp=front;
   previousprocessID=front->processID;
   for(front1=front;front1!=rear;front1=front1->ptr) 
    {
       if(front1->remainingbursttime<min)
         {
          min=front1->remainingbursttime;
          temp=front1;
          previousprocessID=temp->processID;
          replacebeforemin=beforemin;
         }
      beforemin=front1;
    }
    if(front1->remainingbursttime<min)
         {
          min=front1->remainingbursttime;
          temp=front1;
          replacebeforemin=beforemin;
          previousprocessID=temp->processID;
         }
   //delete min, temp stores the minimum's pointer
    if(temp==front && front->ptr!=NULL)//first node is the minimum
     {
       replacebeforemin=replacebeforemin->ptr;
       free(front);
       front=replacebeforemin;
     } 
    else if(front->ptr==NULL)
     {
       free(front);
       front=NULL;
       rear=NULL;
     }  
   else if(temp==rear)//last node is minimum
     {
       rear=replacebeforemin;
       rear->ptr=NULL;
       free(temp);
     } 
   else if(front->ptr==temp)//2nd node is the minimum
     {
       front->ptr=temp->ptr;
       free(temp);
     }
   else //middle node is minimum
     {
       replacebeforemin->ptr=temp->ptr;
       free(temp);
     }    
   count--;
  return min;
}

int peekprocessID()
{
    if ((front != NULL) && (rear != NULL))
        return(front->processID);
    else
        return 0;
}

int getprocessID()
{
  return previousprocessID;
}

int peekburst()
{
    if ((front != NULL) && (rear != NULL))
        return(front->remainingbursttime);
    else
        return 0;
}
 

int empty()
{
     if ((front == NULL) && (rear == NULL))
        return 1;
    else
        return 0;
}

#endif
