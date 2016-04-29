#include<iostream>
#include<vector>
using namespace std;
int main()
{
	int a,b;
	cin >> a>>b;
	vector<int> m;
	int Bcost = 0;
    int Acost = 0;
	int j;
	for (int i = 0; i < a; i++)
	{
		Bcost += abs(b - (i + 1));

	}

	for (int i = 1; i <= a; i++)

	{

        for(j=1;j<= a; j++)

        {

		   Acost+=abs(i-j);
        }
		if (Acost < Bcost)
		{


			cout << i;
			return j;
		}
		Acost = 0;

	}
      return 0;
}