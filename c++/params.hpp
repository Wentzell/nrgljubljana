// param.cc - Parameter parsing
// Copyright (C) 2009-2022 Rok Zitko

#ifndef _param_hpp_
#define _param_hpp_

#include <utility>
#include <list>
#include <string>
#include <stdexcept>
#include <cmath>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <fmt/format.h>

#include "misc.hpp" // contains, from_string, is_stdout_redirected, parsing code
#include "workdir.hpp"
#include "h5.hpp"

namespace NRG {

using namespace std::string_literals;

enum class RUNTYPE { NRG, DMNRG }; // First or second sweep? Used in class Step.

inline const auto fn_rho {"rho"s};
inline const auto fn_rhoFDM {"rhofdm"s};

// Base class for parameter containers
// XXX C++20: std::string may be constexpr
class parambase {
 protected:
   std::string keyword;
   std::string desc;
 public:
   parambase(const std::string &keyword, const std::string &desc) : keyword(keyword), desc(desc) {};
   virtual ~parambase() = default;
   virtual void set_str(const std::string &new_value) = 0;
   virtual void dump(std::ostream &F = std::cout) = 0;
   virtual void h5save(H5Easy::File &file, std::string path) = 0;
   [[nodiscard]] auto getkeyword() const noexcept { return keyword; }
   [[nodiscard]] auto getdesc() const noexcept { return desc; }
};

// Templated specialized classes for various storage types (int, double, string, bool)
template <typename T>
class param : public parambase {
 private:
   T data;
   bool is_default = true;
  public:
   // Constructor: keyword is a CASE SENSITIVE name of the parameter, desc is at this time used as in-line
   // documentation and default_value is a string containing a default value which is immediately parsed.
   // allparams is the list of all parameters.
   param(const std::string &keyword, const std::string &desc, const std::string &default_value, std::list<parambase*> &allparams) :
     parambase(keyword, desc) {
       data = from_string<T>(default_value);
       for (const auto &i : allparams)
         if (i->getkeyword() == keyword) throw std::runtime_error("param class internal error: keyword conflict.");
       allparams.push_back(this);
     }
   void dump(std::ostream &F = std::cout) override {
     F << keyword << "=" << data << (!is_default ? " *" : "") << std::endl;
   }
   void h5save(H5Easy::File &file, std::string path) override {
     h5_dump_scalar(file, path + "/" + keyword, data);
   }
   // This line enables to access parameters using an object as a rvalue
   [[nodiscard]] inline operator const T &() const noexcept { return data; }
   [[nodiscard]] inline T value() const noexcept { return data; }
   void set_str(const std::string &new_value) override { // used in parser
     data       = from_string<T>(new_value);
     is_default = false;
   }
   param & operator= (const T newdata) noexcept { data = newdata; return *this; }
   [[nodiscard]] bool operator== (const T &b) const noexcept { return data == b; }
};

// CONVENTION: parameters that are user configurable are declared as param<T>, other parameters (set at runtime) are
// defined as basic types T.

// Interfacing:
// S = solver, C = constructor, L = low-level, * = hide (deprecated & experimental)
// //! lines define parameters that are only used in the high-level interface

class Params {
 private:
  std::list<parambase *> all; // Container for all parameters

 public:
  std::unique_ptr<Workdir> workdir;

  param<std::string> symtype{"symtype", "Symmetry type", "", all}; // S

  // *************************************************************
  // Parameters controlling discretization scheme and Wilson chain

  param<double> Lambda{"Lambda", "Logarithmic discretization parameter", "2.0", all}; // S

  // Discretization scheme: Y)oshida-Whitaker-Oliveira, C)ampo-Oliveira, Z)itko-Pruschke
  param<std::string> discretization{"discretization", "Discretization scheme", "Z", all}; // N

  // Twist parameter for the mesh. See Yoshida, Whitaker, Oliveira PRB 41 9403 1990
  param<double> z{"z", "Parameter z in the logarithmic discretization", "1.0", all}; // N

  //! param<int> Nz {"Nz", "Number of discretization meshes", "1", all}; // S
  //! param<double> xmax {"xmax", "Largest x in discretization ODE solver", "20", all}; // S

  // Support of the hybridisation function, [-bandrescale:bandrescale]. The automatic rescaling is implemented as
  // follows: 1. The spectral function is rescaled on the x-axis by 1/bandrescale and on the y-axis by bandrescale.
  // 2. The Wilson chain coefficients are scaled by bandrescale. 3. In the data file, all eigenvalues are reduced by
  // the factor of bandrescale. 4. In the NRG code, the current scale is multiplied by bandrescale in function
  // SCALE(N).
  param<double> bandrescale{"bandrescale", "Band rescaling factor", "1.0", all}; // C

  // If polarized=true, the Wilson chain depends on the spin.
  param<bool> polarized{"polarized", "Spin-polarized Wilson chain", "false", all}; // N

  // If pol2x2=true for symtype=U1, the hybridisation function (and
  // thus Wilson chain) depends on two spin indexes, i.e., it is
  // described by a full 2x2 matrix in the spin space.
  param<bool> pol2x2{"pol2x2", "2x2 spin structure in Wilson chain", "false", all}; // N

  // Support for channel-mixing terms in the Wilson chain
  // (implemented for QS and QSZ symmetry types)
  param<bool> rungs{"rungs", "Channel-mixing terms in Wilson chain", "false", all}; // N

  // Length of the Wilson chain. The number of iterations ise determined either by Nmax or, if Tmin>0, by the lowest
  // temperature(=energy) scale still considered, Tmin. In other words, if Tmin>0, the parameter Nmax is recomputed
  // so that T at the Nmax-th iteration is equal or higher than the minimal temperature Tmin. NOTE: step.ndxN runs
  // from 0 to Nmax-1 (including), while the Wilson chain coefficient indexes run from 0 to Nmax (due to the
  // inclusion of the zero-th site of the Wilson chain).
  size_t Nmax = 0; // 0 means non-initialized.

  //! param<double> Tmin {"Tmin", "Lowest scale on the Wilson chain", "1e-4", all}; // S

  // If tri=cpp, we do the tridiagonalisation in the C++ part of the
  // code. In other cases,. we make use of external tools or Mathematica.
  param<std::string> tri{"tri", "Tridiagonalisation approach", "old", all};               // N
  param<size_t> preccpp{"preccpp", "Precision for tridiagonalisation", "2000", all}; // N

  // ************************
  // NRG iteration parameters

  param<std::string> diag{"diag", "Eigensolver routine (dsyev|dsyevd|dsyevr|zheev|zheevr|default)", "default", all}; // N

  // For partial diagonalisation routines (dsyevr, zheevr), diagratio controls the fraction
  // of eigenspectrum that we compute.
  param<double> diagratio{"diagratio", "Ratio of eigenstates computed in partial diagonalisation", "1.0", all}; // N

  // If an insufficient number of states is computed during an
  // iteration with partial diagonalisations, diagratio can be
  // increased by a factor of restartfactor and calculation restarted
  // if restart=true.
  param<bool> restart{"restart", "Restart calculation to achieve truncation goal?", "true", all}; // N
  param<double> restartfactor{"restartfactor", "Rescale factor for restart=true", "2.0", all};    // N

  // Truncation parameters. If keepenergy>0.0, then the cut-off
  // energy truncation scheme will be used. Parameter 'keep' is then
  // used to cap the maximum number of states. If 'keepmin' is set to
  // a positive value, it will ensure that a chosen minimal number of
  // states is always kept.
  param<size_t> keep{"keep", "Maximum number of states to keep at each step", "100", all};     // S
  param<double> keepenergy{"keepenergy", "Cut-off energy for truncation", "-1.0", all};        // S
  param<size_t> keepmin{"keepmin", "Minimum number of states to keep at each step", "0", all}; // S
  param<std::string> keepall{"keepall", "Keep all states at certain iterations", "", all};

  // Safeguard feature: keep more states in case of a near degeneracy
  // near the truncation cutoff. This prevents to truncate in the
  // middle of a (near-)degenerate cluster of states, leading to
  // unphysical effects. Disabled if set to 0, otherwise keeps states
  // that differ at most by P.safeguard.
  param<double> safeguard{"safeguard", "Additional states to keep in case of a near degeneracy", "1e-5", all}; // N
  param<size_t> safeguardmax{"safeguardmax", "Maximal number of additional states", "200", all};               // N

  // Fix artificial splitting of states due to floating point
  // round-off. Note that the use of this feature with large value of
  // epsilon may be dangerous. (The default conservative value of
  // 1e-15 cures differences in the 2-3 least significant bits.)
  param<double> fixeps{"fixeps", "Threshold value for eigenvalue splitting corrections", "1e-15", all}; // N

  // ******************************************************
  // Physical temperature for finite-temperature quantities
  param<double> T{"T", "Temperature, k_B T/D,", "0.001", all}; // S

  // \bar{\beta}, defines the effective temperature for computing the
  // thermodynamic quantities at iteration N. See Krishna-Murthy,
  // page 1009. The default value is 1.0, which is somewhat large,
  // but it is a safer default compared to 0.46 that requires large
  // truncation cutoff energy to obtain converged results.
  param<double> betabar{"betabar", "Parameter \bar{\beta} for thermodynamics", "1.0", all}; // N

  // *************************************************************
  // Parameters controlling the problem and quantities of interest

  //! param<std::string> problem {"problem", "Model considered (templated)", "SIAM", all}; // C

  // List of operators being considered
  param<std::string> ops{"ops", "Operators to be calculated", "", all}; // S

  // Dynamical quantities of interest
  param<std::string> specs{"specs", "Spectral functions (singlet ops) to compute", "", all};           // S
  param<std::string> specd{"specd", "Spectral functions (doublet ops) to compute", "", all};           // S
  param<std::string> spect{"spect", "Spectral functions (triplet ops) to compute", "", all};           // S
  param<std::string> specq{"specq", "Spectral functions (quadruplet ops) to compute", "", all};        // S
  param<std::string> specot{"specot", "Spectral functions (orbital triplet ops) to compute", "", all}; // S

  // Calculation of the temperature-dependent conductance G(T) &
  // first and second moment of A(w)(-df/dw), which are related to
  // the thermopower and the heat conductance.
  param<std::string> specgt{"specgt", "Conductance curves to compute", "", all};  // S
  param<std::string> speci1t{"speci1t", "I_1 curves to compute", "", all};        // S
  param<std::string> speci2t{"speci2t", "I_2 curves to compute", "", all};        // S
  param<double> gtp{"gtp", "Parameter p for G(T) calculations", "0.7", all}; // N

  // Calculation of the temperature-depenedent susceptibility chi(T)
  param<std::string> specchit{"specchit", "Susceptibilities to compute", "", all};      // S
  param<double> chitp{"chitp", "Parameter p for chi(T) calculations", "1.0", all}; // N

  // If chitp_ratio>0, chitp=chitp_ratio/betabar.
  param<double> chitp_ratio{"chitp_ratio", "Determine p from betabar", "-999", all}; // *

  // **************
  // NRG algorithms

  // Calculate finite-temperature spectral functions using Costi,
  // Hewson, Zlastic approach.
  param<bool> finite{"finite", "Perform Costi-Hewson-Zlatic finite-T calculation", "false", all}; // N

  // Perform DMNRG spectral function calculation. W. Hofstetter,
  // Phys. Rev. Lett. 85, 1508 (2000).
  param<bool> dmnrg{"dmnrg", "Perform DMNRG (density-matrix NRG) calculation", "false", all}; // S

  // Perform complete-Fock-space calculation. R. Peters, T. Pruschke,
  // F. B. Anders, Phys. Rev. B 74, 245113 (2006)
  param<bool> cfs{"cfs", "Perform CFS (complete Fock space) calculation", "false", all}; // S

  // Support for calculating greater and lesser correlation functions
  param<bool> cfsgt{"cfsgt", "CFS greater correlation function", "false", all}; // N
  param<bool> cfsls{"cfsls", "CFS lesser correlation function", "false", all};  // N

  // Perform full-density-matrix NRG calculation. Weichselbaum, J. von Delft, PRL 99 076402 (2007).
  param<bool> fdm{"fdm", "Perform FDM (full-density-matrix) calculation", "false", all}; // S

  // Support for calculating greater and lesser correlation functions
  param<bool> fdmgt{"fdmgt", "FDM greater correlation function?", "false", all}; // N
  param<bool> fdmls{"fdmls", "FDM lesser correlation function?", "false", all};  // N

  // Calculate the expectation value <O>(T) using the FDM algorithm.
  param<bool> fdmexpv{"fdmexpv", "Calculate expectation values using FDM", "false", all}; // S
  param<size_t> fdmexpvn{"fdmexpvn", "Iteration where we evaluate expv", "0", all};       // N

  // Dynamical quantity calculations on the Mastubara axis
  param<bool> finitemats{"finitemats", "T>0 calculation on Matsubara axis", "false", all}; // N
  param<bool> dmnrgmats{"dmnrgmats", "DMNRG calculation on Matsubara axis", "false", all}; // S
  param<bool> fdmmats{"fdmmats", "FDM calculation on Matsubara axis", "false", all};       // S
  param<size_t> mats{"mats", "Number of Matsubara points to collect", "100", all};         // S

  // If dm is set to true, density matrices are computed.
  // Automatically enabled when needed (DMNRG, CFS, FDM).
  param<bool> dm{"dm", "Compute density matrixes?", "false", all}; // N

  // **************
  // Frequency mesh
  param<double> broaden_max{"broaden_max", "Broadening mesh maximum frequency", "10", all};             // N
  param<double> broaden_min{"broaden_min", "Broadening mesh minimum frequency", "-99.", all};           // N
  param<double> broaden_min_ratio{"broaden_min_ratio", "Auto-tune broaden_min parameter", "3.0", all};  // N
  param<double> broaden_ratio{"broaden_ratio", "Common ration of the geometric sequence", "1.05", all}; // N

  //! param<double> mesh_max {"mesh_max", "Mesh maximum frequency", "10", all}; // C
  //! param<double> mesh_min {"mesh_min", "Mesh minimum frequency", "1e-4", all}; // C
  //! param<double> mesh_ratio {"mesh_ratio", "Common ratio of the geometric sequence", "1.05", all}; // C

  // Broadening parameters. cf. cond-mat/0607497
  param<double> alpha{"alpha", "Width of logarithmic gaussian", "0.3", all};           // S
  param<double> omega0{"omega0", "Smallest energy scale in the problem", "-1.0", all}; // N
  param<double> omega0_ratio{"omega0_ratio", "omega0 = omega0_ratio x T", "1.0", all}; // N
  //! param<double> gamma {"gamma", "Parameter for Gaussian convolution step", "0.2", all}; // S

  // ******************************************************
  // Parameters for fine-grained control of NRG calculation

  // Number of concurrent threads for matrix diagonalisation
  param<int> diagth{"diagth", "Diagonalisation threads", "1", all}; // N

  // Interleaved diagonalization
  param<bool> substeps{"substeps", "Interleaved diagonalization", "false", all}; // N

  // For "strategy=all", we recompute for the next iteration the
  // matrix elements corresponding to all eigenpairs computed in the
  // diagonalization step. For "strategy=kept", we only recompute
  // those matrix elements that are actually required in the next
  // iteration, i.e. those that correspond to the eigenpairs that we
  // keep after truncation. Note 1: using "strategy=kept" also
  // affects the calculation of thermodynamic and dynamic quantities
  // (in non-CFS algorithms), since the sum over all combinations of
  // states is correspondingly truncated. However, since due to the
  // gain in efficiency with "strategy=kept" we can increase the
  // number of states taken into account, this can be easily
  // compensated since the contributions are weighted with
  // exponentially decreasing Boltzmann factors. Note 2: for CFS/FDM
  // calculations, where all eigenpairs need to be recalculated, the
  // strategy is automatically switched to "all"!
  param<std::string> strategy{"strategy", "Recalculation strategy", "kept", all}; // N

  // It is possible to include more than the zero-th site in the
  // initial Wilson chain. This is controlled by parameter Ninit,
  // which is equal to the last f operator index still retained.
  // Ninit < Nmax.
  param<size_t> Ninit{"Ninit", "Initial Wilson chain ops", "0", all}; // N

  // Output real and imaginary parts of calculated correlators (specs).
  param<bool> reim{"reim", "Output imaginary parts of correlators?", "false", all}; // N

  // Number of eigenstates to save in "annotated.dat" per iteration.
  // Use dumpabs=true together with dumpscaled=false to trace total
  // energies of excitations.
  param<size_t> dumpannotated{"dumpannotated", "Number of eigenvalues to dump", "0", all};                      // N
  param<bool> dumpabs{"dumpabs", "Dump in terms of absolute energies", "false", all};                           // N
  param<bool> dumpscaled{"dumpscaled", "Dump using omega_N energy units", "true", all};                         // N
  param<size_t> dumpprecision{"dumpprecision", "Dump with # digits of precision", "8", all};                    // N
  param<bool> dumpgroups{"dumpgroups", "Dump by grouping degenerate states", "true", all};                      // N
  param<double> grouptol{"grouptol", "Energy tolerance for considering two states as degenerate", "1e-6", all}; // N

  // Dump diagonal matrix elements of singlet operators. This is
  // particularly useful in the presence of the gap, where the matrix
  // elements are different in the long-chain limit. "dumpdiagonal"
  // matrix elements are saved for each singlet operator. The output
  // goes to the standard output.
  param<size_t> dumpdiagonal{"dumpdiagonal", "Dump diagonal matrix elements", "0", all}; // N

  // ********************
  // Binning & broadening

  param<bool> savebins{"savebins", "Save binned (unbroadened) data", "false", all}; // N
  param<bool> broaden{"broaden", "Enable broadening of spectra", "true", all};      // N

  // Overrides for the binning interval limits
  param<double> emin{"emin", "Lower binning limit", "-1.0", all}; // N
  param<double> emax{"emax", "Upper binning limit", "-1.0", all}; // N

  // Number of bins per energy decade for accumulating spectral data
  param<size_t> bins{"bins", "bins/decade for spectral data", "1000", all}; // N

  // If P.accumulation != 0, the accumulation point of the
  // logarithmic mesh will be shifted away from omega=0. This may be
  // used in superconducting cases to get good spectral resolution
  // near the gap edges (cf. T. Hecht et al, JPCM).
  param<double> accumulation{"accumulation", "Shift of the accumulation points for binning", "0.0", all}; // N

  // If P.linstep != 0 (and P.accumulation != 0), a linear mesh
  // will be used in the interval [-accumulation:accumulation]. This
  // may be used to properly resolve the sub-gap peaks (Shiba bound
  // states or resonances).
  param<double> linstep{"linstep", "Bin width for linear mesh", "0", all}; // N

  // *************
  // Peak trimming

  // DISCARD_TRIM is relative value wrt "interval width" between
  // consecutive "bins". A "bin" is trimmed away if its weight is
  // less than (energy_bin(this)-energy_bin(next))*DISCARD_TRIM. This
  // is consistent with the fact that spectral density is spectral
  // weight per interval.
  param<double> discard_trim{"discard_trim", "Peak clipping at the end of the run", "1e-16", all}; // N

  // DISCARD_IMMEDIATELY is relative value wrt peak energy: we clip
  // if peak's weight is lower than energy*DISCARD_IMMEDIATELY.
  param<double> discard_immediately{"discard_immediately", "Peak clipping on the fly", "1e-16", all}; // N

  // ********
  // Patching

  // Parameter that controls the N/N+n patching schemes in non-FDM algorithms
  param<double> goodE{"goodE", "Energy window parameter for patching", "2.0", all}; // N

  // Do N/N+1 patching, rather than N/N+2 which is the default
  param<bool> NN1{"NN1", "Do N/N+1 patching?", "false", all}; // N

  // In N/N+2 patching we can use the information from even
  // iterations (NN2even=true, which is the default) or from odd
  // iterations (NN2even=false).
  param<bool> NN2even{"NN2even", "Use even iterations in N/N+2 patching", "true", all}; // N

  // NN2avg=true is equivalent to averaging results from a
  // NN2even=true and a NN2even=false calculations.
  param<bool> NN2avg{"NN2avg", "Average over even and odd N/N+2 spectra", "false", all}; // N

  // If NNtanh > 0, tanh window function is used instead of linear
  // (i.e. a smooth heap instead of a triangle).
  param<double> NNtanh{"NNtanh", "a in tanh[a(x-0.5)] window function", "0.0", all}; // N

  // **************************
  // Formatting of output files
  param<int> width_td{"width_td", "Widht of columns in 'td'", "16", all};               // N
  param<int> width_custom{"width_custom", "Width of columns in 'custom'", "16", all};   // N
  param<int> prec_td{"prec_td", "Precision of columns in 'td'", "10", all};             // N
  param<int> prec_custom{"prec_custom", "Precision of columns in 'custom'", "10", all}; // N
  param<int> prec_xy{"prec_xy", "Precision of spectral function output", "10", all};    // N

  // Checkpoint-restart functionality. Attempt to restart calculation
  // by reading the "unitary*" files from a previous calculation.
  // Automatically determines the number of files.
  param<bool> resume{"resume", "Attempt restart?", "false", all}; // N
  std::optional<size_t> laststored;                             // has value if stored data is found

  /* Fine-grained control over data logging with the following tokens:
   m - dump Hamiltonian matrix of each subspace [very verbose!]
   0 - functions calls in recalculations, etc. [high-level]
   f - follow recalc_f() [low-level]
   F - matrix elements in recalc_f() [very verbose!]
   r - follow recalc_general() [low-level]
   R - matrix elements in recalc_general() [very verbose!]
   g - follow calc_generic() [low-level]
   A - eigensolver diagnostics (routine used, matrix size)
   t - timing for eigensolver routines
   w - calculation of weights w_n
   M - MPI parallelization details
   ! - debug internal variables
   D - DMNRG calculation details
   @ - follow the program flow
   Z - report the values of different partition functions
   Useful combinations:
    @0 - high-level calculation flow
    fr - debug recalculation of irreducible matrix elements <||f||>
    ies - debug matrix construction and diagonalization
   OBSOLETE:
   i - iteration (subspaces list)
   e - dump eigenvalues in function diagonalize_h()
   s - dump ancestor subspaces during Hamiltonian matrix building
   H - details about storing unitary matrices to files
   c - details about spectral function calculation [high-level]
   d - eigenvalue computation [low-level]
   T - spectral peak trimming statistics
   * - merging details
   X - show energy clusters in find_clusters()
   $ - Hamiltonian matrix sorting diagnostics

  */
  param<std::string> logstr{"log", "list of tokens to define what to log", "", all}; // N
  param<bool> logall{"logall", "Log everything", "false", all};              // N

  // Returns true if option 'c' is selected for logging
  auto logletter(const char c) const { return logall ? true : contains(logstr, c); }

  // ********************************************
  // Parameters where overrides are rarely needed

  param<bool> done{"done", "Create DONE file?", "true", all};                         // N
  param<bool> calc0{"calc0", "Perform calculations at 0-th iteration?", "true", all}; // N

  // If only dmnrg is enabled, setting lastall=true will force "keeping" all states in the last step and the density
  // matrix will be initialized with all the states. Note that by default this feature is disabled, which is
  // especially appropriate for T->0 calculations.
  param<bool> lastall{"lastall", "Keep all states in the last iteratio for DMNRG", "false", all}; // N

  // If lastalloverride=true, then "lastall" is not set to true automatically for full-Fock-space (CFS and FDM)
  // approaches.
  param<bool> lastalloverride{"lastalloverride", "Override automatic lastall setting", "false", all}; // N

  // ******************************************************************
  // Parameters mostly relevant for low-level debugging and RG analysis

  // Store information about subspaces (total, calculated and kept states).
  param<bool> dumpsubspaces{"dumpsubspaces", "Save detailed subspace info", "false", all}; // N

  // Enable dumps of the matrix elements of <f> channel by channel,
  // subspace pair by subspace pair.
  param<bool> dump_f{"dump_f", "Dump <f> matrix elements", "false", all}; // N

  param<bool> dumpenergies{"dumpenergies", "Dump (all) energies to file?", "false", all};  // N
  param<bool> dumpabsenergies{"dumpabsenergies", "Dump (all) absolute energies to file?", "false", all};  // N - new

  // If set to false, the unitary transformation matrix and density
  // matrix files are kept after the calculation.
  param<bool> removefiles{"removefiles", "Remove temporary data files?", "true", all}; // N

  param<bool> checksumrules{"checksumrules", "Check operator sumrules", "false", all}; // N

  param<bool> absolute{"absolute", "Do NRG without any rescaling", "false", all};

  // Parallelization strategy: MPI, OpenMP or serial
  param<std::string> diag_mode{"diag_mode", "Parallelization strategy", "MPI", all};

  param<bool> h5raw{"h5raw", "Store raw data in an HDF5 file", "false", all};
  param<bool> h5all{"h5all", "Store raw data at all steps", "false", all};
  param<bool> h5last{"h5last", "Store raw data at last step", "true", all};
  param<bool> h5ham{"h5ham", "Store Hamiltonian matrices", "false", all};
  param<bool> h5ops{"h5ops", "Store operator matrices", "false", all};
  param<bool> h5vectors{"h5vectors", "Store eigenvectors", "false", all};
  param<bool> h5U{"h5U", "Store eigenvectors U", "false", all};
  param<bool> h5struct{"h5struct", "Store relations between subspaces", "false", all};

  param<std::string> project{"project", "Project the states before taking measurements", "", all};

  // **********************************
  // Backwards compatibility parameters
  param<bool> data_has_rescaled_energies{"data_has_rescaled_energies", "Rescaled eigenvalues?", "true", all};

  // **************************************************
  // Internal parameters, not under direct user control

  bool pretty_out;         // If true, produce color output using the {fmt} library.
  bool embedded;           // If true, the code is being called as a library from some application, not stand-alone.
  size_t channels = 0;     // Number of channels
  size_t coeffactor = 0;   // coefchannels = coeffactor * channels (typically coeffactor=1)
  size_t coefchannels = 0; // Number of coefficient sets (typically coefchannels=channels)
  size_t perchannel = 0;   // f-matrices per channel (typically 1)
  size_t combs = 0;        // dimension of new shell Hilbert space, 4 for single-channel, 16 for two-channel, etc.
  size_t Nlen = 0;         // Nlen=Nmax for regular calculations. Nlen=1 for ZBW. Length of wn, wnfactor, ZnD and dm vectors.

  [[nodiscard]] bool ZBW() const noexcept { return Nmax == Ninit; } // Zero-bandwidth calculation

  // Spin expressed in terms of the spin multiplicity, 2S+1. For SL & SL 3 symmetry types, P.spin is 1. Default value
  // of 2 is valid for all other symmetry types.
  size_t spin = 2;

  // Sets 'channels', then sets 'combs' and related parameters accordingly, depending on the spin of the conduction
  // band electrons. Called from read_data() in read-input.hpp
  void set_channels_and_combs(const int channels_) {
    my_assert(channels_ >= 1);
    channels = channels_;
    // Number of tables of coefficients. It is doubled in the case of spin-polarized conduction bands. The first half
    // corresponds to spin-up, the second half to spin-down. It is quadrupled in the case of full 2x2 matrix structure
    // in the spin space.
    if (pol2x2)
      coeffactor = 4;
    else if (polarized)
      coeffactor = 2;
    else
      coeffactor = 1;
    coefchannels = coeffactor * channels;
    if (symtype == "U1" || symtype == "SU2" || symtype == "DBLSU2") {
      perchannel = 2; // We distinguish spin-up and spin-down operators.
    } else if (symtype == "NONE" || symtype == "P" || symtype == "PP") {
      perchannel = 4; // We distinguish CR/AN and spin UP/DO.
    } else {
      perchannel = 1;
    }
    my_assert(perchannel >= 1);
    spin                     = symtype == "SL" || symtype == "SL3" ? 1 : 2;
    const auto statespersite = intpow(2, spin);
    combs                    = !substeps ? intpow(statespersite, channels) : statespersite;
    if (logletter('!'))
      fmt::print("coefchannels={} perchannel={} combs={}", coefchannels, perchannel, combs);
  }

  bool cfs_flags() const noexcept { return cfs || cfsgt || cfsls; }
  bool fdm_flags() const noexcept { return fdm || fdmgt || fdmls || fdmmats || fdmexpv; }
  bool dmnrg_flags() const noexcept { return dmnrg || dmnrgmats; }
  bool cfs_or_fdm_flags() const noexcept { return cfs_flags() || fdm_flags(); }
  bool dm_flags() const noexcept { return cfs_flags() || fdm_flags() || dmnrg_flags(); }
  bool keep_all_states_in_last_step() const noexcept { return lastall || (cfs_or_fdm_flags() && !lastalloverride); }
  bool need_rho() const noexcept { return cfs_flags() || dmnrg_flags(); }
  bool need_rhoFDM() const noexcept { return fdm_flags(); }
  bool do_recalc_kept(const RUNTYPE &runtype) const noexcept {   // kept: Recalculate using vectors kept after truncation
    return strategy == "kept" && !(cfs_or_fdm_flags() && runtype == RUNTYPE::DMNRG) && !ZBW(); 
  }
  bool do_recalc_all(const RUNTYPE &runtype) const noexcept {    // all: Recalculate using all vectors
    return !do_recalc_kept(runtype) && !ZBW();
  }
  bool do_recalc_none() const noexcept { return ZBW(); }

   // What is the last iteration completed in the previous NRG runs?
  void init_laststored() {
    if (resume) {
      laststored = std::nullopt;
      for (size_t N = Ninit; N < Nmax; N++) {
        const std::string fn = workdir->unitaryfn(N);
        std::ifstream F(fn);
        if (F.good())
          laststored = N;
      }
      if (laststored.has_value()) 
        std::cout << "Last unitary file found: " << laststored.value() << std::endl;
      else 
        std::cout << "No unitary files found." << std::endl;
    }
  }

  void validate() {
    my_assert(keep > 1);
    if (keepenergy > 0.0) my_assert(keepmin <= keep);
    if (dm_flags()) dm = true;
    my_assert(Lambda > 1.0);
    if (diag == "dsyevr"s || diag =="zheevr"s) {
      my_assert(0.0 < diagratio && diagratio <= 1.0);
      if (cfs_flags() && diagratio != 1.0) throw std::invalid_argument("CFS/FDM is not compatible with partial diagonalisation.");
    }
    my_assert(!(dumpabs && dumpscaled)); // dumpabs=true and dumpscaled=true is a meaningless combination
    // Take the first character (for backward compatibility)
    discretization = std::string(discretization, 0, 1);
    if (chitp_ratio > 0.0) chitp = chitp_ratio / betabar;
  }

  void dump(std::ostream &F = std::cout) {
    all.sort([](auto a, auto b) { return a->getkeyword() < b->getkeyword(); });
    F << std::setprecision(std::numeric_limits<double>::max_digits10); // ensure no precision is lost
    for (const auto &i : all) i->dump(F);
  }

   void h5save(H5Easy::File &file) const {
     for (const auto &i : all) i->h5save(file, "params");
     h5_dump_scalar(file, "params/Nmax", Nmax);
     h5_dump_scalar(file, "params/channels", channels);
     h5_dump_scalar(file, "params/coeffactor", coeffactor);
     h5_dump_scalar(file, "params/coefchannels", coefchannels);
     h5_dump_scalar(file, "params/perchannel", perchannel);
     h5_dump_scalar(file, "params/combs", combs);
     h5_dump_scalar(file, "params/Nlen", Nmax);
     h5_dump_scalar(file, "params/spin", spin);
   }

  explicit Params(const std::string &filename, const std::string &block,
         std::unique_ptr<Workdir> workdir_,
         const bool embedded,
         const bool quiet = false )
     : workdir(std::move(workdir_)), embedded(embedded)
  {
    pretty_out = !is_stdout_redirected();
    if (filename != "") {
      auto parsed_params = parser(filename, block);
      for (const auto &i : all) {
        const std::string keyword = i->getkeyword();
        if (parsed_params.count(keyword) == 1) {
          i->set_str(parsed_params[keyword]);
          parsed_params.erase(keyword);
        }
      }
      if (parsed_params.size()) {
        std::cout << "Unused settings: " << std::endl;
        for (const auto &[key, value] : parsed_params)
          std::cout << " " << key << "=" << value << std::endl;
        std::cout << std::endl;
      }
    }
    validate();
    init_laststored();
    if (!quiet) dump();
  }
  explicit Params() : Params("", "", std::make_unique<Workdir>(), true, true) {} // defaulted version (for testing purposes)
  Params(const Params &) = delete;
  Params(Params &&) = delete;
  Params &operator=(const Params &) = delete;
  Params &operator=(Params &&) = delete;

  // The factor that multiplies the eigenvalues of the length-N Wilson chain Hamiltonian in order to obtain the
  // energies on the original scale. Also named the "reduced bandwidth".
  // TO DO: use polymorphism instead of if statements.
  auto SCALE(const int N) const noexcept {
    double scale = 0.0;
    if (discretization == "Y"s)
      // Yoshida,Whitaker,Oliveira PRB 41 9403 Eq. (39)
      scale = 0.5 * (1. + 1. / Lambda); // NOLINT
    if (std::string(discretization) == "C"s || std::string(discretization) == "Z"s)
      // Campo, Oliveira PRB 72 104432, Eq. (46) [+ Lanczos]
      scale = (1.0 - 1. / Lambda) / std::log(Lambda); // NOLINT
    if (!substeps)
      scale *= pow(Lambda, -(N - 1) / 2. + 1 - z); // NOLINT
    else
      scale *= pow(Lambda, -N / (2. * double(channels)) + 3 / 2. - z); // NOLINT
    scale = scale * bandrescale; // RESCALE   // XXX: is this the appropriate place for rescaling? compatible with P.absolute==true?
    return scale;
  }

  // Energy scale at the last NRG iteration. Used in binning and broadening code.
  double last_step_scale() const noexcept { return SCALE(Nmax); }

  double nrg_step_scale_factor() const noexcept { // rescale factor in the RG transformation (matrix construction)
    return absolute ? 1 : (!substeps ? sqrt(Lambda) : pow(Lambda, 0.5/double(channels))); // NOLINT
  }

  // Here we set the lowest frequency at which we will evaluate the spectral density. If the value is not predefined
  // in the parameters file, use the smallest scale from the calculation multiplied by P.broaden_min_ratio.
  double get_broaden_min() const noexcept { return broaden_min <= 0.0 ? broaden_min_ratio * last_step_scale() : broaden_min; }

  // Energy scale factor that is exponentiated. N/N+1 patching is recommended for larger values of Lambda, so as to
  // reduce the upper limit of the patching window to omega_N*goodE*Lambda, i.e., by a factor of Lambda compared to
  // the N/N+2 patching where the upper limit is omega_N*goodE*Lambda^2.
  double getEfactor() const noexcept {
    return (channels == 1 && !NN1) ? Lambda : std::sqrt(Lambda);
  }

  // Energy window for the patching procedure [FT and DMNRG algorithms]. In ZBW calculations the window is extended to [0:infty]
  // to get all spectral contributions.
  double getE0()   const noexcept { return goodE; }
  double getEmin() const noexcept { return !ZBW() ? getE0() : 0.0; }
  double getEx()   const noexcept { return getE0() * getEfactor(); }   // The "peak" energy of the "window function" in the patching procedure.
  double getEmax() const noexcept { return !ZBW() ? getE0() * pow(getEfactor(),2) : std::numeric_limits<double>::max(); }

  auto Nall() const noexcept { return boost::irange(size_t(Ninit), size_t(Nlen)); }
};

class DiagParams {
 public:
   std::string diag{};
   double diagratio{};
   bool logall{};
   std::string logstr{};

   DiagParams() {}
   explicit DiagParams(const Params &P, const double diagratio_ = -1) :
     diag(P.diag), diagratio(diagratio_ > 0 ? diagratio_ : P.diagratio),
     logall(P.logall), logstr(P.logstr) {}
   bool logletter(char c) const { return logall ? true : logstr.find(c) != std::string::npos; }

 private:
   friend class boost::serialization::access;
   template <class Archive> void serialize(Archive &ar, [[maybe_unused]] const unsigned int version) {
      ar &diag;
      ar &diagratio;
      ar &logall;
      ar &logstr;
   }
};

} // namespace

#endif
