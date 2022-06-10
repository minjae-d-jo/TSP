#include <iostream>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <RandomNumber.hpp>
#include <cmath>


using namespace std;
using namespace Snu::Cnrc;

using Size = unsigned int;
using Time = unsigned int;

struct coordinates {
    double x, y;
};

double getDistance(vector<coordinates>& s){
    double distance = 0;
    for(Size i=0; i<s.size()-1; ++i) {
        distance += pow(s[i].x-s[i+1].x, 2) + pow(s[i].y-s[i+1].y, 2);
    }
    distance += pow(s[s.size()-1].x-s[0].x, 2) + pow(s[s.size()-1].y-s[0].y, 2);
    return sqrt(distance);
}

void initializeCoordinates(vector<coordinates>& s) {
    RandomRealGenerator rndReal(0, 1);
    for(Size i=0; i<s.size(); ++i){
        s[i] = coordinates{rndReal(), rndReal()};
    }
    //return s;
}


vector<coordinates> switchCity(vector<coordinates>& s, const Size pickedCity1, const Size pickedCity2) {
    vector<coordinates> sNew = s;
    sNew[pickedCity1] = s[pickedCity2];
    sNew[pickedCity2] = s[pickedCity1];
    return sNew;
}

//void switchCity(vector<coordinates>& s, const Size pickedCity1, const Size pickedCity2) {
//    coordinates temp1, temp2;
//    //cout << "before " << s[pickedCity1].x << '\t' << s[pickedCity2].x << endl;
//    temp1 = s[pickedCity2];
//    temp2 = s[pickedCity1];
//    s[pickedCity1] = temp1;
//    s[pickedCity2] = temp2;
//    //cout << "after " << s[pickedCity1].x << '\t' << s[pickedCity2].x << endl;
//    //sNew[pickedCity1] = s[pickedCity2];
//    //sNew[pickedCity2] = s[pickedCity1];
//    //return sNew;
//}

//////////////////////////////////////////////
/// Print Learing Results
//////////////////////////////////////////////
void printTSP(vector<coordinates>& s, const double T, const double distance, ofstream& file) {
    file << "T = "<< T << ", " << "L = " << distance << '\n';
    for(Size i=0; i<s.size(); ++i){
        file << s[i].x << '\t' << s[i].y << '\n';
    }
    file << s[0].x << '\t' << s[0].y << '\n';
}

void Train(const Size cityNum, const double Tmin) {
    //vector<coordinates> s; 
    vector<coordinates> s(cityNum); 
    vector<coordinates> sNew(cityNum); 
    RandomIntGenerator rndInt(0, cityNum-1);
    RandomRealGenerator rndReal(0, 1);
    double T = 1;
    Size pickedCity1, pickedCity2;
    ostringstream filename; 
    initializeCoordinates(s);
    double deltaL, t = 1;
    while( T>Tmin ) {
        //while( equilibrium? ) {
        for(Time k=1; k<100; ++k){
            do {
                pickedCity1 = rndInt();
                pickedCity2 = rndInt();
            } while ( pickedCity1==pickedCity2 );

            sNew = switchCity(s, pickedCity1, pickedCity2);
            //switchCity(s, pickedCity1, pickedCity2);
            deltaL = getDistance(sNew)-getDistance(s);
            if( deltaL<0 ) {
                s = sNew;
                //switchCity(s, pickedCity1, pickedCity2);
            }
            else {
                if(rndReal() < exp(-deltaL/T)) s = sNew;
            }
        }
        T = 1.*pow(0.95, t);
        if( (Size)t%5==0 ) {
            filename << "TSP_result_"<< (Size)t/10 << ".txt";
            ofstream file(filename.str());
            printTSP(s, T, getDistance(s), file);
            filename.str(""); filename.clear();
        }
        ++t;
	}
}

int main(int, char** argv){
	const Size cityNum = stoi( argv[1] );
    const double Tmin = stod( argv[2] );
	//const Time iter = stoi( argv[3] );
	//const double theta = stod( argv[4] );
	Train(cityNum, Tmin);
	return 0;
}
