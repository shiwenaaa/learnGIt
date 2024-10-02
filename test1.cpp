#include <iostream>
using namespace std;

int main()
{
    int n, m, p, q, t, op;
    cin >> n >> m >> t;

    int arr[100][100], arr2[100];
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            cin >> arr[i][j];
        }
    }

    while (t--)
    {
        cin >> op >> p >> q;

        // ÖØËÜ
        if (op == 1)
        {
            int size = m * n;
            int cnt = 0;
            for (int i = 0; i < n; i++)
            {
                for (int j = 0; j < m; j++)
                {
                    arr2[cnt] = arr[i][j];
                    cnt++;
                }
            }
            cnt = 0;
            for (int i = 0; i < p; i++)
            {
                for (int j = 0; j < q; j++)
                {
                    arr[i][j] = arr2[cnt];
                    cnt++;
                }
            }
        }
        // ×ªÖÃ
        else if (op == 2)
        {
            int temp[100][100];
            for (int i = 0; i < m; i++)
            {
                for (int j = 0; j < n; j++)
                {
                    temp[i][j] = arr[j][i];
                }
            }
            for (int i = 0; i < m; i++)
            {
                for (int j = 0; j < n; j++)
                {
                    arr[i][j] = temp[i][j];
                }
            }
            int temp1;
            temp1 = n;
            n = m;
            m = temp1;
        }
        // ²éÑ¯
        else if (op == 3)
        {
            cout << arr[p][q] << endl;
        }
    }
    return 0;
}