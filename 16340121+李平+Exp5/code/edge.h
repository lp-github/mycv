#pragma once

#include "vector2.h"


class Edge
{
	public:
		using VertexType = Vector2;

		Edge(const VertexType &p1, const VertexType &p2) : p1(p1), p2(p2), isBad(false) {};
		Edge(const Edge &e) : p1(e.p1), p2(e.p2), isBad(false) {};

		VertexType p1;
		VertexType p2;

		bool isBad;
};


inline std::ostream &operator << (std::ostream &str, Edge const &e)
{
	return str << "Edge " << e.p1 << ", " << e.p2;
}


inline bool operator == (const Edge & e1, const Edge & e2)
{
	return 	(e1.p1 == e2.p1 && e1.p2 == e2.p2) ||
			(e1.p1 == e2.p2 && e1.p2 == e2.p1);
}


inline bool almost_equal (const Edge & e1, const Edge & e2)
{
	return	(almost_equal(e1.p1, e2.p1) && almost_equal(e1.p2, e2.p2)) ||
			(almost_equal(e1.p1, e2.p2) && almost_equal(e1.p2, e2.p1));
}



