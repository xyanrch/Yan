int main()
{
    int a;
    a = 90;
    int *p;
   // int **pp;
    //*p = 100;
    //*p =2;
     p =&a;
     a=1000;
    print(*p);
    return 0;
}

/*
Expected output:
1000
*/