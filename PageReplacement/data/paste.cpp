#include <iostream>
using namespace std;

int main(){
    int size;
    cout << "The size of resluts: ";
    cin >> size;
    
    cout << "paste -d \",\" ";
    for (int i = 1; i <= size; ++i){
        cout << "results" << i << " ";
    }
    cout << " > results" << endl;
    
    return 0;
}
