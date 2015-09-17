#pragma once


#ifdef use_boost

#include <mesh/mesh.h>
#include <primitives/geometry.h>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <QString>
#include <QFile>
#include <QMap>

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

namespace RMesh
{

namespace Importer
{
static void from_obj( RMesh::mesh &m, QString filename)
{
        typedef boost::char_separator<char>     CharSeparator;
        typedef boost::tokenizer<CharSeparator> Tokenizer;
        typedef Tokenizer::iterator             TokenizerIterator;

        CharSeparator space_separator(" ");
        CharSeparator slash_separator("/");

        static string vertex = "v";
        static string face   = "f";

        ifstream file(filename.toStdString().c_str());
        string   line;

        if(!file.is_open())
        {
            return;
        }


        while(getline(file,line))
        {
            Tokenizer space_tokenizer(line, space_separator);

            if (space_tokenizer.begin() == space_tokenizer.end()) continue;

            TokenizerIterator token = space_tokenizer.begin();
            string header = *token;

            // Handle
            //
            // v 0.123 0.234 0.345
            // v 0.123 0.234 0.345 1.0

            if (strcmp(header.c_str(), vertex.c_str()) == 0)
            {
                string x = *(++token);
                string y = *(++token);
                string z = *(++token);

                Primitives::Point3d v(   atof(x.c_str()),
                                        atof(y.c_str()),
                                        atof(z.c_str()));
                m.vertices.push_back(v);
            }

            else

            // Handle
            //
            // f 1 2 3
            // f 3/1 4/2 5/3
            // f 6/4/1 3/5/3 7/6/5
            //
            // NOTE: 6/4/1 => only 6 (vertex id) will be stored in the mesh

            if (strcmp(header.c_str(), face.c_str()) == 0)
            {
                MeshPolygon poly;

                for(Tokenizer::iterator it=++(space_tokenizer.begin()); it!=space_tokenizer.end(); ++it)
                {
                    Tokenizer slash_tokenizer(*it, slash_separator);
                    string str_nid = *slash_tokenizer.begin();
                    int int_nid = atoi(str_nid.c_str());
                    poly.vertices.push_back(int_nid - 1);
                }

                m.polygons.push_back(poly);
            }
        }

        m.bbox.Set(m.vertices);

}

static void from_off( RMesh::mesh &m, QString filename)
{
    typedef boost::char_separator<char>     CharSeparator;
    typedef boost::tokenizer<CharSeparator> Tokenizer;
    typedef Tokenizer::iterator             TokenizerIterator;

    CharSeparator space_separator(" ");

    ifstream file(filename.toStdString().c_str());
    string   line;

    if(!file.is_open())
    {
        return;
    }

    getline(file,line);    
    boost::algorithm::trim(line);

    assert( line.compare("OFF") == 0 );
    while(getline(file,line)) { boost::algorithm::trim(line); if( line.find('#') >= line.length()) break;  }

    cout << line << endl;
    int vs, fs, hes;
    sscanf(line.c_str(), "%d %d %d", &vs, &fs, &hes );

    m.vertices.reserve(vs);
    m.polygons.reserve(fs);


    for( int i = 0; i < vs; ++i)
    {
        getline(file,line);
        double x,y,z;
        sscanf(line.c_str(), "%lf %lf %lf", &x, &y, &z );
//        cout << i << ") " << x << ", " << y << ", " << z << endl;
        Primitives::Point3d p( x, y, z );
        m.vertices.push_back(p);
    }
    for( int i = 0; i < fs; ++i)
    {
        getline(file,line);
        MeshPolygon f;
        Tokenizer space_tokenizer(line, space_separator);
        if ( space_tokenizer.begin() == space_tokenizer.end( )) continue;
        TokenizerIterator token = space_tokenizer.begin();
        string nf = *token;
        int num_of_vertices = atoi( nf.c_str( ));
        assert( num_of_vertices > 2 );
        ++token;
        f.vertices.reserve( num_of_vertices );
        while( token != space_tokenizer.end())
        {            
            string vertex = *(token);
            f.vertices.push_back( atoi( vertex.c_str( )));
            ++token;
        }
        m.polygons.push_back(f);

    }
    m.bbox.Set(m.vertices);
    file.close();
}

//        static void meshCopy( PolyMesh *source, PolyMesh *destination )
//        {
//            std::vector<DCEL::Vertex> vertices;
//            std::vector<DCEL::Face> faces;

//            foreach ( DCEL::Vertex v, source->v ) { vertices.push_back( DCEL::Vertex( v )); }
//            foreach ( DCEL::Face f,   source->f ) { faces.push_back( DCEL::Face( f )); }

//            std::cout << "source has     " << source->vn() << "vertices and " << source->fn() << "faces" << std::endl;
//            std::cout << "dest will have " << vertices.size() << "vertices and " << faces.size() << "faces" << std::endl;

//            Importer::from_vectors( destination, vertices, faces );
//        }
}
}

#endif
