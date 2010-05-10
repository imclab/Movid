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


#include <assert.h>
#include "moContourFinderModule.h"
#include "../moLog.h"
#include "cv.h"

// XXX This is currently rather the scratchspace for a fingertip finder...
MODULE_DECLARE(ContourFinder, "native", "ContourFinder finds contours in an image");

typedef std::pair<float, CvConvexityDefect*> depthToDefect;

moContourFinderModule::moContourFinderModule() : moImageFilterModule(){

	MODULE_INIT();

	this->storage = cvCreateMemStorage(0);
	this->properties["min_distance"] = new moProperty(20.);
	this->properties["min_area"] = new moProperty(150.);
}

moContourFinderModule::~moContourFinderModule() {
	cvReleaseMemStorage(&this->storage);
}

bool _sort_pred(const depthToDefect &left, const depthToDefect &right) {
	return left.first > right.first;
}

void moContourFinderModule::applyFilter(IplImage *src) {
	// Create a copy since cvFindContours will manipulate its input
	std::cout << "BEGINNING FRAME ================================================" << std::endl;
	cvCopy(src, this->output_buffer);
	CvSeq *contours = 0;
	cvFindContours(this->output_buffer, this->storage, &contours, sizeof(CvContour), CV_RETR_CCOMP);
	cvZero(this->output_buffer);
	if(contours) {
		// Find the exterior contour (i.e. the hand has to be white) that has the greatest area
		CvSeq *max_cont = contours, *cur_cont = contours;
		double area, max_area = 0;
		while (cur_cont != 0) {
			area = cvContourArea(cur_cont);
			area *= area;
			if (area >= max_area) {
				max_area = area;
				max_cont = cur_cont;
			}
			cur_cont = cur_cont->h_next;
		}
		// Ignore contours whose area is too small, they're likely not hands anyway
		if (max_area < this->property("min_area").asDouble())
			return;
		contours = max_cont;

		cvDrawContours(this->output_buffer, contours, cvScalarAll(255), cvScalarAll(255), 100);

		// Compute the convex hull of the contour
		CvMemStorage *hull_storage = cvCreateMemStorage(0);
		CvSeq* hull = 0;
		hull = cvConvexHull2(contours, hull_storage, CV_CLOCKWISE, 0);

		// Compute the convexity defects of the convex contour with respect to the convex hull.
		// The fingertips are at the start and endpoints of the defects
		CvMemStorage *defects_storage = cvCreateMemStorage(0);
		CvSeq* defects = 0;
		defects = cvConvexityDefects(contours, hull, defects_storage);

		// Find the 5 best defects
		std::vector<depthToDefect> def_depths;
		for(int i=0; i < defects->total; ++i) {
			CvConvexityDefect* defect = (CvConvexityDefect*)cvGetSeqElem (defects, i);
			def_depths.push_back(depthToDefect(defect->depth, defect));
		}
		std::sort(def_depths.begin(), def_depths.end(), _sort_pred);
		for (unsigned int i=0; i < def_depths.size(); i++) {
			// 5 Fingers max
			if (i == 5)
				break;
			float depth = def_depths[i].first;
			double min_dist = this->property("min_distance").asDouble();
			if (min_dist > depth)
				continue;
			CvConvexityDefect *defect = def_depths[i].second;
			std::cout << depth << std::endl;
			int radius = cvRound(10);
			cvCircle(this->output_buffer, *(defect->start), radius, CV_RGB(255, 255, 255), -1);
			cvCircle(this->output_buffer, *(defect->end), radius, CV_RGB(255, 255, 255), -1);
		}
	}
}


