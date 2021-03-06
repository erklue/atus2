//
// ATUS2 - The ATUS2 package is atom interferometer Toolbox developed at ZARM
// (CENTER OF APPLIED SPACE TECHNOLOGY AND MICROGRAVITY), Germany. This project is
// founded by the DLR Agentur (Deutsche Luft und Raumfahrt Agentur). Grant numbers:
// 50WM0942, 50WM1042, 50WM1342.
// Copyright (C) 2017 Želimir Marojević, Ertan Göklü, Claus Lämmerzahl
//
// This file is part of ATUS2.
//
// ATUS2 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// ATUS2 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with ATUS2.  If not, see <http://www.gnu.org/licenses/>.
//

/**
 @author Želimir Marojević
 @brief realtime propagation
*/

#include <cstdio>
#include <cmath>

#include "cft_2d_MPI.h"
#include "cft_3d_MPI.h"
#include "muParser.h"
#include "ParameterHandler.h"
#include "CRT_Base_mpi.h"

using namespace std;

namespace MPI
{
  namespace RT_Solver
  {
    template<class T,int dim>
    class CRT_Propagation : public CRT_Base_mpi<T,dim,1>
    {
    public:
      CRT_Propagation( ParameterHandler * );
      virtual ~CRT_Propagation() {};

    protected:
      bool run_custom_sequence( const sequence_item &item );

      void Setup_Potential();

      using CRT_Base_mpi<T,dim,1>::m_Potential;
    };

    template<class T,int dim>
    CRT_Propagation<T,dim>::CRT_Propagation( ParameterHandler *p ) : CRT_Base_mpi<T,dim,1>( p )
    {
      this->Init_Potential();
    }

    template<class T, int dim>
    bool CRT_Propagation<T,dim>::run_custom_sequence( const sequence_item & /*item*/ )
    {
      return false;
    }

    template<class T,int dim>
    void CRT_Propagation<T,dim>::Setup_Potential()
    {
      #pragma omp parallel
      {
        CPoint<dim> x;
        mu::Parser loc_mup;

        try
        {
          this->m_params->Setup_muParser(loc_mup);
          loc_mup.SetExpr(this->m_params->Get_simulation("POTENTIAL"));
          loc_mup.DefineVar("x", &x[0]);

          switch (dim)
          {
          case 2:
            loc_mup.DefineVar("y", &x[1]);
            break;
          case 3:
            loc_mup.DefineVar("y", &x[1]);
            loc_mup.DefineVar("z", &x[2]);
            break;
          }
        }
        catch (mu::Parser::exception_type &e)
        {
          cout << "Message:  " << e.GetMsg() << "\n";
          cout << "Formula:  " << e.GetExpr() << "\n";
          cout << "Token:    " << e.GetToken() << "\n";
          cout << "Position: " << e.GetPos() << "\n";
          cout << "Errc:     " << e.GetCode() << "\n";
        }

        #pragma omp for
        for ( int l=0; l<this->m_no_of_pts; l++ )
        {
          x = this->m_fields[0]->Get_x(l);
          m_Potential[0][l] = loc_mup.Eval();
        }
      }
    }
  }
}


int main( int argc, char *argv[] )
{
  if ( argc != 2 )
  {
    printf( "No parameter xml file specified.\n" );
    return EXIT_FAILURE;
  }

  ParameterHandler params(argv[1]);
  int dim=0;

  try
  {
    std::string tmp = params.Get_simulation("DIM");
    dim = std::stod(tmp);
  }
  catch (mu::Parser::exception_type &e)
  {
    cout << "Message:  " << e.GetMsg() << "\n";
    cout << "Formula:  " << e.GetExpr() << "\n";
    cout << "Token:    " << e.GetToken() << "\n";
    cout << "Position: " << e.GetPos() << "\n";
    cout << "Errc:     " << e.GetCode() << "\n";
  }

  MPI_Init( &argc, &argv );
  fftw_mpi_init();

  try
  {
    if ( dim == 2 )
    {
      MPI::RT_Solver::CRT_Propagation<MPI::Fourier::cft_2d_MPI,2> rtsol( &params );
      rtsol.run_sequence();
    }
    else if ( dim == 3 )
    {
      MPI::RT_Solver::CRT_Propagation<MPI::Fourier::cft_3d_MPI,3> rtsol( &params );
      rtsol.run_sequence();
    }
    else
    {
      cout << "You have found a new dimension!" << endl;
    }
  }
  catch (mu::Parser::exception_type &e)
  {
    cout << "Message:  " << e.GetMsg() << "\n";
    cout << "Formula:  " << e.GetExpr() << "\n";
    cout << "Token:    " << e.GetToken() << "\n";
    cout << "Position: " << e.GetPos() << "\n";
    cout << "Errc:     " << e.GetCode() << "\n";
  }
  catch (std::string &str)
  {
    cout << str << endl;
  }

  //fftw_mpi_cleanup();
  MPI_Finalize();
  return EXIT_SUCCESS;
}
