
#include <vector>
#include <string>
#include <map>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include "Stock.h"
#include "Group.h"
#include "Matrix.h"
using namespace std;


namespace fre {
    typedef vector<vector<double>> Matrix;
    
    void validator(int selectNum, int mapsize)
    {
        if (selectNum > mapsize){
            cout << "";
            cout << "Selection Number MUST BE LESS THAN the number of stocks = "<< mapsize << endl;
            cout << "";
        }
    }

    void Group::calByBootstrap(const Stock &benchmark, int selectNum, int simuNum)
    {
        int idx = 0;
        Matrix ARmat(selectNum);
        Matrix CARmat(selectNum);
        Matrix BStrapARmat(simuNum);
        Matrix BStrapCARmat(simuNum);
        validator(selectNum, (int)(*stocks).size());
        
        map<string, Stock> ::iterator itr = (*stocks).begin();
        int plen = 2 * N; // get price lenth 2N
        
        // Initialization for BStrapAAR and BStrapCAAR matrix for all simulation, shape = (simuNum, 2N)
        for (int i = 0; i < simuNum; i++)
        {
            BStrapARmat[i].resize(plen);
            BStrapCARmat[i].resize(plen);
        }
        // Initialization for AR and CAR matrix for each simulation, shape = (selectNum, 2N)
        for (int i = 0; i < selectNum; i++)
        {
            ARmat[i].resize(plen);
            CARmat[i].resize(plen);
        }
        
        // BOOTSTRAP, iter: random selection and calc for simuNum times
        for (int j = 0; j < simuNum; j++)
        {
            // Each simulation: populate AR and CAR matrix, shape = (selectNum, 2N)
            for (int i = 0; i < selectNum; i++)
            {
                idx = rand() % stocks->size(); //random idx
                advance(itr, idx);
                ARmat[i] = itr -> second.calAR(benchmark, N);
                CARmat[i] = itr -> second.calCR(benchmark, N);
                itr = (*stocks).begin();
            }
            BStrapARmat[j] = matrixMean(ARmat);
            BStrapCARmat[j] = matrixMean(CARmat);
        }
        
        AAR = matrixMean(BStrapARmat);
        AAR_STD = matrixStd(BStrapARmat);
        CAAR = matrixMean(BStrapCARmat);
        CAAR_STD = matrixStd(BStrapCARmat);
    }
    
}
