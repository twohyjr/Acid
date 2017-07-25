﻿#pragma once

#include "../events/events.h"
#include "../events/eventtime.h"
//#include "../fonts/textobject.h"
#include "../visual/driverslide.h"
#include "guialign.h"
#include "guiobject.h"
#include "screenobject.h"

namespace flounder
{
	class overlaystartup :
		public screenobject
	{
	private:
		guiobject *m_background;
		guiobject *m_logo;
		//	textobject *m_text;
		bool m_starting;

	public:
		overlaystartup(screenobject *parent);

		~overlaystartup();

		void updateObject() override;

		inline bool getStarting() { return m_starting; }

		inline void setStarting(const bool &starting) { m_starting = starting; }
	};
}