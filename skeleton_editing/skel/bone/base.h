#pragma once

//#include <QList>
//#include <QStack>
#include <QList>
#include <QStack>
#include <algorithm>

using namespace std;

namespace Skel
{

	// a bone is just a list of skeleton point IDs belonging to the same chain.
	// it starts with a feature point and ends with a feature point.
	//
	class Bone : public QList<int>
	{

	public:

		Bone()
		{
			//transform.SetIdentity();
			isLoop = false;
		}

		// invert the orientation of the bone
		//
		void reverse()
		{
			QStack<int> stack;

			foreach (int point, *this) stack.push(point);

			clear();

			while (!stack.isEmpty()) append(stack.pop());
		}

		// IDs of the surface vertices attached to the bone after the rigging process
		//
		std::list<int> surfacePoints;

		inline int num_of_points(){ return size(); }
		bool isLoop;
	};

}
