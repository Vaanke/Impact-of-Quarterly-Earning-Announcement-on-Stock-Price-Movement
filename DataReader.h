#pragma once
#include <map>
#include "curl/curl.h"
#include "Stock.h"

using namespace std;

namespace fre {
	void readBasic(
		map<string, Stock>* BeatStocks,
		map<string, Stock>* MeetStocks,
		map<string, Stock>* MissStocks,
		double thr1, double thr2);

	void readPrice_group(map<string, Stock>* stocks, int N, const Stock* benchmark);

	void readPrice(Stock& stock, string from, string to);

	vector<string> readDate(int N);

}