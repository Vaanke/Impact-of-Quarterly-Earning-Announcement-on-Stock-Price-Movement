#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <locale>
#include <iomanip>
#include <fstream>
#include <cmath>
#include "curl/curl.h"
#include "Stock.h"
#include "DataReader.h"

using namespace std;

namespace fre {
	void readBasic(map<string, Stock>* BeatStocks,
		map<string, Stock>* MeetStocks,
		map<string, Stock>* MissStocks,
		double thr1, double thr2)
	{
		map<string, string> temp;
		ifstream fin1;
		fin1.open("Russell_1000_component_stocks.csv", ios::in);
		string line, name, symbol;
		while (getline(fin1, line))
		{
			stringstream sin(line);
			getline(sin, symbol, ',');
			getline(sin, name);
			temp[symbol] = name;
		}

		ifstream fin2;
		fin2.open("EarningsAnnouncements (duplicate removed).csv", ios::in);
		getline(fin2, line); // drop first line
		string date, period_end, estimate, reported, surprise, surprise_pct;
		while (getline(fin2, line))
		{
			stringstream sin(line);
			vector<double> earning;
			getline(sin, symbol, ',');
			getline(sin, date, ',');
			getline(sin, period_end, ',');
			getline(sin, estimate, ',');
			earning.push_back(stod(estimate));
			getline(sin, reported, ',');
			earning.push_back(stod(reported));
			getline(sin, surprise, ',');
			earning.push_back(stod(surprise));
			getline(sin, surprise_pct);
			earning.push_back(stod(surprise_pct));
			Stock stocki(symbol, temp[symbol], date, period_end, earning);

			if (stod(surprise_pct) < thr1)
			{
				(*MissStocks)[symbol] = stocki;
			}
			else if (stod(surprise_pct) > thr2)
			{
				(*BeatStocks)[symbol] = stocki;
			}
			else
			{
				(*MeetStocks)[symbol] = stocki;
			}
		}
	}

	struct MemoryStruct {
		char* memory;
		size_t total_size;
		size_t size;
	};
	void* myrealloc(void* ptr, size_t size)
	{
		if (ptr)
			return realloc(ptr, size);
		else
			return malloc(size);
	}
	int write_data(void* ptr, size_t size, size_t nmemb, void* data)
	{
		size_t realsize = size * nmemb;
		struct MemoryStruct* mem = (struct MemoryStruct*)data;
		if ((mem->size + realsize) >= mem->total_size)
			mem->memory = (char*)myrealloc(mem->memory, mem->size + realsize + 1);
		//mem->memory = (char*)myrealloc(mem->memory, mem->size + realsize + 1);
		if (mem->memory) {
			memcpy(&(mem->memory[mem->size]), ptr, realsize);
			mem->size += realsize;
			mem->memory[mem->size] = 0;
			if (mem->size > mem->total_size)
				mem->total_size = mem->size;
		}
		return realsize;
	}
	string calNDate(string date0, int N)
	{
		int year, month, day;
		tm tm_;
		sscanf(date0.c_str(), "%d-%d-%d", &year, &month, &day);
		tm_.tm_year = year - 1900;
		tm_.tm_mon = month - 1;
		tm_.tm_mday = day;
		tm_.tm_hour = 0;
		tm_.tm_min = 0;
		tm_.tm_sec = 0;
		time_t date = mktime(&tm_);
		date += (time_t)N * 86400;  // 1 day = 86400 sec

		tm* tm_new = gmtime(&date);
		char c[11] = { 0 };
		strftime(c, 11, "%Y-%m-%d", tm_new);
		string str(c);
		return str;
	}

	void readPrice_group(map<string, Stock>* stocks, int N, const Stock* benchmark)
	{
		vector<string> trade_days = benchmark->getPriceDate(); // reference trading days
		// declaration of an object CURL
		CURL* handle;
		CURLcode result;
		// set up the program environment that libcurl needs
		// curl_global_init(CURL_GLOBAL_ALL);
		// curl_easy_init() returns a CURL easy handle
		handle = curl_easy_init();

		// if everything's all right with the easy handle...
		if (handle)
		{
			const string url_common = "https://eodhistoricaldata.com/api/eod/";
			const string api_token = "61ad392951f604.64641793";
			string symbol, start_date, end_date;
			string url_request;

			struct MemoryStruct data;
			data.memory = NULL;
			data.size = 0;
			data.total_size = 0;

			vector<string>::iterator itr_date0;
			map<string, Stock>::iterator itr = stocks->begin();

			for (; itr != stocks->end(); itr++)
			{
				memset(data.memory, '\0', data.total_size);
				data.size = 0;
				//data.memory = NULL;

				itr_date0 = find(trade_days.begin(), trade_days.end(), itr->second.getAnnounceDate());
				start_date = *(itr_date0 - N);
				end_date = *(itr_date0 + N);
				symbol = itr->first;
				url_request = url_common + symbol + ".US?" + "from=" + start_date + "&to=" + end_date + "&period=d" + "&api_token=" + api_token;

				curl_easy_setopt(handle, CURLOPT_URL, url_request.c_str());
				// adding a user agent
				curl_easy_setopt(handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:74.0) Gecko/20100101 Firefox/74.0");
				curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
				curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);
				curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
				curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)&data);

				// perform, then store the expected code in result
				result = curl_easy_perform(handle);

				if (result != CURLE_OK)
				{
					//error occured
					fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
					return;
				}

				vector<string> price_dates;
				vector<double> price_nums;
				stringstream sData;
				sData.str(data.memory);
				string sValue, sDate;
				double dValue = 0;
				string line;
				getline(sData, line); // skip first line
				while (getline(sData, line))
				{
					size_t found = line.find('-');
					//cout << line << endl;
					if (found != std::string::npos)
					{
						sDate = line.substr(0, line.find_first_of(','));
						line.erase(line.find_last_of(','));
						sValue = line.substr(line.find_last_of(',') + 1);
						dValue = strtod(sValue.c_str(), NULL);
						price_dates.push_back(sDate);
						price_nums.push_back(dValue);
					}
					else break;
				}

				if ((int)price_dates.size() < (2 * N + 1))
				{
					// not enough days
					cout << itr->first << " deleted. Not enough dates." << endl;
					stocks->erase(itr--);
				}
				else
				{
					itr->second.updatePrice(price_dates, price_nums);
				}

			}
			free(data.memory);
			data.size = 0;
			data.total_size = 0;
		}
	}

	void readPrice(Stock& stock, string from, string to)
	{
		// retrieve benchmark(IWB) price within given range, and update Stock object
		CURL* handle;
		CURLcode result;
		// set up the program environment that libcurl needs
		curl_global_init(CURL_GLOBAL_ALL);
		// curl_easy_init() returns a CURL easy handle
		handle = curl_easy_init();
		// if everything's all right with the easy handle...
		if (handle)
		{
			const string url_common = "https://eodhistoricaldata.com/api/eod/";
			const string api_token = "61ad392951f604.64641793";
			string url_request;

			struct MemoryStruct data;
			data.memory = NULL;
			data.size = 0;
			data.total_size = 0;

			url_request = url_common + stock.getSymbol() + ".US?" + "from=" + from + "&to=" + to + "&period=d" + "&api_token=" + api_token;
			curl_easy_setopt(handle, CURLOPT_URL, url_request.c_str());
			// adding a user agent
			curl_easy_setopt(handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:74.0) Gecko/20100101 Firefox/74.0");
			curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
			curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);
			curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
			curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)&data);

			// perform, then store the expected code in result
			result = curl_easy_perform(handle);

			if (result != CURLE_OK)
			{
				//error occured
				fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
				return;
			}

			vector<string> price_dates;
			vector<double> price_nums;
			stringstream sData;
			sData.str(data.memory);
			string sValue, sDate;
			double dValue = 0;
			string line;
			while (getline(sData, line))
			{
				size_t found = line.find('-');
				if (found != std::string::npos)
				{
					sDate = line.substr(0, line.find_first_of(','));
					line.erase(line.find_last_of(','));
					sValue = line.substr(line.find_last_of(',') + 1);
					dValue = strtod(sValue.c_str(), NULL);
					price_dates.push_back(sDate);
					price_nums.push_back(dValue);
				}
			}
			stock.updatePrice(price_dates, price_nums);

			free(data.memory);
			data.size = 0;
			data.total_size = 0;
		}
	}

	vector<string> readDate(int N)
	{
		string date_min = "9999-99-99";
		string date_max = "0000-00-00";

		ifstream fin2;
		string line, symbol, date;
		fin2.open("EarningsAnnouncements (duplicate removed).csv", ios::in);
		getline(fin2, line); // drop first line
		while (getline(fin2, line))
		{
			stringstream sin(line);
			getline(sin, symbol, ',');
			getline(sin, date, ',');

			if (date < date_min) date_min = date;
			if (date > date_max) date_max = date;
		}
		int N_ = ceil(N * 7 / 5 + 10); // considering weekend and holiday

		return { calNDate(date_min, -N_), calNDate(date_max, N_) };
	}
}