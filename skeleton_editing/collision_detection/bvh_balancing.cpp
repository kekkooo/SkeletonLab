#ifdef use_gurobi
#include "bvh_balancing.h"

#include "gurobi_c++.h"

#include <sstream>

using namespace std;

size_t matIndex( size_t row, size_t col, size_t ncols ){
    return row * ncols + col;
}

void getResults( const std::map<int, int>& index_to_id, const std::vector<GRBVar>& vars,
                 std::map<int, int>& bone_assignments,
                 size_t no_bones, size_t no_branchings ){
    for( size_t r = 0; r < no_bones; ++r ){
        for( size_t c = 0; c < no_branchings; ++c ){
            size_t idx = matIndex( r, c, no_branchings );
            double v = vars[idx].get(GRB_DoubleAttr_X);
//            std::cout << v << "\t";
            if( v > 0.5 ) {
                std::cout << "Branch " << r << " has been assigned to Bn " << c << std::endl;
                bone_assignments[ r ] = index_to_id.at( c );
            }
        }
//        std::cout << std::endl;
    }
//    assert(false);
//    for( size_t r = 0; r < no_bones; ++r ){
//        for( size_t c = 0; c < no_branchings; ++c ){
//            size_t idx = matIndex( r, c, no_branchings );
//            double v = vars[idx].get(GRB_DoubleAttr_X);
//            std::cout << v << "\t";
////            if( v > 0.5 ) {
////                std::cout << "Branch " << r << " has been assigned to Bn " << c << std::endl;
////                bone_assignments[ index_to_id.at( c ) ] = r;
////            }
//        }
//        std::cout << std::endl;
//    }
}

void BVH_balancing::Solve( const std::map<int, std::set<int> > &compatibility, size_t no_bones,
                           std::map<int, int>& assignments ){
    std::map<int, int> Bn_to_index, index_to_Bn;
    size_t index = 0;
    size_t no_branchings 	= compatibility.size();

    assert( no_bones > 0 );
    assert( no_branchings > 0 );

    for( const auto& item : compatibility ){
        if( Bn_to_index.count(item.first) > 0 ) { continue; } // skip if alredy mapped
        Bn_to_index[item.first] = index;
        index_to_Bn[index] = item.first;
        ++index;
    }
    // entire optimization goes inside a try catch statement.
    // in case of errors, I want it to crash!
    try{
        GRBEnv env;
        GRBModel model = GRBModel( env );

        /* CREATE VARIABLES */
        std::vector<GRBVar> vars( no_bones * no_branchings );
        for( size_t r = 0; r < no_bones; ++r ){
            for( size_t c = 0; c < no_branchings; ++c ){
                size_t idx = matIndex( r, c, no_branchings );
                std::stringstream var_name;
                var_name << "A( " << r << ", " << c << " )";
                assert( compatibility.count( index_to_Bn[c] ) > 0 );
                bool is_connected = compatibility.at( index_to_Bn[c] ).count( r );
                vars[idx] = model.addVar( 0.0, ( is_connected > 0 ? 1.0 : 0.0 ), 1.0, GRB_BINARY, var_name.str());
            }
        }
        model.update();

        // Create Objective Function.
        // for each branching node i : #bones( Bn_i )^2
        // hence I need to sum the number of bones assigned to each branching node
        GRBQuadExpr obj;
        std::vector<GRBLinExpr> cols( no_branchings );

        for( size_t c = 0; c < no_branchings; ++c ){
            for( size_t r = 0; r < no_bones; ++r ){
                size_t idx = matIndex( r, c, no_branchings );
                cols[c] += vars[idx];
            }
        }
        for( size_t c = 0; c < no_branchings; ++c ){ obj += cols[c] * cols[c]; }

        model.setObjective( obj );
        model.update();

        // create constraint : each bone can be assigned to only one branching node.
        // this means that the summation of each row's values must be equal to 1.0
        for( size_t r = 0; r < no_bones; ++r ){
            GRBLinExpr row_sum;
            for( size_t c = 0; c < no_branchings; ++c ){
                size_t idx = matIndex( r, c, no_branchings );
                row_sum += vars[idx];
            }
            model.addConstr( row_sum == 1.0 );
        }
        model.update();

        // Optimize
        model.optimize();
        int status = model.get(GRB_IntAttr_Status);

        if (status == GRB_OPTIMAL) {
          std::cout << "The optimal objective is " << model.get(GRB_DoubleAttr_ObjVal) << std::endl;
          // return results!
//          printResults(vars, no_bones, no_branchings );
          getResults( index_to_Bn, vars, assignments, no_bones, no_branchings );
          return;
        }

        /************************************/
        /*          ERROR HANDLING          */
        /************************************/
        if (status == GRB_UNBOUNDED){
          std::cout << "The model cannot be solved because it is unbounded" << std::endl;
          assert(false);
        }

        if ((status != GRB_INF_OR_UNBD) && (status != GRB_INFEASIBLE)) {
          std::cout << "Optimization was stopped with status " << status << std::endl;
          assert( false );
        }
        GRBConstr* c = 0;
        // do IIS
        std::cout << "The model is infeasible; computing IIS" << std::endl;
        model.computeIIS();
        std::cout << "\nThe following constraint(s) cannot be satisfied:" << std::endl;
        c = model.getConstrs();
        for (int i = 0; i < model.get(GRB_IntAttr_NumConstrs); ++i){
          if (c[i].get(GRB_IntAttr_IISConstr) == 1) {
            std::cout << c[i].get(GRB_StringAttr_ConstrName) << std::endl;
          }
        }
    }
    /*          EXCEPTION HANDLING          */
    catch (GRBException e) {
        std::cout << "Error code = " << e.getErrorCode() << std::endl;
        std::cout << e.getMessage() << std::endl;
        assert( false );
    }
    catch (...){
        std::cout << "Exception during optimization" << std::endl;
        assert( false );
    }

}

#endif
