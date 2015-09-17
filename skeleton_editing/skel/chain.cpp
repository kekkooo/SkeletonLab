#include "chain.h"
#include <iostream>

using namespace Skel;

Chain::Chain()
{
        tot_length = 0.f;
}

ChainNode Chain::sample(float p) const
{
        if (p <= 0) return nodes.front();

        float p0 = 0.f;
        int   i  = 0;

        while (p0 < p)
        {
                if (i >= nodes.size() - 1) return nodes.back();
                p0 += step_length(i);
                ++i;
        }

        assert(i < nodes.size());
        return ChainNode::mix(nodes[i], nodes[i-1], (p0 - p)/step_length(i-1));
}


float Chain::step_length(int node) const // from <node> to <node+1>
{
        assert(node < nodes.size()-1);
        return (nodes[node].coord - nodes[node+1].coord).Norm();
}


Chain Chain::resample(int prism_sides) const
{
        Chain a(*this);
        Chain b = a.flip();

        a = a.resample_one_way(prism_sides);
        b = b.resample_one_way(prism_sides);
        b = b.flip();

        return mix(a,b);
}



ChainNode::ChainNode(Point _coord, float _radius) : coord(_coord), radius(_radius)
{}

ChainNode ChainNode::mix(const ChainNode &c1, const ChainNode &c2, float t)
{
        return ChainNode
        (
                c1.coord  * (1-t) + c2.coord * (t),
                c1.radius * (1-t) + c2.radius * (t)
        );
}


void Chain::add_node(ChainNode node)
{
        nodes.push_back(node);
        if (nodes.size() > 1) tot_length += step_length(nodes.size() - 2);
}



void Chain::ensure_one_node()
{
        assert(nodes.size() >= 2);
        if (nodes.size() == 2)
        {
                add_node(nodes[1]);
                nodes[1] = ChainNode::mix(nodes[0], nodes[1], 0.5);
        }

}


Chain Chain::flip() const
{
        Chain res;
        for(int i=nodes.size()-1; i >= 0; --i) res.add_node(nodes[i]);
        assert(res.n_nodes() == this->n_nodes());
        return res;
}


void Chain::ensure_same_n_nodes(Chain & ca, Chain & cb)
{
        if (ca.n_nodes() != cb.n_nodes()) std::cout << "CATENE DIVERSE!!!!" << ca.n_nodes() << "\t" << cb.n_nodes() << std::endl;

        if (ca.n_nodes() < cb.n_nodes()) cb.nodes.resize(ca.n_nodes());
        if (cb.n_nodes() < ca.n_nodes()) ca.nodes.resize(cb.n_nodes());
        assert(ca.n_nodes() == cb.n_nodes());
}

Chain Chain::mix(Chain & ca, Chain & cb)
{
        Chain::ensure_same_n_nodes(ca, cb);

        Chain res;
        for(int i=0; i<ca.n_nodes(); ++i)
        {
                res.add_node(ChainNode::mix(ca[i], cb[i], (float)i/(float)(ca.n_nodes() - 1)));
        }
        return res;
}

Chain Chain::resample_one_way(int prism_sides) const
{
        Chain new_chain;

        float t = 0;

        std::cout << "chain tot length: " << tot_length << std::endl;

        while(t < tot_length)
        {
                ChainNode s = sample(t);
                t += s.radius * (2 * M_PI) / (float)prism_sides;
                new_chain.add_node(s);
        }

        return new_chain;
}


bool is_equal(const Chain & ca, const Chain & cb)
{

}

bool is_equal(const ChainNode & na, const ChainNode & nb)
{

}
