#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<Windows.h>
using namespace std;
char artistName[25][30];//艺术家名字
double datas[25][25];//相似度数据
vector<vector<int>>cluster;//聚类簇
vector<vector<vector<int>>>clusterRecords;//聚类过程的记录(反向)
vector<vector<vector<int>>>clusterRecords2;//聚类过程的记录
vector<vector<int>>records;//每次合并的记录(反向)
vector<vector<int>>records2;//每次合并的记录
int shift = 0;//从控制台第shift行绘制树
double Dunn[3];//邓恩指标

//用于设置光标位置
void SetPosition(int x, int y)
{
	HANDLE winHandle;//句柄
	COORD pos = { (short)x,(short)y };
	winHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	//设置光标位置 
	SetConsoleCursorPosition(winHandle, pos);
}

//从文件中读取相关数据
void ReadData() {
	//打开输入文件
	ifstream name("artists25.in");
	if (!name) {
		cout << "打开文件失败！" << endl;
		exit(1);
	}
	ifstream dataAm("dataAm25.in");
	if (!dataAm) {
		cout << "打开文件失败！" << endl;
		exit(1);
	}

	//读入艺术家名字
	for (int i = 0; i < 25; i++) {
		while (name.get() != ',');
		name.getline(artistName[i], 30);
	}

	//读入相似度数据
	for (int i = 0; i < 25; i++) {
		for (int j = 0; j < 25; j++) {
			dataAm >> datas[i][j];
		}
	}
	name.close();
	dataAm.close();
}

//初始化聚类簇为25类
void InitCluster() {
	cluster.clear();
	for (int i = 0; i < 25; i++) {
		vector<int> temp;
		temp.push_back(i);
		cluster.push_back(temp);
	}
}

//计算单链接距离(最小相似度)
double SingleLinkage(int i, int j) {
	double min = 99999;
	for (unsigned int x = 0; x < cluster[i].size(); x++) {
		for (unsigned int y = 0; y < cluster[j].size(); y++) {
			if (datas[cluster[i][x]][cluster[j][y]] < min) {
				min = datas[cluster[i][x]][cluster[j][y]];
			}
		}
	}
	return min;
}
//计算完全链接距离(最大相似度)
double CompleteLinkage(int i, int j) {
	double max = -1;
	for (unsigned int x = 0; x < cluster[i].size(); x++) {
		for (unsigned int y = 0; y < cluster[j].size(); y++) {
			if (datas[cluster[i][x]][cluster[j][y]] > max) {
				max = datas[cluster[i][x]][cluster[j][y]];
			}
		}
	}
	return max;
}
//计算平均链接距离
double AverageLinkage(int i, int j) {
	double avg = 0;
	for (unsigned int x = 0; x < cluster[i].size(); x++) {
		for (unsigned int y = 0; y < cluster[j].size(); y++) {
			avg += datas[cluster[i][x]][cluster[j][y]];
		}
	}
	avg /= (double)cluster[i].size() * (double)cluster[j].size();
	return avg;
}

//将聚类簇中的第i个类分为两类，第一类有len个元素
void SplitCluster(int i, int len) {
	vector<int> temp;
	for (int k = len; k < (int)cluster[i].size(); k++) {
		temp.push_back(cluster[i][k]);
	}
	cluster[i].erase(cluster[i].begin() + len,cluster[i].end());
	cluster.insert(cluster.begin() + i + 1, temp);
}

//按照聚类合并的顺序逆向回溯
//将聚类按照合并的顺序逆向分割，以得到绘制聚类树的艺术家输出顺序
void TraceBack() {
	int n = records.size();
	for (int i = 0; i < n; i++) {
		int k = 0;
		for (k = 0; k < (int)cluster.size(); k++) {
			if (cluster[k] == clusterRecords[i][records[i][0]]) {
				break;
			}
		}
		SplitCluster(k, records[i][2]);
	}
}

//绘制聚类树
void DrawTree() {
	//相关变量初始化
	int n = 1;
	vector<vector<int>> row;
	for (int i = 0; i < 25; i++) {
		vector<int> temp;
		temp.push_back(i * 2);
		row.push_back(temp);
	}
	vector<vector<int>> layer;
	for (int i = 0; i < 50; i++) {
		vector<int> temp;
		temp.push_back(-1);
		layer.push_back(temp);
		if (i % 2 == 0) {
			layer[i].push_back(0);
		}
	}
	int layer2[25][2];
	int seq[25];
	for (int i = 0; i < 25; i++) {
		seq[i] = cluster[i][0];
	}
	int size = records2.size();
	int iMerge = 0, jMerge = 0;
	int mid = 0;
	//按照合并记录records2再次进行合并的过程，在过程中记录绘制树的关键点位置
	for (int i = 0; i < size; i++) {
		int k = 0;
		for (k = 0; k < (int)cluster.size(); k++) {
			if (cluster[k] == clusterRecords2[i][records2[i][0]]) {
				break;
			}
		}
		iMerge = k;
		for (k = 0; k < (int)cluster.size(); k++) {
			if (cluster[k] == clusterRecords2[i][records2[i][1]]) {
				break;
			}
		}
		jMerge = k;
		layer[row[iMerge][row[iMerge].size() - 1]].push_back(n);
		layer[row[jMerge][row[jMerge].size() - 1]].push_back(n);
		layer2[n][0] = row[iMerge][row[iMerge].size() - 1];
		layer2[n][1] = row[jMerge][row[jMerge].size() - 1];
		mid = (row[iMerge][row[iMerge].size() - 1] + row[jMerge][row[jMerge].size() - 1]) / 2;
		layer[mid].push_back(n);
		row[iMerge].push_back(mid);
		n++;
		//合并
		for (unsigned int i = 0; i < cluster[jMerge].size(); i++) {
			cluster[iMerge].push_back(cluster[jMerge][i]);
		}
	}
	layer[mid].push_back(25);
	//输出树形
	for (int i = 0; i < 25; i++) {
		cout << artistName[seq[i]];
		for (int j = 0; j < 25 - (int)strlen(artistName[seq[i]]); j++) {
			cout << "-";
		}
		for (int j = layer[2 * i][1]; j < layer[2 * i][2]; j++) {
			cout << "-----";
		}
		for (int j = 3; j < (int)layer[2 * i].size(); j+=2) {
			for (int k = layer[2 * i][j - 1]; k < layer[2 * i][j]; k++) {
				cout << "     ";
			}
			for (int k = layer[2 * i][j]; k < layer[2 * i][j + 1]; k++) {
				cout << "-----";
			}		
		}
		cout << endl;
		cout << "                         ";
		if (layer[2 * i + 1].size() <= 1) {
			cout << endl;
			continue;
		}
		for (int j = 0; j < layer[2 * i + 1][1]; j++) {
			cout << "     ";
		}
		for (int j = layer[2 * i+1][1]; j < layer[2 * i+1][2]; j++) {
			cout << "-----";
		}
		for (int j = 3; j < (int)layer[2 * i+1].size(); j += 2) {
			for (int k = layer[2 * i+1][j - 1]; k < layer[2 * i+1][j]; k++) {
				cout << "     ";
			}
			for (int k = layer[2 * i+1][j]; k < layer[2 * i+1][j + 1]; k++) {
				cout << "-----";
			}
		}
		cout << endl;
	}
	for (int n = 1; n < 25; n++) {
		int min = layer2[n][0], max = layer2[n][1];
		if (min > max) {
			int temp = min;
			min = max;
			max = min;
		}
		for (int p = min; p <= max; p++) {
			SetPosition(25 + n * 5, p + shift);
			cout << "|";
			if (p == min) {
				cout << n;
			}
		}
	}

}

int main() {
	ReadData();
	cout << "采用单链接的聚类树：\n" << endl;
	shift = 2;
	InitCluster();
	while (cluster.size() > 1) {
		clusterRecords2.push_back(cluster);
		double max = -1;
		int iMerge = 0, jMerge = 0;
		for (unsigned int i = 0; i < cluster.size(); i++) {
			for (unsigned int j = i + 1; j < cluster.size(); j++) {
				double temp = SingleLinkage(i, j);
				if (temp > max) {
					max = temp;
					iMerge = i;
					jMerge = j;
				}
			}
		}
		//合并
		vector<int>record = { iMerge,jMerge,(int)cluster[iMerge].size() };
		records.insert(records.begin(), record);
		records2.push_back(record);
		for (unsigned int i = 0; i < cluster[jMerge].size(); i++) {
			cluster[iMerge].push_back(cluster[jMerge][i]);
		}
		cluster.erase(cluster.begin() + jMerge);
		clusterRecords.insert(clusterRecords.begin(), cluster);
	}
	TraceBack();
	DrawTree();
	//计算邓恩指标
	vector<vector<int>> cluster4 = clusterRecords[3];
	//计算最短类间距离,即最大类间相似度
	double min = 0;
	for (int i = 0; i < 4; i++) {
		for (int j = i + 1; j < 4; j++) {
			for (unsigned int x = 0; x < cluster4[i].size(); x++) {
				for (unsigned int y = 0; y < cluster4[j].size(); y++) {
					if (min < datas[cluster4[i][x]][cluster4[j][y]]) {
						min = datas[cluster4[i][x]][cluster4[j][y]];
					}
				}
			}
		}
	}
	//计算最大类内距离,即最小类内相似度（去除0的情况）
	double max = 99999;
	for (int i = 0; i < 4; i++) {
		for (unsigned int x = 0; x < cluster4[i].size(); x++) {
			for (unsigned int y = x + 1; y < cluster4[i].size(); y++) {
				if (datas[cluster4[i][x]][cluster4[i][y]] == 0)continue;
				if (max > datas[cluster4[i][x]][cluster4[i][y]]) {
					max = datas[cluster4[i][x]][cluster4[i][y]];
				}
			}
		}
	}
	Dunn[0] = min / max;

	SetPosition(0, 53);
	cout << "采用完全链接的聚类树：\n" << endl;
	shift = 55;
	InitCluster();
	records.clear();
	records2.clear();
	clusterRecords.clear();
	clusterRecords2.clear();
	while (cluster.size() > 1) {
		clusterRecords2.push_back(cluster);
		double max = -1;
		int iMerge = 0, jMerge = 0;
		for (unsigned int i = 0; i < cluster.size(); i++) {
			for (unsigned int j = i + 1; j < cluster.size(); j++) {
				double temp = CompleteLinkage(i, j);
				if (temp > max) {
					max = temp;
					iMerge = i;
					jMerge = j;
				}
			}
		}
		//合并
		vector<int>record = { iMerge,jMerge,(int)cluster[iMerge].size() };
		records.insert(records.begin(), record);
		records2.push_back(record);
		for (unsigned int i = 0; i < cluster[jMerge].size(); i++) {
			cluster[iMerge].push_back(cluster[jMerge][i]);
		}
		cluster.erase(cluster.begin() + jMerge);
		clusterRecords.insert(clusterRecords.begin(), cluster);
	}
	TraceBack();
	DrawTree();
	//计算邓恩指标
	cluster4 = clusterRecords[3];
	//计算最短类间距离,即最大类间相似度
	min = 0;
	for (int i = 0; i < 4; i++) {
		for (int j = i + 1; j < 4; j++) {
			for (unsigned int x = 0; x < cluster4[i].size(); x++) {
				for (unsigned int y = 0; y < cluster4[j].size(); y++) {
					if (min < datas[cluster4[i][x]][cluster4[j][y]]) {
						min = datas[cluster4[i][x]][cluster4[j][y]];
					}
				}
			}
		}
	}
	//计算最大类内距离,即最小类内相似度（去除0的情况）
	max = 99999;
	for (int i = 0; i < 4; i++) {
		for (unsigned int x = 0; x < cluster4[i].size(); x++) {
			for (unsigned int y = x + 1; y < cluster4[i].size(); y++) {
				if (datas[cluster4[i][x]][cluster4[i][y]] == 0)continue;
				if (max > datas[cluster4[i][x]][cluster4[i][y]]) {
					max = datas[cluster4[i][x]][cluster4[i][y]];
				}
			}
		}
	}
	Dunn[1] = min / max;

	SetPosition(0, 108);
	cout << "采用平均链接的聚类树：\n" << endl;
	shift = 110;
	InitCluster();
	records.clear();
	records2.clear();
	clusterRecords.clear();
	clusterRecords2.clear();
	while (cluster.size() > 1) {
		clusterRecords2.push_back(cluster);
		double max = -1;
		int iMerge = 0, jMerge = 0;
		for (unsigned int i = 0; i < cluster.size(); i++) {
			for (unsigned int j = i + 1; j < cluster.size(); j++) {
				double temp = AverageLinkage(i, j);
				if (temp > max) {
					max = temp;
					iMerge = i;
					jMerge = j;
				}
			}
		}
		//合并
		vector<int>record = { iMerge,jMerge,(int)cluster[iMerge].size() };
		records.insert(records.begin(), record);
		records2.push_back(record);
		for (unsigned int i = 0; i < cluster[jMerge].size(); i++) {
			cluster[iMerge].push_back(cluster[jMerge][i]);
		}
		cluster.erase(cluster.begin() + jMerge);
		clusterRecords.insert(clusterRecords.begin(), cluster);
	}
	TraceBack();
	DrawTree();
	//计算邓恩指标
	cluster4 = clusterRecords[3];
	//计算最短类间距离,即最大类间相似度
	min = 0;
	for (int i = 0; i < 4; i++) {
		for (int j = i + 1; j < 4; j++) {
			for (unsigned int x = 0; x < cluster4[i].size(); x++) {
				for (unsigned int y = 0; y < cluster4[j].size(); y++) {
					if (min < datas[cluster4[i][x]][cluster4[j][y]]) {
						min = datas[cluster4[i][x]][cluster4[j][y]];
					}
				}
			}
		}
	}
	//计算最大类内距离,即最小类内相似度（去除0的情况）
	max = 99999;
	for (int i = 0; i < 4; i++) {
		for (unsigned int x = 0; x < cluster4[i].size(); x++) {
			for (unsigned int y = x + 1; y < cluster4[i].size(); y++) {
				if (datas[cluster4[i][x]][cluster4[i][y]] == 0)continue;
				if (max > datas[cluster4[i][x]][cluster4[i][y]]) {
					max = datas[cluster4[i][x]][cluster4[i][y]];
				}
			}
		}
	}
	Dunn[2] = min / max;
	SetPosition(0, 162);
	cout << "单链接邓恩指标为：" << Dunn[0] << endl;
	cout << "完全链接邓恩指标为：" << Dunn[1] << endl;
	cout << "平均链接邓恩指标为：" << Dunn[2] << endl;
	return 0;
}