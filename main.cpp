#include <iostream>
#include <cstring>
using namespace std;
typedef struct stack
{
    int i;
    int j;
};

const int maxStackSize = 100000;
const int M = 1000000;

//代表共有m个生产地点，n个销地
int m = 0, n = 0;
//prodecer[i]代表第i的生产地点的生产量
//consumer[i]代表第i个消费地点的需求量
int producer[100], consumer[100];
//c[i][j]表示从生产地i到销地j的运输费用
int c[100][100] = {0};
//base[i][j]表示从生产地i到销地j的运输量
int base[100][100] = {0};
//is_base[i][j]表示该点是否为基变量
int is_base[100][100] = {0};

//西北角法寻找问题的初始解
//第一个基变量为x11
void northwest(int i, int j)
{
    if (i >= 1 && i <= m && j >= 1 && j <= n)
    {
        is_base[i][j] = 1;
        base[i][j] = min(producer[i], consumer[j]);
        producer[i] = producer[i] - base[i][j];
        consumer[j] = consumer[j] - base[i][j];
        if (producer[i] == 0 && consumer[j] != 0)
        {
            northwest(i + 1, j);
        }
        else if (producer[i] != 0 && consumer[j] == 0)
        {
            northwest(i, j + 1);
        }
        else if (producer[i] == 0 && consumer[j] == 0)
        {
            //防止出现退化
            base[i + 1][j] = 0;
            is_base[i + 1][j] = 1;
            northwest(i + 1, j + 1);
        }
    }
}
//位势法求解，找到换入变量
int u[100], v[100];
//代表是否已经被赋值
int u_begin[100], v_begin[100];
//检验数
int o[100][100];
//若不是最优解，则返回值为值最小的检验数
//若是最优解：无穷多最优解返回1，唯一最优解返回0
int position(int *i, int *j)
{
    int a, b;
    int start = 0;
    memset(u, 0, sizeof(u));
    memset(v, 0, sizeof(v));
    memset(u_begin, 0, sizeof(u_begin));
    memset(v_begin, 0, sizeof(v_begin));
    memset(o, 0, sizeof(o));
    //令基变量xij的检验数全为0，求得相应的u[i],v[j]
    while (1)
    {
        for (a = 1; a <= m; a++)
            for (b = 1; b <= n; b++)
            {
                if (is_base[a][b] == 1)
                {
                    if (start == 0)
                    {
                        start = 1;
                        u[a] = 0;
                        u_begin[a] = 1;
                    }
                    if (u_begin[a] == 1 && v_begin[b] == 0)
                    {
                        v[b] = c[a][b] - u[a];
                        v_begin[b] = 1;
                    }
                    if (u_begin[a] == 0 && v_begin[b] == 1)
                    {
                        u[a] = c[a][b] - v[b];
                        u_begin[a] = 1;
                    }
                }
            }
        //判断是否每个u[i],v[j]都已正确赋值
        for (a = 1; a <= m; a++)
        {
            if (start == 0)
                break;
            for (b = 1; b <= n; b++)
            {
                if (is_base[a][b] == 1)
                {
                    if (u_begin[a] == 0 || v_begin[a] == 0)
                    {
                        //表示还未完全赋值
                        start = 0;
                        break;
                    }
                }
            }
        }
        if (start == 1)
            break;
    }
    //求得所有变量的检验数
    for (a = 1; a <= m; a++)
        for (b = 1; b <= n; b++)
        {
            o[a][b] = c[a][b] - u[a] - v[b];
        }
    //检查检验数的值是否正确
    for (a = 1; a <= m; a++)
        cout << "u[" << a << "]=" << u[a] << endl;
    for (b = 1; b <= n; b++)
        cout << "v[" << b << "]=" << v[b] << endl;
    for (a = 1; a <= m; a++)
        for (b = 1; b <= n; b++)
        {
            cout << "o[" << a << "][" << b << "]=" << o[a][b] << endl;
            //o[a][b] = c[a][b] - u[a] - v[b];
        }
    //返回检验数最小的i，j
    int temp_a, temp_b, k;
    k = o[1][1];
    for (a = 1; a <= m; a++)
        for (b = 1; b <= n; b++)
        {
            if (o[a][b] < k)
            {
                k = o[a][b];
                temp_b = b;
                temp_a = a;
            }
        }
    if (k == 0)
    {
        for (a = 1; a <= m; a++)
            for (b = 1; b <= n; b++)
            {
                if (is_base[a][b] == 0 && o[a][b] == 0)
                {
                    cout << "There are endless Optimal solutions" << endl;
                    *i = 0;
                    *j = 0;
                    return 1;
                }
            }
        cout << "There is only one Optimal solution" << endl;
        return 0;
    }
    if (k < 0)
    {
        *i = temp_a;
        *j = temp_b;
    }
    return k;
}

stack s[maxStackSize];
int tail = 0;
void initial_stack()
{
    tail = 0;
    for (int a = 0; a < maxStackSize; a++)
    {
        s[a].i = 0;
        s[a].j = 0;
    }
    tail = 0;
}
int is_empty_stack()
{
    if (tail == 0)
        return 1;
    return 0;
}
void push_stack(int x, int y)
{
    s[tail].i = x;
    s[tail].j = y;
    tail++;
}
void pop_stack(int *x, int *y)
{
    *x = s[tail - 1].i;
    *y = s[tail - 1].j;
    tail--;
}
typedef struct point
{
    int x;
    int y;
    int visited;
    int parent;    //指向该点的父节点
    int direction; //该点指向的方向
};
point p[maxStackSize];

//闭回路调整法找到换出变量,并进行基变量互换
//换入变量为Xij
//定义上下方向为1，左右方向为0
//返回值为闭回路终点对应的point结构体数组的下标
int closed_loop(int i, int j)
{
    cout << "i=" << i << ", j=" << j << endl;
    int a, b;
    int parents, k, w, flag;
    int timeT = 0;
    is_base[i][j] = 1;
    //运用DFS搜索找到一个闭回路
    //并对闭回路上的所有基变量进行下一步操作
    initial_stack();
    push_stack(i, j);
    p[0].x = i;
    p[0].y = j;
    p[0].visited = 0;
    p[0].parent = -1;
    p[0].direction = -1;

    int arr = 1;
    while (is_empty_stack() != 1 && timeT++ < maxStackSize)
    {
        pop_stack(&a, &b);
        cout << "pop stack!" << endl;
        cout << "a=" << a << ",b=" << b << endl;
        for (k = 0, flag = 0; k < arr; k++)
        {
            if (p[k].x == a && p[k].y == b)
            {
                if (p[k].visited == 1)
                    flag = 1;
                else
                {
                    p[k].visited = 1;
                    parents = k;
                }
            }
        }
        if (flag == 0)
        {
            //该点的邻居入栈
            cout << "aid is :" << endl;
            cout << "a=" << p[parents].x << ",b=" << p[parents].y << endl;
            cout << "visited = " << p[parents].visited << endl;
            cout << "direction = " << p[parents].direction << endl;
            if (p[parents].direction == 0)
            {
                //上下方向的邻居入栈
                for (int c = 1; c <= m; c++)
                {
                    if (is_base[c][p[parents].y] == 1)
                    {
                        if (c == p[parents].x)
                            continue;
                        push_stack(c, p[parents].y);
                        cout << "push stack!" << endl;
                        cout << "a=" << c << ",b=" << p[parents].y << endl;
                        p[arr].x = c;
                        p[arr].y = p[parents].y;
                        p[arr].visited = 0;
                        p[arr].direction = 1;
                        p[arr].parent = parents; //parent为数组的下标
                        if (c == i && p[parents].y == j)
                            return arr;
                        arr++;
                    }
                }
            }
            else
            {
                //左右方向的邻居入栈
                for (int c = 1; c <= n; c++)
                {
                    if (is_base[p[parents].x][c] == 1)
                    {
                        if (c == p[parents].y)
                            continue;
                        push_stack(p[parents].x, c);
                        cout << "push stack!" << endl;
                        cout << "a=" << p[parents].x << ",b=" << c << endl;
                        p[arr].x = p[parents].x;
                        p[arr].y = c;
                        p[arr].visited = 0;
                        p[arr].direction = 0;
                        p[arr].parent = parents;
                        if (p[parents].x == i && c == j)
                            return arr;
                        arr++;
                    }
                }
            }
      
        }
    }

    return 0;
}

int main()
{
    int temp, s, t;
    int temp_x, temp_y;
    int sum_produce = 0, sum_consume = 0; //分别表示总产量与总销量
    int i = 0, j = 0;
    //flag为解决产销不平衡的一个变量
    //flag为1代表增加了销地，为2代表增加了产地，为0代表未增加任何产地与销地
    //flag默认为0
    int flag = 0;
    memset(c, 0, sizeof(c));
    memset(base, 0, sizeof(base));
    memset(consumer, 0, sizeof(consumer));
    memset(producer, 0, sizeof(producer));
    cout << "Input the total of producer:";
    cin >> m;
    cout << "Input the total of consumer:";
    cin >> n;
    for (i = 1; i <= m; i++)
    {
        cout << "Input the production of producer " << i << " :";
        cin >> producer[i];
        sum_produce += producer[i];
    }
    for (i = 1; i <= n; i++)
    {
        cout << "Input the consumption of consumer " << i << " :";
        cin >> consumer[i];
        sum_consume += consumer[i];
    }
    cout << "sum_produce = " << sum_produce << ",sum_consume = " << sum_consume << endl;
    if (sum_produce > sum_consume)
    {
        //产大于销，则假想一个销地,其单位运价为0
        //其销量为sum_produce-sum_consume
        consumer[n + 1] = sum_produce - sum_consume;
        flag = 1;
    }
    else if (sum_produce < sum_consume)
    {
        //销大于产，则假想一个产地，其单位运价为0
        //其产量为sum_consume-sum_prodece
        producer[m + 1] = sum_consume - sum_produce;
        flag = 2;
    }

    for (i = 1; i <= m; i++)
        for (j = 1; j <= n; j++)
        {
            cout << "Input the cost from producer " << i << " to consumer " << j << " :";
            cin >> c[i][j];
        }
    if (flag == 1)
    {
        for (i = 1; i <= m; i++)
            c[i][n + 1] = 0;
        n++;
    }
    if (flag == 2)
    {
        for (j = 1; j <= n; j++)
            c[m + 1][j] = 0;
        m++;
    }

    northwest(1, 1);
    //测试初始基可行解
    for (i = 1; i <= m; i++)
        for (j = 1; j <= n; j++)
        {
            if (is_base[i][j] == 1)
            {
                cout << "base[" << i << "][" << j << "]=" << base[i][j] << endl;
            }
        }
    //位势法求最优解，找到换入变量
    int o = position(&i, &j);
    while (o < 0)
    {
        int arr = closed_loop(i, j);
        temp = arr;
        if (arr == 0)
            cout << "error!" << endl;
        else
        {
            //找到-1项中数值最小的基变量
            s = 1;
            t = maxStackSize;
            while (temp != 0)
            {
                if (s % 2 == 0)
                {
                    if (base[p[temp].x][p[temp].y] < t)
                    {
                        t = base[p[temp].x][p[temp].y];
                        temp_x = p[temp].x;
                        temp_y = p[temp].y;
                    }
                }
                s++;
                //用于测试结果是否正确
                //cout << "x=" << p[arr].x << ",y=" << p[arr].y << endl;
                temp = p[temp].parent;
            }
            cout << "temp_x=" << temp_x << ",temp_y=" << temp_y << endl;
            is_base[temp_x][temp_y] = 0;
            temp = arr;
            s = 1;
            while (temp != 0)
            {
                if (s % 2 == 0)

                    base[p[temp].x][p[temp].y] -= t;
                else
                    base[p[temp].x][p[temp].y] += t;
                s++;
                temp = p[temp].parent;
            }
        }
        o = position(&i, &j);
    }
    cout << "solution is : " << endl;
    int cost = 0;
    if (flag == 1)
        n--;
    if (flag == 2)
        m--;
    for (i = 1; i <= m; i++)
        for (j = 1; j <= n; j++)
        {
            if (is_base[i][j] == 1)
            {
                cout << "The transportation form producer " << i << " to consumer " << j << " is " << base[i][j] << endl;
                //cout << "base[" << i << "][" << j << "]=" << base[i][j] << endl;
                cost += base[i][j] * c[i][j];
            }
        }
    cout << "The minimum total cost is " << cost << endl;
    return 0;
}