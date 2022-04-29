#pragma once

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <iomanip>

using namespace std;
namespace fre {
	class Stock {
	private:
		string symbol, name;
		string date, period_end;
		vector<double> earning; // {estimate, reported, surprise, surprise%}

		string starting_date, ending_date;
		vector<string> price_date;
		vector<double> price_val;
		map<string, int> date_price;
	public:
		Stock(string symbol_, string name_, string date_, string period_end_, vector<double> earning_) :
				symbol(symbol_), name(name_), date(date_), period_end(period_end_), earning(earning_) {
			starting_date = "";
			ending_date = "";
			price_date = {};
			price_val = {};
		}

		Stock() {
			symbol = "";
			name = "";
			date = "";
			period_end = "";
			earning = {};
			starting_date = "";
			ending_date = "";
			price_date = {};
			price_val = {};
		}

		Stock(const Stock &stock) {
			symbol = stock.symbol;
			name = stock.name;
			date = stock.date;
			period_end = stock.period_end;
			earning = stock.earning;
			starting_date = stock.starting_date;
			ending_date = stock.ending_date;
			price_date = stock.price_date;
			price_val = stock.price_val;
		}

		void updatePrice(vector<string> price_date_, vector<double> price_val_) {

			price_date = price_date_;
			price_val = price_val_;
			starting_date = price_date_.front();
			ending_date = price_date_.back();
		}

		string getName() const { return name; }

		string getSymbol() const { return symbol; }

		string getAnnounceDate() const { return date; }

		string getPeriodEnd() const { return period_end; }

		vector<string> getPriceDate() const { return price_date; }

		vector<double> getPriceVal() const { return price_val; }

		string getStartingDate() const { return starting_date; }

		string getEndingDate() const { return ending_date; }

		vector<double> calAR(const Stock &benchmark, int N) const;

		vector<double> calCR(const Stock &benchmark, int N) const;

		void Display(Stock& benchmark, int N)const {
			cout << name << endl << endl;

			cout << setw(15) << "Earning Date" << setw(15) << "Period End" << setw(15) << "Estimated"
				<< setw(15) << "Reported" << setw(15) << "Surprise" << setw(15) << "Surprise%" << endl;
			cout << setw(15) << date << setw(15) << period_end;
			for (vector<double>::const_iterator itr = earning.begin(); itr != earning.end(); itr++)
				cout << fixed << setprecision(4) << setw(15) << *itr;
			cout << endl << endl;

			vector<double> CR = calCR(benchmark, N);
			cout << setw(15) << "Date" << setw(15) << "Price" << setw(20) << "Cumulative Return" << endl;
			cout << setw(15) << price_date[0] << setw(15) << fixed << setprecision(2) << price_val[0] << endl;
			for (int i = 1; i < (int)price_date.size(); i++)
				cout << setw(15) << price_date[i] << fixed << setprecision(2) << setw(15) << price_val[i] << fixed << setprecision(4) << setw(20) << CR[i - 1] << endl;
		}
	};

}
