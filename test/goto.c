void main()
{
    int i;
    i = 0;
    while (i<10)
    {
        i = i+1;
        if(i>5)
            goto label;
        else
        {
            goto ll;
        }
        
    }
    label:
    print(1);
    ll:
    print(2);
    return;

}
/*
Expected output:
2
*/