#pragma once
#include <vector>
#include <iostream>
using namespace std;
namespace fre {

typedef vector<double> Vector;
typedef vector<Vector> Matrix;


Vector operator+(const Vector& V, const Vector& W);
Vector operator-(const Vector& V, const Vector& W);
Vector operator/(const Vector& V, const double& a);
Vector Vpower(const Vector& V, double a);
Matrix Mpower(const Matrix& M, double a);
vector<double> matrixMean(const Matrix& mat);
vector<double> matrixStd(const Matrix& mat);
ostream& operator<<(ostream& out, const Vector& V);
}
