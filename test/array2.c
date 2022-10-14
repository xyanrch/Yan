int main()
{
    int a[2][2];
    int i;
    int j;
    int c;
    c = 0;
   // a[]
    for(i = 0; i<2;i++)
    {
        //print(i);
        for(j = 0;j<2;j++)
        {
            a[i][j] = i*2 +j;//*(*(a+i)+j)
            print(a[i][j]);

        }

    }
      
    print(a[0][1]);
    return 0;
}
/*
Expected output:
0
1
2
3
1
*/
