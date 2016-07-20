/************************************************************************/
/*                                                                      */
/*                 Copyright 2011 by Ullrich Koethe                     */
/*                                                                      */
/*    This file is part of the VIGRA computer vision library.           */
/*    The VIGRA Website is                                              */
/*        http://hci.iwr.uni-heidelberg.de/vigra/                       */
/*    Please direct questions, bug reports, and contributions to        */
/*        ullrich.koethe@iwr.uni-heidelberg.de    or                    */
/*        vigra@informatik.uni-hamburg.de                               */
/*                                                                      */
/*    Permission is hereby granted, free of charge, to any person       */
/*    obtaining a copy of this software and associated documentation    */
/*    files (the "Software"), to deal in the Software without           */
/*    restriction, including without limitation the rights to use,      */
/*    copy, modify, merge, publish, distribute, sublicense, and/or      */
/*    sell copies of the Software, and to permit persons to whom the    */
/*    Software is furnished to do so, subject to the following          */
/*    conditions:                                                       */
/*                                                                      */
/*    The above copyright notice and this permission notice shall be    */
/*    included in all copies or substantial portions of the             */
/*    Software.                                                         */
/*                                                                      */
/*    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND    */
/*    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES   */
/*    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND          */
/*    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT       */
/*    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,      */
/*    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING      */
/*    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR     */
/*    OTHER DEALINGS IN THE SOFTWARE.                                   */
/*                                                                      */
/************************************************************************/

#define PY_ARRAY_UNIQUE_SYMBOL vigranumpyilastiktools_PyArray_API
//#define NO_IMPORT_ARRAY

#include <utility>
#include <vector>
#include <unordered_map>

// Include this first to avoid name conflicts for boost::tie,
// similar to issue described in vigra#237
#include <boost/tuple/tuple.hpp>
#include <boost/functional/hash.hpp>

/*vigra*/
#include <ilastiktools/carving.hxx>


/*vigra python */
#include <vigra/numpy_array.hxx>
#include <vigra/numpy_array_converters.hxx>

// #include "export_graph_visitor.hxx"
// #include "export_graph_rag_visitor.hxx"
// #include "export_graph_algorithm_visitor.hxx"
// #include "export_graph_shortest_path_visitor.hxx"
// #include "export_graph_hierarchical_clustering_visitor.hxx"



namespace python = boost::python;

namespace vigra{



} // namespace vigra

using namespace vigra;
using namespace boost::python;


template<unsigned int DIM, class LABEL_TYPE>
void pyAssignLabels(
    GridRag<DIM, LABEL_TYPE> & graph,
    const NumpyArray<DIM, LABEL_TYPE> & labels
){
    graph.assignLabels(labels);
}

template<unsigned int DIM, class LABEL_TYPE>
void pyAssignLabelsFromSerialization(
    GridRag<DIM, LABEL_TYPE> & graph,
    const NumpyArray<DIM, LABEL_TYPE> & labels,
    NumpyArray<1, UInt32> serialization 
){
    graph.assignLabelsFromSerialization(labels, serialization);
}



template<unsigned int DIM, class LABEL_TYPE,
         class FEATURES_IN>
NumpyAnyArray pyAccumulateEdgeFeatures(
    GridRag<DIM, LABEL_TYPE> & graph,
    const NumpyArray<DIM, FEATURES_IN> & featuresIn,
    NumpyArray<1, typename NumericTraits<FEATURES_IN>::RealPromote > out 
){
    typedef TinyVector<MultiArrayIndex, 1>  Shape1;
    Shape1 shape(graph.edgeNum());
    out.reshapeIfEmpty(shape);
    graph.accumulateEdgeFeatures(featuresIn, out);
    return out;
}

template<unsigned int DIM, class LABEL_TYPE>
void pyPreprocessing(
    GridSegmentor<DIM , LABEL_TYPE, float> & gridSegmentor,
    const NumpyArray<DIM, LABEL_TYPE> & labels,
    const NumpyArray<DIM, float> & weightArray      
){
    gridSegmentor.preprocessing(labels, weightArray);
}


template<unsigned int DIM, class LABEL_TYPE>
void pyPreprocessingFromSerialization(
    GridSegmentor<DIM , LABEL_TYPE, float> & gridSegmentor,
    const NumpyArray<DIM, LABEL_TYPE> & labels,
    const NumpyArray<1, LABEL_TYPE> & serialization,
    const NumpyArray<1, float> & edgeWeights,
    const NumpyArray<1, UInt8> & nodeSeeds,
    const NumpyArray<1, UInt8> & resultSegmentation
){
    gridSegmentor.preprocessingFromSerialization(labels, serialization,
                                                 edgeWeights, nodeSeeds,
                                                 resultSegmentation);
}







template<unsigned int DIM, class LABEL_TYPE>
void pyAddLabels(
    GridSegmentor<DIM , LABEL_TYPE, float> & gridSegmentor,
    const NumpyArray<DIM, UInt8> & brushStroke,
    const TinyVector<MultiArrayIndex, DIM> roiBegin,
    const TinyVector<MultiArrayIndex, DIM> roiEnd,
    const UInt8 maxValidLabel
){
    gridSegmentor.addLabels(brushStroke, roiBegin, roiEnd, maxValidLabel);;
}

template<unsigned int DIM, class LABEL_TYPE>
NumpyAnyArray pyGetSegmentation(
    const GridSegmentor<DIM , LABEL_TYPE, float> & gridSegmentor,
    const TinyVector<MultiArrayIndex, DIM> roiBegin,
    const TinyVector<MultiArrayIndex, DIM> roiEnd,
    NumpyArray<DIM, UInt8>  segmentation
){
    typedef TinyVector<MultiArrayIndex, DIM>  ShapeN;
    ShapeN shape(roiEnd-roiBegin);
    segmentation.reshapeIfEmpty(shape);

    {
        PyAllowThreads _pythread;
        gridSegmentor.getSegmentation(roiBegin, roiEnd, segmentation);
    }
   
    return segmentation;
}


template<unsigned int DIM, class LABEL_TYPE>
NumpyAnyArray pyGetSuperVoxelSeg(
    const GridSegmentor<DIM , LABEL_TYPE, float> & gridSegmentor,
    NumpyArray<1, UInt8>  segmentation
){
    typedef TinyVector<MultiArrayIndex, 1>  Shape1;
    Shape1 shape(gridSegmentor.maxNodeId()+1);
    segmentation.reshapeIfEmpty(shape);

    {
        PyAllowThreads _pythread;
        gridSegmentor.getSuperVoxelSeg(segmentation);
    }
   
    return segmentation;
}

template<unsigned int DIM, class LABEL_TYPE>
NumpyAnyArray pyGetSuperVoxelSeeds(
    const GridSegmentor<DIM , LABEL_TYPE, float> & gridSegmentor,
    NumpyArray<1, UInt8>  seeds
){
    typedef TinyVector<MultiArrayIndex, 1>  Shape1;
    Shape1 shape(gridSegmentor.maxNodeId()+1);
    seeds.reshapeIfEmpty(shape);

    {
        PyAllowThreads _pythread;
        gridSegmentor.getSuperVoxelSeeds(seeds);
    }
   
    return seeds;
}





template<unsigned int DIM, class LABEL_TYPE>
NumpyAnyArray pySerializeGraph(
    const GridSegmentor<DIM , LABEL_TYPE, float> & gridSegmentor,
    NumpyArray<1, UInt32> serialization 
){
    serialization.reshapeIfEmpty( NumpyArray<1, UInt32>::difference_type(gridSegmentor.graph().serializationSize()));
    gridSegmentor.graph().serialize(serialization.begin());
    return serialization;
}

template<unsigned int DIM, class LABEL_TYPE>
void pyDeserializeGraph(
    GridSegmentor<DIM , LABEL_TYPE, float> & gridSegmentor,
    const NumpyArray<1, UInt32> & serialization 
){
    gridSegmentor.graph().clear();
    gridSegmentor.graph().deserialize(serialization.begin(),serialization.end());
}




template<unsigned int DIM, class LABEL_TYPE>
NumpyAnyArray pyEdgeWeights(
    const GridSegmentor<DIM , LABEL_TYPE, float> & gridSegmentor,
    NumpyArray<1, float> out
){
    out.reshapeIfEmpty( 
        NumpyArray<1, UInt32>::difference_type(gridSegmentor.edgeNum())
    );
    out = gridSegmentor.edgeWeights();
    return out;
}

template<unsigned int DIM, class LABEL_TYPE>
NumpyAnyArray pyNodeSeeds(
    const GridSegmentor<DIM , LABEL_TYPE, float> & gridSegmentor,
    NumpyArray<1, UInt8> out
){
    out.reshapeIfEmpty( 
        NumpyArray<1, UInt32>::difference_type(gridSegmentor.maxNodeId()+1)
    );
    out = gridSegmentor.nodeSeeds();
    return out;
}

template<unsigned int DIM, class LABEL_TYPE>
NumpyAnyArray pyGetResultSegmentation(
    const GridSegmentor<DIM , LABEL_TYPE, float> & gridSegmentor,
    NumpyArray<1, UInt8> out
){
    out.reshapeIfEmpty( 
        NumpyArray<1, UInt32>::difference_type(gridSegmentor.maxNodeId()+1)
    );
    out = gridSegmentor.resultSegmentation();
    return out;
}



template<unsigned int DIM, class LABEL_TYPE>
void pySetSeeds(
    GridSegmentor<DIM , LABEL_TYPE, float> & gridSegmentor,
    const NumpyArray<2, Int64> & fgSeeds,
    const NumpyArray<2, Int64> & bgSeeds
){
    gridSegmentor.setSeeds(fgSeeds, bgSeeds);
}

template<unsigned int DIM, class LABEL_TYPE>
void pySetResulFgObj(
    GridSegmentor<DIM , LABEL_TYPE, float> & gridSegmentor,
    const NumpyArray<1, Int64> & fgNodes
){
    gridSegmentor.setResulFgObj(fgNodes);
}

template<unsigned int DIM, class LABEL_TYPE>
void defineGridRag(const std::string & clsName){


    typedef GridRag<DIM, LABEL_TYPE> Graph;

    python::class_<Graph>(clsName.c_str(),python::init<  >())
        //.def("__init__",python::make_constructor(&pyGridGraphFactory3d<DIM,boost_graph::undirected_tag>))
        //.def(LemonUndirectedGraphCoreVisitor<Graph>(clsName))
        //.def(LemonGraphAlgorithmVisitor<Graph>(clsName))
        // my functions
        .def("assignLabels",registerConverters(&pyAssignLabels<DIM, LABEL_TYPE>))
        .def("accumulateEdgeFeatures", 
            registerConverters(&pyAccumulateEdgeFeatures<DIM, LABEL_TYPE, float>),
            (
                python::arg("features"),
                python::arg("out") = python::object()
            )
        )

    ;
}




template<unsigned int DIM, class LABEL_TYPE>
void defineGridSegmentor(const std::string & clsName){


    typedef GridSegmentor<DIM, LABEL_TYPE, float> Segmentor;

    python::class_<Segmentor>(clsName.c_str(),python::init<  >())
        .def("preprocessing", 
            registerConverters( & pyPreprocessing<DIM, LABEL_TYPE>),
            (
                python::arg("labels"),
                python::arg("weightArray")
            )
        )
        .def("preprocessingFromSerialization", 
            registerConverters( & pyPreprocessingFromSerialization<DIM, LABEL_TYPE>),
            (
                python::arg("labels"),
                python::arg("serialization"),
                python::arg("edgeWeights"),
                python::arg("nodeSeeds"),
                python::arg("resultSegmentation")
            )
        )
        .def("addSeeds", 
            registerConverters( & pyAddLabels<DIM, LABEL_TYPE>),
            (
                python::arg("brushStroke"),
                python::arg("roiBegin"),
                python::arg("roiEnd"),
                python::arg("maxValidLabel")
            )
        )
        .def("setSeeds", 
            registerConverters( & pySetSeeds<DIM, LABEL_TYPE>),
            (
                python::arg("fgSeeds"),
                python::arg("bgSeeds")
            )
        )
        .def("setResulFgObj", 
            registerConverters( & pySetResulFgObj<DIM, LABEL_TYPE>),
            (
                python::arg("fgNodes")
            )
        )

        .def("getSegmentation", 
            registerConverters( & pyGetSegmentation<DIM, LABEL_TYPE>),
            (
                python::arg("roiBegin"),
                python::arg("roiEnd"),
                python::arg("out") = python::object()
            )
        )
        .def("nodeNum",&Segmentor::nodeNum)
        .def("edgeNum",&Segmentor::edgeNum)
        .def("maxNodeId",&Segmentor::maxNodeId)
        .def("maxEdgeId",&Segmentor::maxEdgeId)
        .def("run",&Segmentor::run)
        .def("clearSeeds",&Segmentor::clearSeeds)
        .def("clearSegmentation",&Segmentor::clearSegmentation)
        .def("serializeGraph", registerConverters(&pySerializeGraph<DIM, LABEL_TYPE>),
            (
                python::arg("out") = python::object()
            )
        )
        .def("deserializeGraph", registerConverters(&pyDeserializeGraph<DIM, LABEL_TYPE>),
            (
                python::arg("serialization")
            )
        )
        .def("getEdgeWeights",registerConverters(pyEdgeWeights<DIM, LABEL_TYPE>),
            (
                python::arg("out") = python::object()
            )
        )
        .def("getNodeSeeds",registerConverters(pyNodeSeeds<DIM, LABEL_TYPE>),
            (
                python::arg("out") = python::object()
            )
        )
        .def("getResultSegmentation",registerConverters(pyGetResultSegmentation<DIM, LABEL_TYPE>),
            (
                python::arg("out") = python::object()
            )
        )
        .def("getSuperVoxelSeg",registerConverters(pyGetSuperVoxelSeg<DIM, LABEL_TYPE>),
            (
                python::arg("out") = python::object()
            )
        )
        .def("getSuperVoxelSeeds",registerConverters(pyGetSuperVoxelSeeds<DIM, LABEL_TYPE>),
            (
                python::arg("out") = python::object()
            )
        )
    ;
}

// Define lookup type:
// (u,v) -> [(x,y), (x,y),...]
typedef std::pair<UInt32, UInt32> edge_id_t;
typedef std::unordered_map<edge_id_t,
                           std::vector<Shape2>,
                           boost::hash<edge_id_t> > edge_coord_lookup_t;

typedef std::pair<edge_coord_lookup_t, edge_coord_lookup_t> edge_coord_lookup_pair_t;

edge_coord_lookup_pair_t edgeCoords2D( MultiArrayView<2, UInt32> const & src )
{
    using namespace vigra;
    MultiArrayIndex x_dim = src.shape(0);
    MultiArrayIndex y_dim = src.shape(1);

    edge_coord_lookup_t horizontal_edge_coords;
    edge_coord_lookup_t vertical_edge_coords;

    for (MultiArrayIndex x = 0; x < x_dim; ++x)
    {
        for (MultiArrayIndex y = 0; y < y_dim; ++y)
        {
            // Lambda to append to a lookup
            auto append_to_lookup =
            [&](edge_coord_lookup_t & lookup, MultiArrayIndex x1, MultiArrayIndex y1)
            {
                auto u = src(x,y);
                auto v = src(x1,y1);
                edge_id_t edge_id = (u < v) ? std::make_pair(u,v) : std::make_pair(v,u);

                auto iter = lookup.find(edge_id);
                if ( iter == lookup.end() )
                {
                    // Edge not yet seen. Create a new coord vector
                    auto coord_list = std::vector<Shape2>();
                    coord_list.push_back(Shape2(x,y));
                    lookup[edge_id] = coord_list;
                }
                else
                {
                    // Append to coord vector
                    auto & coord_list = iter->second;
                    coord_list.push_back(Shape2(x,y));
                }
            };

            // Check to the right
            if (x < x_dim-1 && src(x,y) != src(x+1,y))
            {
                append_to_lookup(horizontal_edge_coords, x+1, y);
            }

            // Check below
            if (y < y_dim-1 && src(x,y) != src(x,y+1))
            {
                append_to_lookup(vertical_edge_coords, x, y+1);
            }
        }
    }
    return std::make_pair(horizontal_edge_coords, vertical_edge_coords);
}

python::dict edgeCoordLookupToPython( edge_coord_lookup_t const & edge_coord_lookup )
{
    namespace py = boost::python;

    py::dict pylookup;
    for ( auto & edge_and_coords : edge_coord_lookup )
    {
        edge_id_t const & edge_id = edge_and_coords.first;
        std::vector<Shape2> const & coords = edge_and_coords.second;

        py::list pycoords;
        for ( auto coord : coords )
        {
            pycoords.append(py::make_tuple(coord[0], coord[1]));
        }
        pylookup[py::make_tuple(edge_id.first, edge_id.second)] = pycoords;
    }
    return pylookup;
}

python::tuple pythonEdgeCoords2D(NumpyArray<2, UInt32> const & src)
{
    namespace py = boost::python;
    auto lookup_pair = edgeCoords2D(src);
    py::dict pycoords_horizontal = edgeCoordLookupToPython(lookup_pair.first);
    py::dict pycoords_vertical = edgeCoordLookupToPython(lookup_pair.second);
    return py::make_tuple( pycoords_horizontal, pycoords_vertical );
}


BOOST_PYTHON_MODULE_INIT(_core)
{
    import_vigranumpy();

    python::docstring_options doc_options(true, true, false);


    defineGridRag<2, vigra::UInt32>("GridRag_2D_UInt32");
    defineGridSegmentor<2, vigra::UInt32>("GridSegmentor_2D_UInt32");


    defineGridRag<3, vigra::UInt32>("GridRag_3D_UInt32");
    defineGridSegmentor<3, vigra::UInt32>("GridSegmentor_3D_UInt32");

    using namespace boost::python;
    def("edgeCoords2D", registerConverters(&pythonEdgeCoords2D), (arg("src")));
}


