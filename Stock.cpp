#include <string>
#include <vector>
#include <map>
#include "Stock.h"
#include <iostream>

using namespace std;

namespace fre {

	vector<double> Stock::calAR(const Stock &benchmark, int N) const {
		// calculate abnormal return
		vector<string> dates = benchmark.getPriceDate();
		vector<double> prices = benchmark.getPriceVal();

		int idx = find(dates.begin(), dates.end(), starting_date) - dates.begin();
		//vector<string> new_dates = vector<string>(dates.begin() + idx, dates.begin() + idx + 2 * N + 1);
		vector<double> new_prices = vector<double>(prices.begin() + idx, prices.begin() + idx + 2 * N + 1);

		vector<double> stock_daily_return(2 * N), benchmark_daily_return(2 * N), abnormal_return(2 * N);

		for (int i = 1; i < 2 * N + 1; i++) {
			stock_daily_return[i - 1] = (price_val[i] - price_val[i - 1]) / price_val[i - 1];
			benchmark_daily_return[i - 1] = (new_prices[i] - new_prices[i - 1]) / new_prices[i - 1];
			abnormal_return[i - 1] = stock_daily_return[i - 1] - benchmark_daily_return[i - 1];
		}
		return abnormal_return;


	}

	vector<double> Stock::calCR(const Stock &benchmark, int N) const {
		// calculate cumulated return
		vector<double> abnormal_return = calAR(benchmark, N);
		vector<double> cumulative_return(2 * N);
		cumulative_return[0] = abnormal_return[0];
		for (int i = 1; i < 2 * N; i++) {
			cumulative_return[i] = cumulative_return[i - 1] + abnormal_return[i];
		}
		return cumulative_return;
	}
}