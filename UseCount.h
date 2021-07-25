/*============================================
# Filename: UseCount.h
# Ver 2.0 2019-06-09
# Copyright (C) 2019 Chenhui Wang, Hongwei Huo, Jeffrey Scott Vitter
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License (GPLv3.0) or later of the License.
#
# Description: a class used for use-count.
  Ruminations on C++,Chapter 7.
=============================================*/
#ifndef USECOUNT_H
#define USECOUNT_H
class UseCount
{
	public:
		UseCount();
		UseCount(const UseCount &);
		~UseCount();
		bool only();
		bool reattach(const UseCount &);
	private:
		int *p;
		UseCount & operator = (const UseCount &);
};
#endif
