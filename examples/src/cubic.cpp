// *************************************************************************
// Copyright (C) 2014 by Arash Bakhtiari
// You may not use this file except in compliance with the License.
// You obtain a copy of the License in the LICENSE file.

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// *************************************************************************

#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <string>

#include <pvfmm_common.hpp>
#include <mpi_tree.hpp>
#include <cheb_node.hpp>
#include <utils.hpp>
#include <vector.hpp>
#include <cheb_utils.hpp>
#include <profile.hpp>

#include <utils/common.h>
#include <utils/metadata.h>

// #include <utils/profile.h>
#include <tree/semilag_tree.h>
#include <tree/utils_tree.h>

typedef pvfmm::Cheb_Node<double> Node_t;
typedef pvfmm::MPI_Tree<Node_t> Tree_t;

typedef tbslas::MetaData<std::string,
                         std::string,
                         std::string> MetaData_t;
double tcurr = 0;

const char* OUTPUT_FILE_FORMAT = "%s/%s-VAR_%s-TS_%04d-RNK";
const char* OUTPUT_FILE_PREFIX = "cubic";

int main (int argc, char **argv) {
  MPI_Init(&argc, &argv);
  MPI_Comm comm=MPI_COMM_WORLD;

  // commandline_option_end(argc, argv);
  parse_command_line_options(argc, argv);

  {
    int myrank;
    MPI_Comm_rank(comm, &myrank);
    // =========================================================================
    // SIMULATION PARAMETERS
    // =========================================================================
    tbslas::SimConfig* sim_config       = tbslas::SimConfigSingleton::Instance();
    sim_config->vtk_filename_format     = OUTPUT_FILE_FORMAT;
    sim_config->vtk_filename_prefix     = OUTPUT_FILE_PREFIX;
    sim_config->vtk_filename_variable   = "conc";
    // sim_config->use_cubic = true;
    // sim_config->cubic_use_analytical = true;
    pvfmm::Profile::Enable(sim_config->profile);
    // =========================================================================
    // PRINT METADATA
    // =========================================================================
    if (!myrank) {
      MetaData_t::Print();
    }
    // =========================================================================
    // CONSTRUCT TREE
    // =========================================================================
    tcurr = 0;
    Tree_t tree(comm);
    tbslas::ConstructTree<Tree_t>(sim_config->tree_num_point_sources,
                                  sim_config->tree_num_points_per_octanct,
                                  sim_config->tree_chebyshev_order,
                                  sim_config->tree_max_depth,
                                  sim_config->tree_adap,
                                  sim_config->tree_tolerance,
                                  comm,
                                  get_gaussian_field_cylinder_atT<double,3>,
                                  1,
                                  tree);
    // =========================================================================
    // COMPUTE ERROR
    // =========================================================================
    double rli, rl2;
    double ali, al2;
    CheckChebOutput<Tree_t>(&tree,
                            get_gaussian_field_cylinder_atT<double,3>,
                            1,
                            al2,rl2,ali,rli,
                            std::string("Output"));

    std::vector<double> grid_points;
    std::vector<double> node_pos = pvfmm::cheb_nodes<double>(sim_config->tree_chebyshev_order, 3);
    tbslas::CollectTreeGridPoints(tree,
                                  node_pos,
                                  grid_points);

    tbslas::ComputeTreeError(tree,
                             get_gaussian_field_cylinder_atT<double,3>,
                             grid_points,
                             ali,
                             al2);
    int num_leaves = tbslas::CountNumLeafNodes(tree);
    // =========================================================================
    // REPORT RESULTS
    // =========================================================================
    if(!myrank) {
      printf("#TBSLAS-HEADER: %-15s %-15s %-15s %-15s %-15s %-15s %-15s\n",
             "TOL",
             "CUBIC",
             "CUF",
             "ANAL",
             "AL2",
             "ALINF",
             "NOCT");
      printf("#TBSLAS-RESULT: %-15.5e %-15d %-15d %-15d %-15.5e %-15.5e %-15d\n",
             sim_config->tree_tolerance,
             sim_config->use_cubic,
             sim_config->cubic_upsampling_factor,
             sim_config->cubic_use_analytical,
             al2,
             ali,
             num_leaves
             );
    }
    //Output Profiling results.
    pvfmm::Profile::print(&comm);
  }

  // Shut down MPI
  MPI_Finalize();
  return 0;
}