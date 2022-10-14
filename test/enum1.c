
int main()
{
    enum rgb
    {
        R = 100,
        B = 200,
        G = 400
    };
    print(R);

    print(1+G);
    enum rgb b;
  //s int  b;
    b= B;
    print(b);
    return 0;
}
/*
Expected output:
100
401
200
*/