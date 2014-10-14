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

#ifndef SRC_TREE_NODE_FIELD_FUNCTOR_H_
#define SRC_TREE_NODE_FIELD_FUNCTOR_H_

#include <vector>

#include <pvfmm_common.hpp>
#include <cheb_node.hpp>

namespace tbslas {

template<typename real_t>
class NodeFieldFunctor {

  typedef pvfmm::Cheb_Node<real_t> Node_t;

 public:
  explicit NodeFieldFunctor(Node_t* node): node_(node) {
  }

  virtual ~NodeFieldFunctor() {
  }

  void operator () (const real_t* points_pos,
                    int num_points,
                    real_t* out) {
    // TODO: remove this part
    // std::vector<real_t> x_c,y_c,z_c;
    // for (int i = 0; i < num_points; i++) {
    //   x_c.push_back(points_pos[i*COORD_DIM+0]);
    //   y_c.push_back(points_pos[i*COORD_DIM+1]);
    //   z_c.push_back(points_pos[i*COORD_DIM+2]);
    // }
    // std::vector<int> outsiders = isOutside(node_, x_c, y_c, z_c);
    // assert(outsiders.empty());

    // TODO: optimize! you do not need to get the values of each point separately
    for (int i = 0; i < num_points; i++) {
      std::vector<real_t> x,y,z;
      x.push_back(points_pos[i*COORD_DIM+0]);
      y.push_back(points_pos[i*COORD_DIM+1]);
      z.push_back(points_pos[i*COORD_DIM+2]);
      node_->ReadVal(x, y, z, &out[i*node_->DataDOF()]);
    }
  }

 private:
  Node_t* node_;
};

}      // namespace tbslas
#endif  // SRC_TREE_NODE_FIELD_FUNCTOR_H_