// Spatial Index Library
//
// Copyright (C) 2004  Navel Ltd.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//  Email:
//    mhadji@gmail.com

#pragma once

namespace SpatialIndex
{
	class SIDX_DLL Region : public Tools::IObject, public virtual IShape
	{
	public:
		Region();
		Region(uint32_t dimension);
		Region(const double* pLow, const double* pHigh, uint32_t dimension);
		Region(const Point& low, const Point& high);
		Region(const Region& in);
		virtual ~Region();

		virtual Region& operator=(const Region& r);
		virtual bool operator==(const Region&) const;

		//
		// IObject interface
		//
		virtual Region* clone();

		//
		// ISerializable interface
		//
		virtual uint32_t getByteArraySize();
		virtual void loadFromByteArray(const byte* data);
		virtual void storeToByteArray(byte** data, uint32_t& length);

		//
		// IShape interface
		//
		virtual bool intersectsShape(const IShape& in) const;
		virtual bool containsShape(const IShape& in) const;
		virtual bool touchesShape(const IShape& in) const;
		virtual void getCenter(Point& out) const;
		virtual uint32_t getDimension() const;
		virtual void getMBR(Region& out) const;
		virtual double getArea() const;
		virtual double getMinimumDistance(const IShape& in) const;

		virtual bool intersectsRegion(const Region& in) const;
		virtual bool containsRegion(const Region& in) const;
		virtual bool touchesRegion(const Region& in) const;
		virtual double getMinimumDistance(const Region& in) const;

		virtual bool containsPoint(const Point& in) const;
		virtual bool touchesPoint(const Point& in) const;
		virtual double getMinimumDistance(const Point& in) const;

		virtual Region getIntersectingRegion(const Region& r) const;
		virtual double getIntersectingArea(const Region& in) const;
		virtual double getMargin() const;

		virtual void combineRegion(const Region& in);
		virtual void combinePoint(const Point& in);
		virtual void getCombinedRegion(Region& out, const Region& in) const;

		virtual double getLow(uint32_t index) const;
		virtual double getHigh(uint32_t index) const;

		virtual void makeInfinite(uint32_t dimension);
		virtual void makeDimension(uint32_t dimension);


		double getHausDistLB(const IShape& in) const;
		double getHausDistLB(const std::vector<const Region*> vec_pMBR, double max, int& counter) const;
		
		double getMHausDistLB(const IShape& in) const;
		double getMHausDistLB(const std::vector<const Region*> vec_pMBR, double max) const;

		double getHausDistUB(const IShape& in) const;
		double getHausDistUB(const Region& in) const;
		double getHausDistUB(const Point& in) const;
		double getHausDistUB(const std::vector<const IShape*> vec_pShape) const;


		double getMinimumDistanceSq(const IShape& in) const;
		double getMinimumDistanceSq(const Region& in) const;
		double getMinimumDistanceSq(const Point& p) const;
		double getMaximumDistanceSq(const Region& in) const;

		void getEdge(int edgeId, Region& cout) const;

	private:
		void initialize(const double* pLow, const double* pHigh, uint32_t dimension);
		void initialize(uint32_t dimension);

	public:
		uint32_t m_dimension;
		double* m_pLow;
		double* m_pHigh;
		std::vector<Region*> m_vec_pEdge;

		friend SIDX_DLL std::ostream& operator<<(std::ostream& os, const Region& r);
	}; // Region

	SIDX_DLL std::ostream& operator<<(std::ostream& os, const Region& r);
}
