#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include<sys/sysinfo.h>
#include<iostream>
#include<fstream>
#include<semaphore.h>
using namespace std;
void PrintResult(int **res,int v1,int v2);
int *count;          //help this array in different threads to store count of entries.Size of this array =available noOfProcessor 

//int* count1;//for getting count how many entries calculated by one thread 

   
 sem_t sem1;
 
int ** matrix1;
int **matrix2;

int l1=0;
int l2=0;  //Actually these are use for to maintain the record, where our last threads counts comppleted and its stops the calculation

 //l1=l2=0;  //initally, initialize with zero

int r1,c1,r2,c2;   //for rows and column of both matrixes
int **outmatrix;   //for output of matrix  
      
bool matrixReading()
{  


        fstream fin;
	fin.open("m1.txt");
	fin>>r1;     //First Matrix Rows
	fin.ignore();//ignore semicolon

	fin>>c1;     //First Matrix Columns
	fstream fin2;
	fin2.open("m2.txt");
	fin2>>r2;     //Second Matrix Rows
	fin2.ignore();  //ignore semicolon
	fin2>>c2;     //Second Matrix Columns
	
	//the reason to check  multiplication can be occur or not is to aviod extre operation if multiplication not exist 
	 
	  if(c1!=r2 || c1==0|| r1 == 0|| c2==0|| r2==0)
	  {
	  
	  
	  fin.close();         //close the files
	  fin2.close();
	  return false; 
	   
	  }
        if(c1==r2)    //check our multiplication can occur or not
        {
	//Dynamic alloction of Memory
       matrix1=new int*[r1];
       for(int i=0;i<r1;i++)
       {
       
       matrix1[i]=new int[c1];
       
       
       }
	for(int i=0;i<r1;i++)
	{
		for(int j=0;j<c1;j++)
		{
			fin>>matrix1[i][j];   //First Matrix
		}
	}
	
	
        matrix2=new int*[r2];
       for(int i=0;i<r2;i++)
       {
       
       matrix2[i]=new int[c2];
       
       
       }
	
	for(int i=0;i<r2;i++)
	{
		for(int j=0;j<c1;j++)
		{
			fin2>>matrix2[i][j];  //Second Matrix
		}
	}
	
	}
	//close both files
         fin.close();
         fin2.close();
return true;
	

}
void* Multiplication(void* arg)
{
  
     
  int key=0;

   int cnt=0;
	sem_wait(&sem1);	//Using Semaphore For synchronization

	
	for(int i=l1;i<r1;i++)   //Only One thread will Excute the outer Most loop
	{

		for(int j=l2;j<c2;j++)
		{
			for(int k=0;k<c1;k++)
			{
		
		  key=key+matrix1[i][k]*matrix2[k][j];
	         
	         //outmatrix[i][j]+=matrix1[i][k]*matrix2[k][j];  //Multiplication      
               cnt++;

	         
	         	}

          	           
          	            outmatrix[i][j]=key;
          	            key=0;
          	            
          	            //division of entries on different threads
          	            if(cnt==count[i])
          	            {
          	              cnt=0;
          	              l1=i;
          	            if(j==c2-1)
          	            {
          	            
          	                l1=i+1;
          	            
          	            }
          	            l2=j+1;
          	            
          	            sem_post(&sem1);  //signaling
                            pthread_exit(NULL);  //if entries are remaining

          	            }
          	            
          }
	
	}

	
	
          sem_post(&sem1);  //signaling
         pthread_exit(NULL);  //if all entries have been calculated

    }
void PrintResult(int **res,int v1,int v2)
{
	for(int i=0;i<v1;i++)
	{
		cout<<endl;
		for(int j=0;j<v2;j++)
			cout<<"  "<<res[i][j];
	}
}
int main()
{
 
 
//cout<<"danger"<<endl;
int div1=0;  //check wether count of entries of specific thread calculated or not

  if(matrixReading()) //if Multiplication is not possible gives error
 
  {
  int TotEntr=r1*c2;    //total entries in resultant matrix
   

   
   int nfp=get_nprocs();  //find number of processor
   

   
   sem_init(&sem1,0,1);   //semaphore to avoid race condition
   
    count=new int[nfp];  //global array

   int TotEntr1=TotEntr;
   
   div1=(TotEntr1/nfp);   //store how much entries will be calculated by specific thread

   
   int mod=(TotEntr%nfp); //how many entries  left in evenly dividision of entries 
   

  for(int i=0;i<nfp;i++)
  {
  

  count[i]=div1;

  
  }
  //below loop needs explanation which is given below
  //for instance we have two matrix 5x2 and 2x5.Its mean our resultant matrix of order 5x5. And we have 9 processor.Its mean if divide evenly every thread calculte ONLY TWO entries so other seven entries again divide evenly on 7 processor calculte 3 entries and 2 processor calculate 2 entries
  
  for(int i=0;i<mod;i++)
  {

  
  count[i]=count[i]+1;
     
  
  }

  


pthread_t *tid=new pthread_t[nfp];   //number of thread id equal to number of processor
    
       pthread_t tid1; 
       outmatrix=new int*[r1];
       for(int i=0;i<r1;i++)
       {
       
       outmatrix[i]=new int[c2];
       
       
       }
  
  
    
       for(int i=0;i<nfp;i++)
         {


  
     if(pthread_create(&tid[i],NULL,Multiplication,(void*)NULL)!=0)
     perror("Error in creation of thread");
          }
      for(int i=0;i<nfp;i++)
        {
     pthread_join(tid[i],NULL);
  
     }
 //PrintResult(outmatrix,r1,c2);
 
 //write output on txt file
 fstream fin2;
	fin2.open("out1.txt");
	fin2<<r1;     //Out Matrix Rows 
	fin2<<":";    //write semicolon
        fin2<<c2;
        fin2<<endl;
 for(int i=0;i<r1;i++)
	{
	
		for(int j=0;j<c2;j++)
		{
			fin2<<" "<<outmatrix[i][j];
			
		}
		fin2<<endl;
	}
   cout<<"Data write on  files successfully"<<endl;
   fin2.close(); //closing the output file
 
 delete []count;  //destroy dynamic memory
 delete [] tid;   //destroy dunamically created thread id array
 
 for(int i=0;i<r1;i++)  //deletion of 2D matrix
 {
 
 delete []outmatrix[i];
 
 }
 delete outmatrix;
 }
 else 
   cout<<"Oops!Multiplication can not occur."<<endl;
return 0;

}//