int test(int *a, int x)
{
    if(*a == 9)
    {
        *a = 90;
    }
    print(*a);
    return 88+x;
}

int main()
{
    
    
   // int *a;
    int b;
    
    int*a;
    int c;
    b = 9;

    
     a = &b;
    // print(*a);
    // int c;
     //c =*a;

   // b= 9;
    //*a = 90;
    print(*a);
    
    print(test(a,1));
    print(*a);
     return 0;
     
}

/*
Expected output:
9
90
89
90
*/