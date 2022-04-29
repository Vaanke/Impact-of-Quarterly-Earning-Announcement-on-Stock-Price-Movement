#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include "Group.h"
#include "Matrix.h"

using namespace std;
namespace fre {
    
    //helper function for plotCAARGraph
    void plotResults(const Vector& xData, const Vector& miss_CAAR, const Vector& meet_CAAR, const Vector& beat_CAAR, const int dataSize) {
        FILE* gnuplotPipe, * tempDataFile;
        const char* filename = "plotdata";
        const char* label1 = "Miss CAAR";
        const char* label2 = "Meet CAAR";
        const char* label3 = "Beat CAAR";
        double x, y1, y2, y3;
        // gnuplotPipe = popen("/opt/local/bin/gnuplot","w");       for MAC
        gnuplotPipe = _popen("gnuplot.exe", "w");
        if (gnuplotPipe) {
            fprintf(gnuplotPipe, "plot \"%s\" using 1:2 w l title \"%s\", '' using 1:3 w l title \"%s\", '' using 1:4 w l title \"%s\" \n",
                filename, label1, label2, label3);
            fflush(gnuplotPipe);

            //create file that contatins all the data to plot
            tempDataFile = fopen(filename, "w");
            for (int i = 0; i < dataSize; i++) {
                x = xData[i];
                y1 = miss_CAAR[i];
                y2 = meet_CAAR[i];
                y3 = beat_CAAR[i];
                fprintf(tempDataFile, "%lf %lf %lf %lf\n", x, y1, y2, y3);
            }
            //Close file after writing
            fclose(tempDataFile);
            //The program has to stop here so gnuplot can run, otherwise the file will be delete and error will appear
            printf("press enter to continue...");
            getchar();
            getchar();
            //remove file
            remove(filename);
            //exit gnuplot
            fprintf(gnuplotPipe, "exit \n");
        }
        else {
            printf("gnuplot not found...");
        }
    }

	void plotCAARGraph(
		const Group* BeatGroup,
		const Group* MeetGroup,
		const Group* MissGroup)
	{
		// plot CAAR graph of all 3 groups
		int N = (int)(BeatGroup->getCAAR().size()/2);
		Vector xData(2*N);
		for (int i = 0; i < 2*N; i++) {
			xData[i] = -(N-1) + i ; 
		}

		plotResults(xData, MissGroup->getCAAR(), MeetGroup->getCAAR(), BeatGroup->getCAAR(), 2*N);
	}

	void showReturnMatrix(const Group* group)
	{
		// show AAR, AAR_STD, CAAR, CAAR_STD matrix of one group

		cout << "AAR:" << endl;
		cout << group->getAAR();
		cout << "AAR Std:" << endl;
		cout << group->getAAR_STD();
		cout << "CAAR:" << endl;
		cout << group->getCAAR();
		cout << "CAAR Std:" << endl;
		cout << group->getCAAR_STD();
	}
}