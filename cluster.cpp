#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<Windows.h>
using namespace std;
char artistName[25][30];//����������
double datas[25][25];//���ƶ�����
vector<vector<int>>cluster;//�����
vector<vector<vector<int>>>clusterRecords;//������̵ļ�¼(����)
vector<vector<vector<int>>>clusterRecords2;//������̵ļ�¼
vector<vector<int>>records;//ÿ�κϲ��ļ�¼(����)
vector<vector<int>>records2;//ÿ�κϲ��ļ�¼
int shift = 0;//�ӿ���̨��shift�л�����
double Dunn[3];//�˶�ָ��

//�������ù��λ��
void SetPosition(int x, int y)
{
	HANDLE winHandle;//���
	COORD pos = { (short)x,(short)y };
	winHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	//���ù��λ�� 
	SetConsoleCursorPosition(winHandle, pos);
}

//���ļ��ж�ȡ�������
void ReadData() {
	//�������ļ�
	ifstream name("artists25.in");
	if (!name) {
		cout << "���ļ�ʧ�ܣ�" << endl;
		exit(1);
	}
	ifstream dataAm("dataAm25.in");
	if (!dataAm) {
		cout << "���ļ�ʧ�ܣ�" << endl;
		exit(1);
	}

	//��������������
	for (int i = 0; i < 25; i++) {
		while (name.get() != ',');
		name.getline(artistName[i], 30);
	}

	//�������ƶ�����
	for (int i = 0; i < 25; i++) {
		for (int j = 0; j < 25; j++) {
			dataAm >> datas[i][j];
		}
	}
	name.close();
	dataAm.close();
}

//��ʼ�������Ϊ25��
void InitCluster() {
	cluster.clear();
	for (int i = 0; i < 25; i++) {
		vector<int> temp;
		temp.push_back(i);
		cluster.push_back(temp);
	}
}

//���㵥���Ӿ���(��С���ƶ�)
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
//������ȫ���Ӿ���(������ƶ�)
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
//����ƽ�����Ӿ���
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

//��������еĵ�i�����Ϊ���࣬��һ����len��Ԫ��
void SplitCluster(int i, int len) {
	vector<int> temp;
	for (int k = len; k < (int)cluster[i].size(); k++) {
		temp.push_back(cluster[i][k]);
	}
	cluster[i].erase(cluster[i].begin() + len,cluster[i].end());
	cluster.insert(cluster.begin() + i + 1, temp);
}

//���վ���ϲ���˳���������
//�����ఴ�պϲ���˳������ָ�Եõ����ƾ����������������˳��
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

//���ƾ�����
void DrawTree() {
	//��ر�����ʼ��
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
	//���պϲ���¼records2�ٴν��кϲ��Ĺ��̣��ڹ����м�¼�������Ĺؼ���λ��
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
		//�ϲ�
		for (unsigned int i = 0; i < cluster[jMerge].size(); i++) {
			cluster[iMerge].push_back(cluster[jMerge][i]);
		}
	}
	layer[mid].push_back(25);
	//�������
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
	cout << "���õ����ӵľ�������\n" << endl;
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
		//�ϲ�
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
	//����˶�ָ��
	vector<vector<int>> cluster4 = clusterRecords[3];
	//�������������,�����������ƶ�
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
	//����������ھ���,����С�������ƶȣ�ȥ��0�������
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
	cout << "������ȫ���ӵľ�������\n" << endl;
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
		//�ϲ�
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
	//����˶�ָ��
	cluster4 = clusterRecords[3];
	//�������������,�����������ƶ�
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
	//����������ھ���,����С�������ƶȣ�ȥ��0�������
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
	cout << "����ƽ�����ӵľ�������\n" << endl;
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
		//�ϲ�
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
	//����˶�ָ��
	cluster4 = clusterRecords[3];
	//�������������,�����������ƶ�
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
	//����������ھ���,����С�������ƶȣ�ȥ��0�������
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
	cout << "�����ӵ˶�ָ��Ϊ��" << Dunn[0] << endl;
	cout << "��ȫ���ӵ˶�ָ��Ϊ��" << Dunn[1] << endl;
	cout << "ƽ�����ӵ˶�ָ��Ϊ��" << Dunn[2] << endl;
	return 0;
}