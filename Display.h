#pragma once
#include "Group.h"
#include "Matrix.h"

using namespace std;
namespace fre {

	//helper function for plotCAARGraph
	void plotResults(const Vector& xData, const Vector& miss_CAAR, const Vector& meet_CAAR, const Vector& beat_CAAR, const int dataSize);

	void plotCAARGraph(
		const Group* BeatGroup,
		const Group* MeetGroup,
		const Group* MissGroup);

	void showReturnMatrix(const Group* group);
}