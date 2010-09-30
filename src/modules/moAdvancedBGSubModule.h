/***********************************************************************
 ** Copyright (C) 2010 Movid Authors.  All rights reserved.
 **
 ** This file is part of the Movid Software.
 **
 ** This file may be distributed under the terms of the Q Public License
 ** as defined by Trolltech AS of Norway and appearing in the file
 ** LICENSE included in the packaging of this file.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** Contact info@movid.org if any conditions of this licensing are
 ** not clear to you.
 **
 **********************************************************************/


#ifndef MO_SMOOTH_MODULE_H
#define MO_SMOOTH_MODULE_H

#include "moImageFilterModule.h"

#include <assert.h>
#include "../moLog.h"
#include "cv.h"

#include "AdaptiveMedianBGS.hpp"
#include "GrimsonGMM.hpp"
#include "ZivkovicAGMM.hpp"
#include "MeanBGS.hpp"
#include "WrenGA.hpp"
#include "PratiMediodBGS.hpp"
#include "Eigenbackground.hpp"

class moAdvancedBGSubModule : public moImageFilterModule8 {
public:
	moAdvancedBGSubModule();
	virtual ~moAdvancedBGSubModule();
	
protected:
	bool initialized;
	Algorithms::BackgroundSubtraction::EigenbackgroundParams params;
	Algorithms::BackgroundSubtraction::Eigenbackground bgs;

	void applyFilter(IplImage *);
	int width, height;

	MODULE_INTERNALS();
};

#endif

