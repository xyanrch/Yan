int main()
{
    int a[2][2];
    int i;
    int j;
    int c;
    c = 0;   // a[]


    for(i=0; i<2;i++)    
        for(j=0;j<2;j++) 
                  print(c++);
    for(;;)
    {
        if(c++ == 100)
        break;
    }
 
  

    print(i);
    print(j);
    print(c);
    return 0;
}
/*
Expected output:
1
2
3
4
2
2
100
*/