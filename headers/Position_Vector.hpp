#ifndef HEADERS_POSITION_VECTOR_H_
#define HEADERS_POSITION_VECTOR_H_
#include <inttypes.h>
#include <iostream>
class Position_Vector {
public:
Position_Vector(int32_t X, int32_t Y, int32_t Z, int32_t A) : X(X), Y(Y),Z(Z),A(A){
}

int32_t X;
int32_t Y;
int32_t Z;
int32_t A;
//uint64_t epoch;//TODO : add time
void printvect()
{
        std::cout << "Pos x : "<< X << " Pos y : "<< Y << " Pos z : "<< Z << " Pos a : "<< A <<  std::endl;
}
};


#endif
