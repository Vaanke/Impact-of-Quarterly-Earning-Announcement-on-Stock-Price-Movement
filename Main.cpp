#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <thread>
#include "curl/curl.h"
#include "Stock.h"
#include "Group.h"
#include "DataReader.h"
#include "Display.h"

using namespace std;
using namespace fre;

const double thr1 = 6;
const double thr2 = 24;
const int selecNum = 80;
const int simuNum = 40;

void time_log()
{
	time_t now = time(0);
	tm* tm_ = localtime(&now);
	char buffer[80];
	strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", tm_);
	cout << buffer << "\t";
}

void init(Group& BeatGroup, Group& MeetGroup, Group& MissGroup, Stock& benchmark, int& N) {
	int N_ = 0;
	cout << "input N:" << endl;
	while (N_ < 60) {
		cin >> N_;
		if (N_ < 60) cerr << "N should be no less than 60" << endl;
	}
	N = N_; 

	time_t t_0 = time(0);
	time_log(); cout << "initializing..." << endl;

	BeatGroup.updateN(N);
	MeetGroup.updateN(N);
	MissGroup.updateN(N);

	time_log(); cout << "reading earnings..." << endl;
	readBasic(BeatGroup.getStocks(), MeetGroup.getStocks(), MissGroup.getStocks(), thr1, thr2);
	cout << "Beat Group: " << BeatGroup.getStocks()->size() << endl <<
		"Meet Group: " << MeetGroup.getStocks()->size() << endl <<
		"Miss Group: " << MissGroup.getStocks()->size() << endl;

	time_log(); cout << "downloading price ..." << endl;
	vector<string> date_range = readDate(N);
	readPrice(benchmark, date_range[0], date_range[1]);

	curl_global_init(CURL_GLOBAL_ALL);
	thread t1(readPrice_group, BeatGroup.getStocks(), N, &benchmark);
	thread t2(readPrice_group, MeetGroup.getStocks(), N, &benchmark);
	thread t3(readPrice_group, MissGroup.getStocks(), N, &benchmark);
	t1.join();
	t2.join();
	t3.join();
	time_log(); cout << "price ok" << endl;
	cout << "Beat Group: " << BeatGroup.getStocks()->size() << endl <<
		"Meet Group: " << MeetGroup.getStocks()->size() << endl <<
		"Miss Group: " << MissGroup.getStocks()->size() << endl;

	time_log(); cout << "calculate by bootstrap..." << endl;
	BeatGroup.calByBootstrap(benchmark, selecNum, simuNum);
	MeetGroup.calByBootstrap(benchmark, selecNum, simuNum);
	MissGroup.calByBootstrap(benchmark, selecNum, simuNum);
	time_log(); cout << "initialization finished. total time: " << (time_t)(time(0) - t_0) << "sec" << endl;
}

int main() {
	Stock benchmark("IWB", "IWB", "", "", {});
	map<string, Stock> BeatStocks, MeetStocks, MissStocks;
	int N = 60;
	Group BeatGroup(&BeatStocks, N);
	Group MeetGroup(&MeetStocks, N);
	Group MissGroup(&MissStocks, N);

	init(BeatGroup, MeetGroup, MissGroup, benchmark, N);

	int option = 0;
	string symbol;
	map<string, Stock>::const_iterator itr;

	while (1)
	{
		cout << "\n1\t Pull stock infomation\n";
		cout << "2\t Show AAR, AAR-STD, CAAR, CAAR-STD\n";
		cout << "3\t Plot CAAR graph\n";
		cout << "4\t Select new N\n";
		cout << "5\t Exit\n";
		cin >> option;
		switch (option)
		{
		case 1:
			cout << "input symbol" << endl;
			cin >> symbol;
			if (BeatStocks.find(symbol) != BeatStocks.end()) {
				cout << "Beat Group\n";
				BeatStocks[symbol].Display(benchmark, N);
				break;
			}
			else if (MeetStocks.find(symbol) != MeetStocks.end()) {
				cout << "Meet Group\n";
				MeetStocks[symbol].Display(benchmark, N);
				break;
			}
			else if (MissStocks.find(symbol) != MissStocks.end()) {
				cout << "Miss Group\n";
				MissStocks[symbol].Display(benchmark, N);
				break;
			}
			else {
				cerr << "Not Found\n";
				break;
			}

		case 2:
			cout << "select group\n1\tbeat group\n2\tmeet group\n3\tmiss group" << endl;
			cin >> option;
			switch (option)
			{
			case 1:
				showReturnMatrix(&BeatGroup);
				break;
			case 2:
				showReturnMatrix(&MeetGroup);
				break;
			case 3:
				showReturnMatrix(&MissGroup);
				break;
			default:
				cout << "unexpected input" << endl;
			}
			break;
		case 3:
			plotCAARGraph(&BeatGroup, &MeetGroup, &MissGroup);
			break;
		case 4:
			init(BeatGroup, MeetGroup, MissGroup, benchmark, N);
			break;
		case 5:
			return 0;
		default:
			cout << "unexpected input" << endl;
		}
	}

	return 0;
}