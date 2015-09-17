#include "select.h"

namespace Skel
{
namespace Select
{


// return the full list of inner points (i.e. no terminal points) of a given bone (<id>)
//
QList<int> bone_without_endpoints(CurveSkeleton *cs, int id)
{

        assert(cs->bones.contains(id));
        //QList<int> list;
        //if (!cs->bones.contains(id)) return list;

        return cs->bones[id].mid(1, cs->bones[id].size() - 2);
}




QList<int> incident_bones(CurveSkeleton *cs, int fp, int filter)
{

        //assert( cs->points[fp].isF() );


        QList<int> list;


        for (CurveSkeleton::BoneIterator it = cs->bones.begin(); it != cs->bones.end(); ++it)
        {

                if (it.value().first() == fp || it.value().last() == fp)
                {

                        if (it.key() != filter)  list.append(it.key());
                }
        }

        return list;
}


}
}
