#include<iostream>
#include <cmath>
#include<fstream>
#include<vector>
#include<sstream>
#include <algorithm>
using namespace std;

#define COMMA_NUM 3 //database中每行信息的逗号数
#define VIHICLE_CAPACITY 9 //车辆最大载量

class Node
{
public:
	float N_x;
	float N_y;
	float N_demand; //需求
	int N_index; //序号
	Node(float x, float y, float demand, int index)
	{
		N_x = x;
		N_y = y;
		N_demand = demand;
		N_index = index;
	}
};

class Saving
{
public:
	int A_index; //A点的编码号
	int B_index;
	float SavingScore;
	Saving(int a, int b, float s)
	{
		A_index = a;
		B_index = b;
		SavingScore = s;
	}
};

class Routing
{
public:
	float mileage; //路线里程
	float routingLoad; //载重
	vector<int> route; //路线
	Routing()
	{
		routingLoad = 0;
		mileage = 0;
		route.push_back(0);
	}
};

int StringToInt(string str)//用于读文件时，将string类型数据转化成int类型
{
	stringstream sf;
	int f = 0;
	sf << str;
	sf >> f;
	return f;
}

float StringToFloat(string str)//用于读文件时，将string类型数据转化成float类型
{
	stringstream sf;
	float f = 0;
	sf << str;
	sf >> f;
	return f;
}

int pos(string T, string P, int n) //查找database中“，”第n次出现的位置
{
	//if (n == 0)return -1;//相当于没找
	int count = 0;
	unsigned begined = 0;
	while ((begined = T.find(P, begined)) != string::npos) {
		count++;
		begined += P.length();
		if (n == count) {
			return begined - 1;
		}
	}
	//return -1;//意味着没有那么多次
}

vector<Node> GetFile(const char* database) //读取txt文件中的点的信息，返回vector<Node> dataset
{
	vector<Node> dataset;
	ifstream ifs;
	ifs.open(database, ios::in);
	if (!ifs.is_open())
	{
		cout << "文件打开失败！" << endl;
		return dataset;
	}
	string buf;
	while (getline(ifs, buf))
	{
		float x, y, demand;
		int index;
		for (int i = 1; i < COMMA_NUM + 1; i++)
		{
			if (i == 1)
			{
				x = StringToFloat(buf.substr(0, pos(buf, ",", i)));
			}
			if (i == 2)
			{
				y = StringToFloat(buf.substr(pos(buf, ",", i - 1) + 1, pos(buf, ",", i) - pos(buf, ",", i - 1) + 1));
			}
			if (i == 3)
			{
				demand = StringToFloat(buf.substr(pos(buf, ",", i - 1) + 1, pos(buf, ",", i) - pos(buf, ",", i - 1) + 1));
				index = StringToInt(buf.substr(pos(buf, ",", i) + 1));
			}
		}
		Node n(x, y, demand, index);
		dataset.push_back(n);
	}
	return dataset;
}

vector<vector<float>> GetDistanceMatrix(vector<Node> database) //得到距离矩阵
{
	vector <vector<float>> DistanceMatrix;
	for (int i = 0; i < database.size(); i++)
	{
		vector<float> temp;
		DistanceMatrix.push_back(temp);
		for (int j = 0; j < database.size(); j++)
		{
			float tempDistance = sqrt((database[i].N_x - database[j].N_x) * (database[i].N_x - database[j].N_x) + (database[i].N_y - database[j].N_y) * (database[i].N_y - database[j].N_y));
			DistanceMatrix[i].push_back(tempDistance);
		}
	}
	return DistanceMatrix;
}

void SavingSort(vector<Saving>& v) //基于选择排序，按Saving.SavingScore从大到小排序
{
	int len = v.size();
	Saving temp(0, 0, 0);
	for (int i = 0; i < len; i++)
	{
		for (int j = i + 1; j < len; j++)
		{
			if (v[i].SavingScore <= v[j].SavingScore)
			{
				temp = v[i];
				v[i] = v[j];
				v[j] = temp;
			}
		}
	}
}

void OutputResult(vector<Routing>& Finalresult) //输出结果
{
	float totalDistance = 0;
	int count = 1;
	ofstream ofs;
	ofs.open("Saving Hueristics.txt", ios::out);
	for (int i = 0; i < Finalresult.size(); i++)
	{
		totalDistance += Finalresult[i].mileage;
		if (Finalresult[i].routingLoad != 0)
		{
			ofs << "第" << count << "条路线： ";
			for (int j = 0; j < Finalresult[i].route.size(); j++)
			{
				ofs << Finalresult[i].route[j] << " → ";
			}
			ofs << "0" << "\t" << "路径行程： " << Finalresult[i].mileage << "\t" << "共计载重： " << Finalresult[i].routingLoad << endl;
			count++;
		}
	}
	ofs << "总路程： " << totalDistance << endl;
	ofs.close();
}

void Saving_Hueristics(vector<Node> dataset)
{
	// 获得从大到小排序好的节约值数组
	vector<Saving> savingVec;
	vector<vector<float>> distanceMatrix = GetDistanceMatrix(dataset);
	for (int i = 1; i < dataset.size(); i++)
	{
		for (int j = i + 1; j < dataset.size(); j++)
		{
			float savingtemp = distanceMatrix[i][0] + distanceMatrix[j][0] - distanceMatrix[i][j];
			Saving s(i, j, savingtemp);
			savingVec.push_back(s);
		}
	}
	SavingSort(savingVec);
	//for (int i = 0; i < savingVec.size(); i++) //输出节约值
	//{
	//	cout << savingVec[i].A_index << " - " << savingVec[i].B_index << "的节约值： " << savingVec[i].SavingScore << endl;
	//}

	//初始化初始路径
	vector<Routing> routeVec;
	for (int i = 1; i < dataset.size(); i++)
	{
		Routing r;
		r.mileage = distanceMatrix[i][0] * 2;
		r.routingLoad = dataset[i].N_demand;
		r.route.push_back(i);
		routeVec.push_back(r);
	}
	//OutputResult(routeVec);
	for (int i = 0; i < savingVec.size(); i++)
	{
		int pointA = savingVec[i].A_index;
		int pointB = savingVec[i].B_index;
		int route1 = 0;
		int route2 = 1;
		for (int j = 0; j < routeVec.size(); j++)
		{
			for (int k = 0; k < routeVec[j].route.size(); k++)
			{
				if (routeVec[j].route[k] == pointA)	route1 = j;
				if (routeVec[j].route[k] == pointB) route2 = j;
			}
		}
		if (route1 == route2) continue;//在同一条路线中找到两个客户，跳过
		int tempRoute; //确保route2 > route1
		if (route1 > route2) 
		{
			tempRoute = route1;
			route1 = route2;
			route2 = tempRoute;
		}
		float routeLoad = routeVec[route1].routingLoad + routeVec[route2].routingLoad;
	/*	for (int j = 0; j < routeVec[route1].route.size(); j++)
		{
			routeLoad += routeVec[route1].routingLoad;
		}
		for (int j = 0; j < routeVec[route2].route.size(); j++)
		{
			routeLoad += routeVec[route2].routingLoad;
		}*/
		if (routeLoad > VIHICLE_CAPACITY) continue; //如果欲合并的两条线路的载重之和超过单量车的额定载重，跳过

		vector<int> greaterRoute;
		vector<int> smallerRoute;
		if (routeVec[route1].route.size() >= routeVec[route2].route.size())
		{
			greaterRoute = { routeVec[route1].route.begin(),routeVec[route1].route.end() };
			smallerRoute = { routeVec[route2].route.begin() + 1,routeVec[route2].route.end() };
		}
		else
		{
			smallerRoute = { routeVec[route1].route.begin() + 1,routeVec[route1].route.end() };
			greaterRoute = { routeVec[route2].route.begin(),routeVec[route2].route.end() };
		}
		greaterRoute.insert(greaterRoute.end(), smallerRoute.begin(), smallerRoute.end()); //将小路径插到大路径之后

		float origiLen = routeVec[route1].mileage + routeVec[route2].mileage;
		float newLen = 0;
		for (int j = 0; j < greaterRoute.size() - 1; j++)
		{
			newLen += distanceMatrix[greaterRoute[j]][greaterRoute[j + 1]];
		}
		newLen += distanceMatrix[greaterRoute[greaterRoute.size() - 1]][0];
		if (origiLen < newLen) continue; //如果合并后的路径长度没有缩短，跳过

		routeVec.erase(routeVec.begin() + route2);
		routeVec.erase(routeVec.begin() + route1); //这里注意必须先删除下标数字更大的路线
		Routing r;
		r.mileage = newLen;
		r.routingLoad = routeLoad;
		r.route = greaterRoute;
		routeVec.push_back(r);
	}
	OutputResult(routeVec);
}

//void test()
//{
//	vector<int> v;
//	v.push_back(0);
//	for (int i = 0; i < v.size(); i++)
//	{
//		cout << v[i] << endl;
//	}
//	vector<int> v2;
//	v2.push_back(1);
//	v2.push_back(2);
//	v = v2;
//	cout << "_____________" << endl;
//	for (int i = 0; i < v.size(); i++)
//	{
//		cout << v[i] << endl;
//	}
//}

int main()
{
	vector<Node> data = GetFile("database3.txt");
	Saving_Hueristics(data);
	/*test();*/
	system("pause");
	return 0;
}