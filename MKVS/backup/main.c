/*This program is realize the improved correlation algorithm.
The algorithm memory effect was avoided

Version 0.1


Function description:
1. EK_start - initialization and first open the files
2. Make_Real_Signal - function that to manage the real EK signals like echo signal, residual echo signal. Also manage the active signal
3. Make_Virtual_Signal - function that make service residual echo signal
4. Algorithm_vector - calculate delta H for adaptation
5. Filter_response - function that adapt pulse response of filter

*/


#include <stdio.h>
#include <stdlib.h>

int N, M, Ro;
char ActiveFile[255], EchoFile[255], ResultFile[255], Response[255];

float deltaH[255], H[255];
float Active_signal[255], Echo_signal[255], Residual_signal[255];
float Service_signal[255];


void Get_parameters ();
void EK_start();
void Real_signals(short int x, short int e);
void Virtual_signals();
void Algorithm_vector();


int main()
{
 short int xi, ei, zi;
 int i;
 float temp1;

 //get user parameters
    Get_parameters ();

// Open the input signal files
    FILE *active_signal_pointer;
    FILE *echo_signal_pointer;
    active_signal_pointer=fopen(ActiveFile, "rb");
    echo_signal_pointer=fopen(EchoFile, "rb");

// Open the output files
    FILE *residual_signal_pointer;
    FILE *response_pointer;
    residual_signal_pointer=fopen(ResultFile, "wb");
    response_pointer=fopen(Response, "wb");

// Start initialization
    EK_start();

//start echoconceller
while(!feof(active_signal_pointer)&&!feof(echo_signal_pointer))
{
fread(&xi, sizeof(short int), 1, active_signal_pointer);
fread(&ei, sizeof(short int), 1, echo_signal_pointer);

Real_signals(xi, ei);
zi=(short int)Residual_signal[0];
fwrite(&zi, sizeof(short int), 1, residual_signal_pointer);

Virtual_signals();
Algorithm_vector();

}

for(i=0;i<=M;i++)
{
fputs("H[i]=\n", response_pointer);
temp1 = H[i];
fwrite(&temp1, sizeof(float), 1, response_pointer);
}

//This is the end. Close all files
fclose(active_signal_pointer);
fclose(residual_signal_pointer);
fclose(echo_signal_pointer);
fclose(response_pointer);


    return 0;
}


void Get_parameters()
{
 //this function take parameters from user
 printf("Here is correlation echoconceller program\n\n\n");

 printf("Please enter the number of filter tips:\n");
 scanf("%i", &M);

 printf("Please enter the number of signal sample for calculation of correlation:\n");
 scanf("%i", &N);

 printf("Please enter the adaptation parameter:\n");
 scanf("%i", &Ro);


 printf("Please enter the active signal file name:\n");
 scanf("%s", ActiveFile);

 printf("Please enter the echo signal file name:\n");
 scanf("%s", EchoFile);

 printf("Please enter the residual signal file name:\n");
 scanf("%s", ResultFile);

 printf("Please enter the pulse response file name:\n");
 scanf("%s", Response);
}



void EK_start ()
{
 // this function initialaze the variables and opens the files

int i;

// printf("We are in the start\n");

for(i=0; i<=M; i++)
{
    deltaH[i]=0;
    H[i]=0;
}

for(i=0; i<=N; i++)
{
   Active_signal[i]=0;
   Echo_signal[i]=0;
   Residual_signal[i]=0;
   Service_signal[i]=0;
}

return;

}

void Real_signals(short int x, short int e)
{
    int i;
    float s;

// printf("We are in the real signal blocs\n");

// move the array
    for(i=0; i<=254; i++)
    { Active_signal[255-i]=Active_signal[254-i];
      Echo_signal [255-i]=Echo_signal[254-i];
      Residual_signal[255-i]=Residual_signal[254-i]; // this array no need?
    }

//new members of arrays
    Active_signal[0]=(float)x;
    Echo_signal [0]=(float)e;

// residual echo signal
    s=0;
    for(i=0; i<=M; i++)
        {
         s=s+Active_signal[i]*H[i];
        }
    Residual_signal[0]=Echo_signal[0]-s;

return;

}


void Virtual_signals()
{
    int i,j;
    float s_virt;

//    printf("We are in the virtual signal blocs\n");

    for (j=0; j<=255-M;j++)
    {
        s_virt=0;
        for (i=0; i<=M; i++)
    {
        s_virt=s_virt + Active_signal[i+j]*H[i];
    }
    Service_signal[j]=Echo_signal[j]-s_virt;
    }

    return;
}

void Algorithm_vector()
{
    int i,j;
    float Dx, VKF;

//   printf("We are in the vector calculation blocs\n");

for (i=0; i<=M; i++)
{
    Dx=0.00001;
    VKF=0;

// calculate the disspersiion
   for(j=0; j<=N; j++)
   {
       Dx=Dx+Active_signal[j+i]*Active_signal[j+i]; //slow way
   }

// calculate the VKF
   for(j=0; j<=N; j++)
   {
       VKF=VKF+Active_signal[j+i]*Service_signal[j];
   }
// current value of pulse response
   H[i]=H[i]+VKF/(Dx*Ro);
   deltaH[i]=VKF/(Dx*Ro);
   }

   return;
}
