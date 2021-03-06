#ifndef parameter_handling_h
#define parameter_handling_h

#include <deal.II/base/parameter_handler.h>

#include "../../adapter/precice_parameter.h"

namespace Nonlinear_Elasticity
{
  using namespace dealii;
  using namespace Parameters;
  /**
   * This class declares all parameters, which can be specified in the parameter
   * file. The subsection abut preCICE configurations is directly interlinked
   * to the Adapter class.
   */
  namespace Parameters
  {
    /**
     * @brief System: Specifies system properties
     */
    struct System
    {
      double               nu;
      double               mu;
      double               rho;
      Tensor<1, 3, double> body_force;

      static void
      declare_parameters(ParameterHandler &prm);

      void
      parse_parameters(ParameterHandler &prm);
    };

    void
    System::declare_parameters(ParameterHandler &prm)
    {
      prm.enter_subsection("System properties");
      {
        prm.declare_entry("Poisson's ratio",
                          "0.3",
                          Patterns::Double(-1.0, 0.5),
                          "Poisson's ratio");

        prm.declare_entry("Shear modulus",
                          "0.4225e6",
                          Patterns::Double(),
                          "Shear modulus");

        prm.declare_entry("rho", "1000", Patterns::Double(), "rho");

        prm.declare_entry("body forces",
                          "0,0,0",
                          Patterns::List(Patterns::Double()),
                          "body forces x,y,z");
      }
      prm.leave_subsection();
    }

    void
    System::parse_parameters(ParameterHandler &prm)
    {
      prm.enter_subsection("System properties");
      {
        nu  = prm.get_double("Poisson's ratio");
        mu  = prm.get_double("Shear modulus");
        rho = prm.get_double("rho");
        const std::vector<std::string> body_forces_input =
          Utilities::split_string_list(prm.get("body forces"));
        for (uint d = 0; d < 3; ++d)
          body_force[d] = Utilities::string_to_double(body_forces_input[d]);
      }
      prm.leave_subsection();
    }

    /**
     * @brief LinearSolver: Specifies linear solver properties
     */
    struct LinearSolver
    {
      std::string type_lin;
      double      tol_lin;
      double      max_iterations_lin;

      static void
      declare_parameters(ParameterHandler &prm);

      void
      parse_parameters(ParameterHandler &prm);
    };

    void
    LinearSolver::declare_parameters(ParameterHandler &prm)
    {
      prm.enter_subsection("Linear solver");
      {
        prm.declare_entry("Solver type",
                          "CG",
                          Patterns::Selection("CG|Direct"),
                          "Linear solver: CG or Direct");

        prm.declare_entry("Residual",
                          "1e-6",
                          Patterns::Double(0.0),
                          "Linear solver residual (scaled by residual norm)");

        prm.declare_entry(
          "Max iteration multiplier",
          "1",
          Patterns::Double(0.0),
          "Linear solver iterations (multiples of the system matrix size)");
      }
      prm.leave_subsection();
    }

    void
    LinearSolver::parse_parameters(ParameterHandler &prm)
    {
      prm.enter_subsection("Linear solver");
      {
        type_lin           = prm.get("Solver type");
        tol_lin            = prm.get_double("Residual");
        max_iterations_lin = prm.get_double("Max iteration multiplier");
      }
      prm.leave_subsection();
    }

    /**
     * @brief NonlinearSolver: Specifies nonlinear solver properties
     */
    struct NonlinearSolver
    {
      unsigned int max_iterations_NR;
      double       tol_f;
      double       tol_u;

      static void
      declare_parameters(ParameterHandler &prm);

      void
      parse_parameters(ParameterHandler &prm);
    };

    void
    NonlinearSolver::declare_parameters(ParameterHandler &prm)
    {
      prm.enter_subsection("Nonlinear solver");
      {
        prm.declare_entry("Max iterations Newton-Raphson",
                          "10",
                          Patterns::Integer(0),
                          "Number of Newton-Raphson iterations allowed");

        prm.declare_entry("Tolerance force",
                          "1.0e-9",
                          Patterns::Double(0.0),
                          "Force residual tolerance");

        prm.declare_entry("Tolerance displacement",
                          "1.0e-6",
                          Patterns::Double(0.0),
                          "Displacement error tolerance");
      }
      prm.leave_subsection();
    }

    void
    NonlinearSolver::parse_parameters(ParameterHandler &prm)
    {
      prm.enter_subsection("Nonlinear solver");
      {
        max_iterations_NR = prm.get_integer("Max iterations Newton-Raphson");
        tol_f             = prm.get_double("Tolerance force");
        tol_u             = prm.get_double("Tolerance displacement");
      }
      prm.leave_subsection();
    }

    /**
     * @brief Time: Specifies simulation time properties
     */
    struct Time
    {
      double delta_t;
      double end_time;
      int    output_interval;

      static void
      declare_parameters(ParameterHandler &prm);

      void
      parse_parameters(ParameterHandler &prm);
    };

    void
    Time::declare_parameters(ParameterHandler &prm)
    {
      prm.enter_subsection("Time");
      {
        prm.declare_entry("End time", "1", Patterns::Double(), "End time");

        prm.declare_entry("Time step size",
                          "0.1",
                          Patterns::Double(),
                          "Time step size");

        prm.declare_entry("Output interval",
                          "1",
                          Patterns::Integer(),
                          "Output interval");
      }
      prm.leave_subsection();
    }

    void
    Time::parse_parameters(ParameterHandler &prm)
    {
      prm.enter_subsection("Time");
      {
        end_time        = prm.get_double("End time");
        delta_t         = prm.get_double("Time step size");
        output_interval = prm.get_integer("Output interval");
      }
      prm.leave_subsection();
    }

    /**
     * @brief Discretization: Specifies parameters for time integration by an
     *        implicit Newmark scheme and polynomial degree of the FE system
     */
    struct Discretization
    {
      double       beta;
      double       gamma;
      unsigned int poly_degree;


      static void
      declare_parameters(ParameterHandler &prm);

      void
      parse_parameters(ParameterHandler &prm);
    };

    void
    Discretization::declare_parameters(ParameterHandler &prm)
    {
      prm.enter_subsection("Discretization");
      {
        prm.declare_entry("beta",
                          "0.25",
                          Patterns::Double(0, 0.5),
                          "Newmark beta");

        prm.declare_entry("gamma",
                          "0.5",
                          Patterns::Double(0, 1),
                          "Newmark gamma");

        prm.declare_entry("Polynomial degree",
                          "3",
                          Patterns::Integer(0),
                          "Polynomial degree of the FE system");
      }
      prm.leave_subsection();
    }

    void
    Discretization::parse_parameters(ParameterHandler &prm)
    {
      prm.enter_subsection("Discretization");
      {
        beta        = prm.get_double("beta");
        gamma       = prm.get_double("gamma");
        poly_degree = prm.get_integer("Polynomial degree");
      }
      prm.leave_subsection();
    }



    struct AllParameters : public System,
                           public LinearSolver,
                           public NonlinearSolver,
                           public Time,
                           public Discretization,
                           public PreciceAdapterConfiguration

    {
      AllParameters(const std::string &input_file);

      static void
      declare_parameters(ParameterHandler &prm);

      void
      parse_parameters(ParameterHandler &prm);
    };

    AllParameters::AllParameters(const std::string &input_file)
    {
      ParameterHandler prm;
      declare_parameters(prm);
      prm.parse_input(input_file);
      parse_parameters(prm);

      // Optional, if we want to print all parameters in the beginning of the
      // simulation
      //      prm.print_parameters(std::cout,ParameterHandler::Text);
    }

    void
    AllParameters::declare_parameters(ParameterHandler &prm)
    {
      System::declare_parameters(prm);
      LinearSolver::declare_parameters(prm);
      NonlinearSolver::declare_parameters(prm);
      Time::declare_parameters(prm);
      Discretization::declare_parameters(prm);
      PreciceAdapterConfiguration::declare_parameters(prm);
    }

    void
    AllParameters::parse_parameters(ParameterHandler &prm)
    {
      System::parse_parameters(prm);
      LinearSolver::parse_parameters(prm);
      NonlinearSolver::parse_parameters(prm);
      Time::parse_parameters(prm);
      Discretization::parse_parameters(prm);
      PreciceAdapterConfiguration::parse_parameters(prm);
    }
  } // namespace Parameters
} // namespace Nonlinear_Elasticity

#endif
