#pragma once
#include "file.h"
#include "process/process.h"
class OSManager
{
public:
	File file;
	ProcessManager pm;

	OSManager() {
		auto& timer = pm.getTimer();
		timer.setCallBack([this]() {
			file.commandChangePath();
		});
	}
};

