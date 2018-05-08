/* Adapted from: https://eigen.tuxfamily.org/dox/classEigen_1_1JacobiSVD.html */

#include <iostream>
using std::cout;
using std::endl;

#include <Eigen/Core>
#include <Eigen/SVD>
using Eigen::ComputeThinU;
using Eigen::ComputeThinV;
using Eigen::JacobiSVD;
using Eigen::MatrixXf;
using Eigen::Vector3f;

int main() {
	MatrixXf m = MatrixXf::Random(3,2);
	JacobiSVD<MatrixXf> svd(m, ComputeThinU | ComputeThinV);
	Vector3f rhs(1, 0, 0);
	cout << "m = " << endl << m << endl << endl;
	cout << "svd.singularValues() = " << endl << svd.singularValues() << endl << endl;
	cout << "svd.MatrixU() = " << endl << svd.matrixU() << endl << endl;
	cout << "svd.MatrixV() = " << endl << svd.matrixV() << endl << endl;
	cout << "svd.solve() = " << endl << svd.solve(rhs) << endl << endl;
}
