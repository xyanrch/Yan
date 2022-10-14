//#include<stdio.h>
// void print(int x)
// {
//     printf("%d",x);
//     return;
// }
int pp(int *p,int x)
{
 print(p[x]);
 return 0;
}


int main()
{
    int a[10];
    
    a[9] = 9;
    pp(a,9);
    print(a[9]);
   // print(a);

  return 0;
}
/*
Expected output:
9
9
*/