#pragma once
#include "file.h"
#include<iostream>
#include "process/process.h"
class OSManager
{
public:
	File file;
	ProcessManager pm;

	void mainControl() {

		file.loginIn();

	}

};

