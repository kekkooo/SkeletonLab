#pragma once

#include <vector>
#include <vcg/space/point3.h>


namespace Skel
{

typedef vcg::Point3f Point;


class ChainNode
{
public:

        ChainNode(){}
        ChainNode(Point _coord, float _radius);

        static ChainNode mix(const ChainNode & c1, const ChainNode & c2, float t);

        bool operator== (const ChainNode & n) const { return (coord == n.coord && radius == n.radius); }

        Point coord;
        float radius;
};


class Chain
{
        public:

                Chain();

                void add_node(ChainNode node);

                Chain resample(int prism_sides) const;

                Chain resample_one_way(int prism_sides) const;

                ChainNode sample(float p) const; // p \in [0, tot_length]

                float step_length(int node) const; // from <node> to <node+1>

                void ensure_one_node();

                Chain flip() const;

                inline unsigned int n_nodes() const { return nodes.size(); }

                static Chain mix(Chain & ca, Chain & cb);

                static void ensure_same_n_nodes(Chain & ca, Chain & cb);

                ChainNode & operator[] (int i) { return nodes[i]; }
                const ChainNode & operator[] (int i) const { return nodes[i]; }                


        float tot_length;
        std::vector<ChainNode> nodes;
};




}
