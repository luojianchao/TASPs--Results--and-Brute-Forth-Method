#include "iostream"//��ͨ���������
#include "fstream"//�ļ��������
#include "algorithm"
#include <time.h>
#include <list>
#include <vector>
#include <time.h>
using namespace::std;
#define INF 100000000

struct ScmOpenClosed//δ�������������Ľṹ��
{
	int nNodeNuM;
};
struct ScmNode//���Ľṹ��
{
	int nNodeNuM;
	int nLineNum;//�����к�
	vector<int> dMachFinishTime;//��¼ÿ��������ǰ�����ʱ��
	bool isBranched;
	int nParent;//���ڵ�ı��
	int nJob;
};

vector<vector<int>> vProcessingTime;
vector<vector<int>> vSetupTime;
vector<vector<int>> vNodeNum;
int nMachN;
int nJob;
list<ScmOpenClosed> open, closed;
vector<ScmNode> aScmNode;



void fnDel2(int l, int m, int ***arr)
{
	for (int i = 0; i < l; i++)
	{
		delete[](*arr)[i];// = new int[m];
	}
	delete[](*arr);
}
template <class T> T fnMax(T * nArr, T nL)
{
	T tmax = -1;
	for (int i = 0; i < nL; i++)
	{
		if (nArr[i]>tmax)
		{
			tmax = nArr[i];
		}
	}
	return tmax;
}

void fnNew2(int l, int m, int ***arr)
{
	*arr = new int*[l];
	for (int i = 0; i < l; i++)
	{
		(*arr)[i] = new int[m];
	}
}

void fnComputeTime(int *pmatrix, int &finaltime)
{
	int* nMachineTime = new int[nMachN];
	for (int i = 0; i < nMachN; i++)
		nMachineTime[i] = 0;
	for (int i = 0; i < nJob; i++)
	{
		int nmax = 0;
		for (int j = 0; j < nMachN - 1; j++)
		{
			nMachineTime[j] = nMachineTime[j] + vProcessingTime[pmatrix[i] - 1][j] + vSetupTime[pmatrix[i] - 1][j];
			nmax = max(nMachineTime[j], nmax);
		}
		nMachineTime[nMachN - 1] = max(nmax, nMachineTime[nMachN - 1] + vSetupTime[pmatrix[i] - 1][nMachN - 1]) + vProcessingTime[pmatrix[i] - 1][nMachN - 1];
	}
	finaltime = nMachineTime[nMachN - 1];
	delete[] nMachineTime;
}

void ChangeNode(ScmNode node, int job, ScmNode& node1)
{
	//node �� number û�иı�,����aScmNodeʱ��Ҫע�����
	node1 = node;
	node1.nLineNum = node.nLineNum + 1;
	node1.nParent = node.nNodeNuM;
	int nmax = 0;
	for (int i = 0; i < nMachN - 1; i++)
	{
		node1.dMachFinishTime[i] += vProcessingTime[job][i] + vSetupTime[job][i];
		nmax = max(node1.dMachFinishTime[i], nmax);
	}
	node1.dMachFinishTime[nMachN - 1] = max(nmax, node1.dMachFinishTime[nMachN - 1] + vSetupTime[job][nMachN - 1]) + vProcessingTime[job][nMachN - 1];
	node1.nJob = job;
	node1.isBranched = false;
}

void Initial()
{
	ScmNode ininod;
	list<ScmOpenClosed> clist;
	open.swap(clist);
	list<ScmOpenClosed> clist1;
	closed.swap(clist1);//��ɾ�����е�Ԫ��
	vector<ScmNode> cnod;
	aScmNode.swap(cnod);
	aScmNode.reserve(3700000);
	vector<vector<int>> cv;
	vNodeNum.swap(cv);//�洢ÿһ�еĽ����
	ininod.nNodeNuM = 0;
	ininod.nLineNum = 0;
	ininod.isBranched = false;
	ininod.nParent = -1;//�޸��׽��
	vector<int> vtemp;
	vtemp.clear();
	for (int i = 0; i <= nJob; i++)
	{
		vector<int> vec;
		vec.clear();
		vNodeNum.push_back(vec);
	}
	for (int i = 0; i < nMachN; i++)
	{
		ininod.dMachFinishTime.push_back(0);
	}
	aScmNode.push_back(ininod);
	ScmOpenClosed firstnode;
	firstnode.nNodeNuM = 0;
	open.push_back(firstnode);
	vNodeNum[ininod.nLineNum].push_back(ininod.nNodeNuM);
}

void Retrieve(int num, int* jobSequence)
{
	vector<int> o;
	for (int i = 0; i < nJob; i++)
	{
		jobSequence[nJob - i - 1] = aScmNode[num].nJob;
		num = aScmNode[num].nParent;
	}
}

void ComputeRemainpart(ScmNode node, vector<int>& remain)
{
	vector<int> fir;
	fir.push_back(node.nJob);
	int np = node.nParent;
	for (int i = 0; i < node.nLineNum - 1; i++)
	{
		fir.push_back(aScmNode[np].nJob);
		np = aScmNode[np].nParent;
	}
	bool flag;
	for (int i = 0; i < nJob; i++)
	{
		flag = true;
		for (int j = 0; j < fir.size(); j++)
		{
			if (i == fir[j])
			{
				flag = false;
				break;
			}
		}
		if (flag == true)
		{
			remain.push_back(i);
		}
	}
}

void BruteForthMethod(int& best, int* jobSequence)
{
	Initial();
	best = INF;
	while (open.size() != 0)
	{
		ScmOpenClosed ScmArry = open.front();
		open.pop_front();
		if (aScmNode[ScmArry.nNodeNuM].nLineNum == nJob && aScmNode[ScmArry.nNodeNuM].dMachFinishTime[nMachN - 1] < best)
		{
			best = aScmNode[ScmArry.nNodeNuM].dMachFinishTime[nMachN - 1];
			Retrieve(ScmArry.nNodeNuM, jobSequence);
			closed.push_back(ScmArry);
			aScmNode[ScmArry.nNodeNuM].isBranched = true;//�����ֽ����Ϊ�������Ľ�㣬���ٽ�������
			continue;
		}
		if (aScmNode[ScmArry.nNodeNuM].nLineNum == nJob)
			continue;
		closed.push_back(ScmArry);
		vector<int> R1;
		ComputeRemainpart(aScmNode[ScmArry.nNodeNuM], R1);
		aScmNode[ScmArry.nNodeNuM].isBranched = true;
		ScmNode node1;
		while (R1.size() > 0)
		{
			int job = R1[R1.size() - 1];
			R1.pop_back();
			ChangeNode(aScmNode[ScmArry.nNodeNuM], job, node1);
			node1.nNodeNuM = aScmNode.size();
			node1.isBranched = false;
			aScmNode.push_back(node1);//�²����Ľ������������
			vNodeNum[node1.nLineNum].push_back(node1.nNodeNuM);//�������
			ScmOpenClosed scm;
			scm.nNodeNuM = node1.nNodeNuM;
			open.push_back(scm);//���²����Ľ�����open��
		}
	}
}

int main()
{
	srand((unsigned)(time(NULL)));
	int jobs[] = { 10 };
	int machine[] = { 4, 6, 10 };
	ofstream file2("Result.csv");
	ofstream file3("Sequence.csv");
	ifstream file1("TAFSPs.csv");
	for (int i = 0; i < 1; i++)
	{
		for (int j = 0; j < 3; j++)//3
		{
			nJob = jobs[i];
			nMachN = machine[j];
			for (int k = 0; k < 20; k++)//20
			{
				vector<vector<int>> vv;
				vProcessingTime.swap(vv);
				vector<vector<int>> vv1;
				vSetupTime.swap(vv1);
				for (int t = 0; t < jobs[i] + 1; t++)
				{
					if (t == 0)
					{
						char bb = 'c';
						char c;
						int m;
						while (bb != ',')
						{
							file1 >> bb;
						}
						file1 >> m >> c;
						file1 >> m >> c;
					}
					else
					{
						int bb;
						char c;
						file1 >> bb >> c;
						vector<int> vP;
						for (int f = 0; f < machine[j]; f++)
						{
							int bb;
							char c;
							file1 >> bb >> c;
							vP.push_back(bb);
						}
						vProcessingTime.push_back(vP);
						char cc;
						file1 >> cc;
						vector<int> vS;
						for (int f = 0; f < machine[j]; f++)
						{
							int bb;
							char c;
							file1 >> bb >> c;
							vS.push_back(bb);
						}
						vSetupTime.push_back(vS);
					}
				}
				int best = INF;
				int* jobSequence = new int[nJob];
				double st, end;//��ʼ����ʱ��ͽ�������ʱ��
				st = clock();
				BruteForthMethod(best, jobSequence);
				end = clock();
				cout << best << endl;
				double runtime = (end - st) / (1.0*CLOCKS_PER_SEC);
				file2 << best << "," << runtime << endl;
				for (int kk = 0; kk < nJob - 1; kk++)
					file3 << jobSequence[kk] << ",";
				file3 << jobSequence[nJob - 1] << endl;
				delete[] jobSequence;
			}
		}
	}
	file1.close();
	return 0;
}
