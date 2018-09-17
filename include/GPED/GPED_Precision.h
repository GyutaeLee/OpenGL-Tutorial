#ifndef __GPED_PRECISON_H__
#define __GPED_PRECISON_H__

#include <limits>

namespace GPED
{
#if 1
	/*
	조건부 컴파일이 필요한 모든 코드에 대해 single 정밀도 모드로 정의한다
	*/
#define SINGLE_PRECISION
	
	/*
	real 정밀도를 정의한다. 사이클론은 단 single 또는 double 정밀도 버전으로 컴파일 할 수 있다.
	기본적으로는 단 정밀도가 제공된다.
	*/
	typedef float real;

	/* 가장 높은 값의 real number를 정의한다 */
#define REAL_MAX FLT_MAX

	/* square root 연산자의 정밀도를 정의한다 */
#define real_sqrt sqrtf
	/* 절대값 연산자의 정밀도를 정의한다 */
#define real_abs fabsf
	/* sin 연산자의 정밀도를 정의한다 */
#define real_sin sinf

	/* cos 연산자의 정밀도를 정의한다 */
#define real_cos cosf

	/* exponent 연산자의 정밀도를 정의한다 */
#define real_exp expf
	/* power 연산자의 정밀도를 정의한다 */
#define real_pow powf
	/* floating point modulo 연산자의 정밀도를 정의한다 */
#define real_fmod fmodf

	/* 1+e == 1 인 e 를 정의한다 */
#define real_epsilon FLT_EPSILON

#define R_PI 3.14159f

#else
#define DOUBLE_PRECISION
	typedef double real;
#define REAL_MAX DBL_MAX
#define real_sqrt sqrt
#define real_abs fabs
#define real_sin sin
#define real_cos cos
#define real_exp exp
#define real_pow pow
#define real_fmod fmod
#define real_epsilon DBL_EPSILON
#define R_PI 3.14159265358979
#endif

}

#endif