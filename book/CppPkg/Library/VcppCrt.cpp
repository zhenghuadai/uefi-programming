/*
 * =====================================================================================
 *
 *       Filename:  VcppCrt.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/17/2013 07:54:10 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DAI ZHENGHUA (), djx.zhenghua@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */
#include "cpp.h"
#include <vector>
_STD_BEGIN
void _Throw(stdext::exception const &){}
void _String_base::_Xran(){}
void _String_base::_Xlen(){}

void (__cdecl* std::_Raise_handler)(class stdext::exception const &);
_STD_END

extern "C"{

void _invalid_parameter_noinfo(){}
} // extern "C"{
