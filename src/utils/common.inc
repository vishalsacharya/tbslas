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

namespace tbslas {

template <class Real_t>
std::vector<Real_t> point_distrib(DistribType dist_type, size_t N, MPI_Comm comm){
  int np, myrank;
  MPI_Comm_size(comm, &np);
  MPI_Comm_rank(comm, &myrank);
  std::vector<Real_t> coord;
  srand48(myrank+1);

  switch(dist_type){
  case UnifGrid:
    {
      size_t NN=(size_t)round(pow((double)N,1.0/3.0));
      size_t N_total=NN*NN*NN;
      size_t start= myrank   *N_total/np;
      size_t end  =(myrank+1)*N_total/np;
      for(size_t i=start;i<end;i++){
        coord.push_back(((Real_t)((i/  1    )%NN)+0.5)/NN);
        coord.push_back(((Real_t)((i/ NN    )%NN)+0.5)/NN);
        coord.push_back(((Real_t)((i/(NN*NN))%NN)+0.5)/NN);
      }
    }
    break;
  case RandUnif:
    {
      size_t N_total=N;
      size_t start= myrank   *N_total/np;
      size_t end  =(myrank+1)*N_total/np;
      size_t N_local=end-start;
      coord.resize(N_local*3);

      for(size_t i=0;i<N_local*3;i++)
        coord[i]=((Real_t)drand48());
    }
    break;
  case RandGaus:
    {
      Real_t e=2.7182818284590452;
      Real_t log_e=log(e);
      size_t N_total=N;
      size_t start= myrank   *N_total/np;
      size_t end  =(myrank+1)*N_total/np;

      for(size_t i=start*3;i<end*3;i++){
        Real_t y=-1;
        while(y<=0 || y>=1){
          Real_t r1=sqrt(-2*log(drand48())/log_e)*cos(2*M_PI*drand48());
          Real_t r2=pow(0.5,i*10/N_total);
          y=0.5+r1*r2;
        }
        coord.push_back(y);
      }
    }
    break;
  case RandElps:
    {
      size_t N_total=N;
      size_t start= myrank   *N_total/np;
      size_t end  =(myrank+1)*N_total/np;
      size_t N_local=end-start;
      coord.resize(N_local*3);

      const Real_t r=0.25;
      const Real_t center[3]={0.5,0.5,0.5};
      for(size_t i=0;i<N_local;i++){
        Real_t* y=&coord[i*3];
        Real_t phi=2*M_PI*drand48();
        Real_t theta=M_PI*drand48();
        y[0]=center[0]+0.25*r*sin(theta)*cos(phi);
        y[1]=center[1]+0.25*r*sin(theta)*sin(phi);
        y[2]=center[2]+r*cos(theta);
      }
    }
    break;
  case RandSphr:
    {
      size_t N_total=N;
      size_t start= myrank   *N_total/np;
      size_t end  =(myrank+1)*N_total/np;
      size_t N_local=end-start;
      coord.resize(N_local*3);

      const Real_t center[3]={0.5,0.5,0.5};
      for(size_t i=0;i<N_local;i++){
        Real_t* y=&coord[i*3];
        Real_t r=1;
        while(r>0.5 || r==0){
          y[0]=drand48(); y[1]=drand48(); y[2]=drand48();
          r=sqrt((y[0]-center[0])*(y[0]-center[0])
                +(y[1]-center[1])*(y[1]-center[1])
                +(y[2]-center[2])*(y[2]-center[2]));
          y[0]=center[0]+0.1*(y[0]-center[0])/r;
          y[1]=center[1]+0.1*(y[1]-center[1])/r;
          y[2]=center[2]+0.1*(y[2]-center[2])/r;
        }
      }
    }
    break;
  default:
    break;
  }
  return coord;
}


template <typename real_t, int sdim>
void
get_reg_grid_points(const size_t N,
                    real_t* out) {
  size_t tN = std::pow(N, sdim);
  real_t dx = 1.0/(N-1);
  // regular grid points positions
  for (size_t i = 0; i < tN; i++)
    for ( int idim = 0; idim < sdim; idim++) {
      size_t denom_shift = (size_t)std::pow(N, idim);
      out[i*sdim+idim] = (((real_t)((i / denom_shift) % N))*dx);
    }
}

template<typename real_t, int sdim>
void
get_vorticity_field(const real_t* points_pos,
                    int num_points,
                    real_t* points_values) {
  real_t omega = 1;
  real_t time_factor = 1;//+sin(2*3.14159*time);
  for (int i = 0; i < num_points; i++) {
    points_values[i*3+0] = omega*(0.5 - points_pos[i*sdim+1])*time_factor;
    points_values[i*3+1] = omega*(points_pos[i*sdim+0] - 0.5)*time_factor;
    points_values[i*3+2] = 0;
  }
}

template<typename real_t, int sdim>
void
get_vel_field_hom(const real_t* points_pos,
                  int num_points,
                  real_t* points_values) {
  real_t omega = 0.1;
  real_t time_factor = 1;//+sin(2*3.14159*time);
  for (int i = 0; i < num_points; i++) {
    points_values[i*3+0] = -omega*time_factor;
    points_values[i*3+1] = 0;
    points_values[i*3+2] = 0;
  }
}

template<typename real_t, int sdim>
void
get_gaussian_field(const real_t* points_pos,
                   int num_points,
                   real_t* out) {
  const real_t xc      = 0.6;
  const real_t yc      = 0.5;
  const real_t theta   = 0.0;
  const real_t sigma_x = 0.06;
  const real_t sigma_y = 0.06;
  const real_t A       = 1.0;
  real_t cos_theta   = cos(theta);
  real_t cos_2theta  = cos(2*theta);
  real_t cos_theta_2 = cos_theta * cos_theta;
  real_t sin_theta   = sin(theta);
  real_t sin_2theta  = sin(2*theta);
  real_t sin_theta_2 = sin_theta * sin_theta;
  real_t sigma_x_2   = sigma_x*sigma_x;
  real_t sigma_y_2   = sigma_y*sigma_y;

  real_t a = cos_theta_2*0.5/sigma_x_2 + sin_theta_2*0.5/sigma_y_2;
  real_t b = -sin_2theta*0.5/sigma_x_2 + sin_2theta*0.5/sigma_y_2;
  real_t c = sin_theta_2*0.5/sigma_x_2 + cos_theta_2*0.5/sigma_y_2;

  for (int i = 0; i < num_points; i++) {
    out[i]  = A*exp(-(a * (points_pos[i*sdim]-xc)   * (points_pos[i*sdim]-xc)   +
                      b * (points_pos[i*sdim]-xc)   * (points_pos[i*sdim+1]-yc) +
                      c * (points_pos[i*sdim+1]-yc) * (points_pos[i*sdim+1]-yc)
                      )
                    );
  }
};

template<typename real_t, int sdim>
void
get_gaussian_field_yext(const real_t* points_pos,
            int num_points,
            real_t* out) {
  const real_t xc      = 0.8;
  const real_t yc      = 0.5;
  const real_t theta   = 0.0;
  const real_t sigma_x = 0.06;
  const real_t sigma_y = 0.4;
  const real_t A       = 1.0;
  const real_t bmargin = 0.1;
  real_t cos_theta   = cos(theta);
  real_t cos_2theta  = cos(2*theta);
  real_t cos_theta_2 = cos_theta * cos_theta;
  real_t sin_theta   = sin(theta);
  real_t sin_2theta  = sin(2*theta);
  real_t sin_theta_2 = sin_theta * sin_theta;
  real_t sigma_x_2   = sigma_x*sigma_x;
  real_t sigma_y_2   = sigma_y*sigma_y;

  real_t a = cos_theta_2*0.5/sigma_x_2 + sin_theta_2*0.5/sigma_y_2;
  real_t b = -sin_2theta*0.5/sigma_x_2 + sin_2theta*0.5/sigma_y_2;
  real_t c = sin_theta_2*0.5/sigma_x_2 + cos_theta_2*0.5/sigma_y_2;

  for (int i = 0; i < num_points; i++) {
    if (points_pos[i*sdim+2] > bmargin && points_pos[i*sdim+2] < (1-bmargin) &&
        points_pos[i*sdim+1] > bmargin && points_pos[i*sdim+1] < (1-bmargin)) {
      out[i]  = A*exp(-(a * (points_pos[i*sdim]-xc)   * (points_pos[i*sdim]-xc)   +
                        b * (points_pos[i*sdim]-xc)   * (points_pos[i*sdim+1]-yc) +
                        c * (points_pos[i*sdim+1]-yc) * (points_pos[i*sdim+1]-yc)
                        ));
    } else {
      out[i] = 0;
    }
  }
};

template<typename real_t, int sdim>
void
get_gaussian_field_1d(const real_t* points_pos,
                      int num_points,
                      real_t* out) {
  const real_t a = 1.0;
  const real_t b = 0.9;
  const real_t c = 0.1;
  const real_t d = 0.0;

  real_t tmp;
  for (int i = 0; i < num_points; i++) {
    out[i] = a*exp(-((points_pos[i*sdim+0] - b)*(points_pos[i*sdim+0] - b) )/(2*c*c)) + d;
    if((points_pos[i*sdim+0] - b > 0))
      out[i] = 0;
  }
};

template<typename real_t, int sdim>
void
get_gaussian_field_3d(const real_t* points_pos,
                      int num_points,
                      real_t* out) {
  const real_t xc      = 0.8;
  const real_t yc      = 0.5;
  const real_t zc      = 0.5;
  const real_t A       = 1.0;
  const real_t sigma_x = 0.06;
  const real_t sigma_y = 0.35;
  const real_t sigma_z = 0.35;

  for (int i = 0; i < num_points; i++) {
    out[i]  = A*exp(-(((points_pos[i*sdim+0]-xc) * (points_pos[i*sdim+0]-xc))/(2*sigma_x*sigma_x) +
                      ((points_pos[i*sdim+1]-yc) * (points_pos[i*sdim+1]-yc))/(2*sigma_y*sigma_y) +
                      ((points_pos[i*sdim+2]-zc) * (points_pos[i*sdim+2]-zc))/(2*sigma_z*sigma_z)
                     )
                    );
  }
};

template <typename real_t, int dim>
std::vector<real_t>
generate_reg_grid_points(const size_t N) {
  size_t tN = std::pow(N, dim);
  real_t dx = 1.0/(N-1);
  // regular grid points positions
  std::vector<real_t> reg_grid_points_pos(tN*dim);
  for ( int idim = 0; idim < dim; idim++) {
    // idim's cooridnate
    int index_shift = idim*tN;
    size_t denom_shift = (size_t)std::pow(N, idim);
    for (size_t i = 0; i < tN; i++)
      reg_grid_points_pos[i+index_shift] = (((real_t)((i / denom_shift) % N))*dx);
  }
  return reg_grid_points_pos;
}

template<typename real_t>
std::vector<real_t>
generate_vorticity_field(const std::vector<real_t>& points_pos,
                         const real_t time ,
                         const real_t omega
                         ) {
  real_t time_factor = 1+sin(2*3.14159*time);
  size_t tN = points_pos.size()/3;
  std::vector<real_t> points_values(3*tN);
  for (size_t i = 0; i < tN; i++) {
    points_values[i]      = omega*(0.5           - points_pos[i+tN])*time_factor;
    points_values[i+tN]   = omega*(points_pos[i] - 0.5             )*time_factor;
    points_values[i+2*tN] = 0;
  }
  return points_values;
}

template<typename real_t>
std::vector<real_t>
generate_gaussian_field(const std::vector<real_t>& points_pos,
                        const real_t xc      ,
                        const real_t yc      ,
                        const real_t theta   ,
                        const real_t sigma_x ,
                        const real_t sigma_y) {
  real_t A           = 1.0;
  real_t cos_theta   = cos(theta);
  real_t cos_2theta  = cos(2*theta);
  real_t cos_theta_2 = cos_theta * cos_theta;
  real_t sin_theta   = sin(theta);
  real_t sin_2theta  = sin(2*theta);
  real_t sin_theta_2 = sin_theta * sin_theta;
  real_t sigma_x_2   = sigma_x*sigma_x;
  real_t sigma_y_2   = sigma_y*sigma_y;

  real_t a = cos_theta_2*0.5/sigma_x_2 + sin_theta_2*0.5/sigma_y_2;
  real_t b = -sin_2theta*0.5/sigma_x_2 + sin_2theta*0.5/sigma_y_2;
  real_t c = sin_theta_2*0.5/sigma_x_2 + cos_theta_2*0.5/sigma_y_2;

  size_t tN = points_pos.size()/3;
  std::vector<real_t> points_values(tN);
  for (size_t i = 0; i < tN; i++) {
    points_values[i]  = A*exp(-(a * (points_pos[i]-xc)    * (points_pos[i]-xc)    +
                                b * (points_pos[i]-xc)    * (points_pos[i+tN]-yc) +
                                c * (points_pos[i+tN]-yc) * (points_pos[i+tN]-yc)
                                )
                              );
  }
  return points_values;
}

template<typename real_t>
inline int
find_grid_index_1d(const std::vector<real_t>& grid,
                   const real_t query) {
  int num_el = grid.size();
  assert(num_el > 2);
  // assume grid is sorted
  real_t init  = grid[0];
  real_t final = grid[num_el-1];
  // assume grid points are equidistant
  real_t spacing = grid[1] - grid[0];
  assert(query >= init);
  assert(query <= final);
  int index = (int)std::floor((query - init) / spacing);
  assert(index >= 0);
  assert(index < num_el);
  return index;
}

}  // namepace tbslas