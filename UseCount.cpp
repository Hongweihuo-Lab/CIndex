/*============================================
# Filename: UseCount.cpp
# Ver 2.0 2019-06-09
# Copyright (C) 2019 Chenhui Wang, Pengfei Liu, Hongwei Huo, Jeffrey Scott Vitter
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License (GPLv3.0) or later of the License.
#
# Description: 
=============================================*/
#include"UseCount.h"
UseCount::UseCount():p(new int(1)){}

UseCount::UseCount(const UseCount &u):p(u.p){++*p;}

UseCount::~UseCount(){if(--*p==0) delete p;}

bool UseCount::only() {return *p==1;}

bool UseCount::reattach(const UseCount & u)
{
	++*u.p;
	if(--*p==0)
	{
		delete p;
		p=u.p;
		return true;
	}
	p=u.p;
	return false;
}
