#include<iostream>
#include <cmath>
#include<fstream>
#include<vector>
#include<sstream>
#include <algorithm>
using namespace std;

#define COMMA_NUM 3 //database��ÿ����Ϣ�Ķ�����
#define VIHICLE_CAPACITY 9 //�����������

class Node
{
public:
	float N_x;
	float N_y;
	float N_demand; //����
	int N_index; //���
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
	int A_index; //A��ı����
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
	float mileage; //·�����
	float routingLoad; //����
	vector<int> route; //·��
	Routing()
	{
		routingLoad = 0;
		mileage = 0;
		route.push_back(0);
	}
};

int StringToInt(string str)//���ڶ��ļ�ʱ����string��������ת����int����
{
	stringstream sf;
	int f = 0;
	sf << str;
	sf >> f;
	return f;
}

float StringToFloat(string str)//���ڶ��ļ�ʱ����string��������ת����float����
{
	stringstream sf;
	float f = 0;
	sf << str;
	sf >> f;
	return f;
}

int pos(string T, string P, int n) //����database�С�������n�γ��ֵ�λ��
{
	//if (n == 0)return -1;//�൱��û��
	int count = 0;
	unsigned begined = 0;
	while ((begined = T.find(P, begined)) != string::npos) {
		count++;
		begined += P.length();
		if (n == count) {
			return begined - 1;
		}
	}
	//return -1;//��ζ��û����ô���
}

vector<Node> GetFile(const char* database) //��ȡtxt�ļ��еĵ����Ϣ������vector<Node> dataset
{
	vector<Node> dataset;
	ifstream ifs;
	ifs.open(database, ios::in);
	if (!ifs.is_open())
	{
		cout << "�ļ���ʧ�ܣ�" << endl;
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

vector<vector<float>> GetDistanceMatrix(vector<Node> database) //�õ��������
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

void SavingSort(vector<Saving>& v) //����ѡ�����򣬰�Saving.SavingScore�Ӵ�С����
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

void OutputResult(vector<Routing>& Finalresult) //������
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
			ofs << "��" << count << "��·�ߣ� ";
			for (int j = 0; j < Finalresult[i].route.size(); j++)
			{
				ofs << Finalresult[i].route[j] << " �� ";
			}
			ofs << "0" << "\t" << "·���г̣� " << Finalresult[i].mileage << "\t" << "�������أ� " << Finalresult[i].routingLoad << endl;
			count++;
		}
	}
	ofs << "��·�̣� " << totalDistance << endl;
	ofs.close();
}

void Saving_Hueristics(vector<Node> dataset)
{
	// ��ôӴ�С����õĽ�Լֵ����
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
	//for (int i = 0; i < savingVec.size(); i++) //�����Լֵ
	//{
	//	cout << savingVec[i].A_index << " - " << savingVec[i].B_index << "�Ľ�Լֵ�� " << savingVec[i].SavingScore << endl;
	//}

	//��ʼ����ʼ·��
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
		if (route1 == route2) continue;//��ͬһ��·�����ҵ������ͻ�������
		int tempRoute; //ȷ��route2 > route1
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
		if (routeLoad > VIHICLE_CAPACITY) continue; //������ϲ���������·������֮�ͳ����������Ķ���أ�����

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
		greaterRoute.insert(greaterRoute.end(), smallerRoute.begin(), smallerRoute.end()); //��С·���嵽��·��֮��

		float origiLen = routeVec[route1].mileage + routeVec[route2].mileage;
		float newLen = 0;
		for (int j = 0; j < greaterRoute.size() - 1; j++)
		{
			newLen += distanceMatrix[greaterRoute[j]][greaterRoute[j + 1]];
		}
		newLen += distanceMatrix[greaterRoute[greaterRoute.size() - 1]][0];
		if (origiLen < newLen) continue; //����ϲ����·������û�����̣�����

		routeVec.erase(routeVec.begin() + route2);
		routeVec.erase(routeVec.begin() + route1); //����ע�������ɾ���±����ָ����·��
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