#include <iostream>

using namespace std;

int sum(int n, int p)
{
    return n + p;
}

void calc(  int (*fptr)(int, int),  int n, int p) 
{
    cout << fptr(n, p) << endl;
}

class CCalc {
public:
    void operator()(int n, int p) {
        cout << "functor-sum:" << n + p << endl;
    }
};

int main()
{
    cout << "main is called" << endl;

    //=============================================
    // function pointer
    int (*calc)(int, int) = sum; 
    cout << "fptr-sum: "<<calc(1, 2) << endl; // output: 3 

    //=============================================
    // functor
    CCalc oCalc;
    oCalc(1, 2);

    //=============================================
    // ramda 
    // ramda is a anonymous function
    //[ capture clause ] (parameters) -> return-type  
    //{   
    //    definition of method   
    //} 
    auto aCalc = [](int n, int p) -> int { return n + p; };
    cout << "ramda-sum: " << aCalc(1, 2) << endl; // output: 3

    // void return ramda function
    int num1 = 0, num2 = 1;
    auto aCalc2 = [&num1, num2](int n, int p) { 
        num1 += num2 + n + p;
        //num2 = 100; // error, copy value is not modified
    };

    aCalc2(2, 3);
    cout << "ramda-sum2: " << num1 << endl; // output : 6

     // void return ramda function - mutable
    int num3 = 0;
    auto aCalc3 = [&num3]() mutable { 
        num3 += 1 ;
    };

    aCalc3();
    cout << "ramda-sum3: " << num3 << endl; // output : 1

     // void return ramda function - closer
    int nCloser = 0;
    auto aCalc4 = [&nCloser]() mutable { 
        nCloser += 1 ;
        cout << "ramda-aCalc4: " << nCloser << endl; // output : 2    
    };

    aCalc4();
    cout << "ramda-sum4: " << nCloser << endl; // output : 1    
    aCalc4();
    cout << "ramda-sum4-1: " << nCloser << endl; // output : 1    

    return 0;
}