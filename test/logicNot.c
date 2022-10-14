int main()
{
    int a;
    int b;
    a=4;
    b =5;
    if(!a)
    {
        printstr("!a== true\n");
    }
    else
    {
        printstr("!a==false\n");
    }

    if(!(a==b))
    {
        print(a+b);
    }
    return 0;
    
}
/*
Expected output:
!a==false
9
*/