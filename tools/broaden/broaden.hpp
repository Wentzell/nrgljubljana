// broaden - Finite-temperature raw spectral data broadening tool
// Ljubljana code Rok Zitko, rok.zitko@ijs.si, 2012-2025

#ifndef _broaden_broaden_hpp_
#define _broaden_broaden_hpp_

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <cstdlib>
#include <cassert>
#include <cfloat>
#include <utility>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <numeric>

#include <unistd.h>
#include <getopt.h>

#include <range/v3/all.hpp>

#include "misc.hpp"
#include "basicio.hpp"

namespace NRG::Broaden {

constexpr double m_SQRTPI = 1.7724538509055160273;
const double R_1_SQRT2PI = 1.0 / sqrt(2.0 * M_PI); // sqrt not constexpr on all platforms (yet)

using std::abs; // important!!

inline std::string tostring(const int i) {
  std::ostringstream S;
  S << i;
  return S.str();
}

template<typename T> auto sqr(const T x) { return x * x; }

inline double gaussian_kernel(const double x, const double y, const double sigma) {
  const auto d = (x - y) / sigma;
  return R_1_SQRT2PI * exp(-d * d / 2.0) / sigma;
}

// Derivative of the Fermi-Dirac function: -d/dw f_FD(x-y) with effective temperature sigma.
inline double derfd_kernel(const double x, const double y, const double sigma) {
  const auto d = (x - y) / sigma;
  return 1.0 / ((1.0 + cosh(d)) * 2.0 * sigma);
}

template<typename S, typename T, typename FNC>
void convolve(const std::vector<S> &mesh, std::vector<T> &a, const double sigma, 
              FNC kernel, const double cutoff_ratio = 100) {
  const auto nr_mesh = mesh.size();
  auto b(a); // source
  a[0]           = b[0];
  a[nr_mesh - 1] = b[nr_mesh - 1];
  for (auto i = 1; i < nr_mesh - 1; i++) {
    const auto x = mesh[i];
    if (abs(x) < cutoff_ratio * sigma) {
      auto sum = 0.0;
      auto sum_kernel = 0.0;
      for (auto j = 1; j < nr_mesh - 1; j++) {
        const auto y     = mesh[j];
        const auto width = (mesh[j + 1] + mesh[j]) / 2.0 - (mesh[j - 1] + mesh[j]) / 2.0;
        const auto kw    = kernel(x, y, sigma) * width;
        assert(std::isfinite(kw));
        sum += b[j] * kw;
        sum_kernel += kw;
      }
      a[i] = sum;
    } else {
      // High temperatures: convolution not necessary
      a[i] = b[i];
    }
  }
}

template<typename S, typename T>
void save(const std::string &filename, const std::vector<S> &x, const std::vector<T> &y, 
          bool verbose, const int SAVE_PREC = 18) {
  if (verbose) { std::cout << "Saving " << filename << std::endl; }
  std::ofstream F(filename.c_str());
  if (!F) throw std::runtime_error("Failed to open " + filename + " for writing.");
  F << std::setprecision(SAVE_PREC);
  assert(x.size() == y.size());
  const auto nr = x.size();
  for (auto i = 0; i < nr; i++) { F << x[i] << " " << y[i] << std::endl; }
}

// Estimate the weight using the trapezoidal rule. The x array must be sorted.
template<typename T> T trapez(const std::vector<T> &x, const std::vector<T> &y) {
  T weight = 0.0;
  assert(x.size() == y.size());
  const auto nr = x.size();
  for (auto i = 1; i < nr; i++) {
    assert(x[i] >= x[i-1]);
    weight += (y[i-1] + y[i]) / 2 * (x[i] - x[i-1]);
  }
  return weight;
}

// Create a mesh on which the output spectral function will be computed. a is the accumulation point of the mesh.
auto make_mesh(const double min, const double max, const double ratio, 
               const double a, const bool add_positive = true, const bool add_negative = false) {
  assert(min < max);
  assert(ratio > 1.0);
  const auto rescale_factor = (max-a)/max;
  std::vector<double> mesh;
  for (double z = max; z > min; z /= ratio) {
    const auto x = a + z * rescale_factor;
    if (add_positive) mesh.push_back(x);
    if (add_negative) mesh.push_back(-x);
  }
  std::sort(mesh.begin(), mesh.end());
  return mesh;
}

auto load_mesh(const std::string &filename, bool verbose = false) {
  auto l = readtable<double,double>(filename, verbose);
  std::vector<double> mesh;
  for (const auto &first : l | ranges::views::transform([](const auto& pair) { return pair.first; }))
    mesh.push_back(first);
  return mesh;
}

auto file_size(std::ifstream &f) {
  f.seekg(0, std::ios::beg);
  const auto begin_pos = f.tellg();
  f.seekg(0, std::ios::end);
  const auto end_pos   = f.tellg();
  return end_pos - begin_pos;
}
   
auto nr_doubles(std::ifstream &f) {
  const auto len = file_size(f);
  assert(len % sizeof(double) == 0);
  return len/sizeof(double);
}
  
auto nr_rows(std::ifstream &f, const int cols) {
  const auto d = nr_doubles(f);
  assert(d % cols == 0);
  return d / cols;
}
   
// Load a file containing binary representation of raw spectral density. The grid is not assumed to be uniform.
auto load(const std::string &filename, const int nrcol, const bool verbose) {
  std::ifstream f(filename.c_str(), std::ios::in | std::ios::binary);
  if (!f.good() || f.eof() || !f.is_open()) 
    throw std::runtime_error("Error opening file " + filename + " for reading.");
  if (verbose) { std::cout << "Reading " << filename << std::endl; }
  const auto len = file_size(f); // in bytes
  const auto cols = 1 + nrcol;
  const auto rows = nr_rows(f, cols);
  if (verbose) { std::cout << "len=" << len << ", " << rows << " data points" << std::endl; }
  auto buffer = std::vector<double>(cols*rows);
  f.seekg(0, std::ios::beg); // Return to the beginning of the file.
  f.read((char *)buffer.data(), len);
  if (f.fail()) throw std::runtime_error("Error reading " + filename);
  f.close();
  return buffer;
}

class Broaden {
 private:
   bool verbose         = false; // output verbosity level
   bool sumrules        = false; // compute the integrals for testing the T!=0 sum rules
   bool cumulative      = false; // output of integrated spectral function
   double broaden_min   = 1e-7;  // parameters defining the frequency mesh
   double broaden_max   = 2.0;
   double broaden_ratio = 1.01;
   double alpha;          // broadening parameter alpha
   double ggamma, dgamma; // broadening parameter gamma (final Gaussian, derFD)
   double T;              // temperature parameter
   double omega0_ratio;   // omega0=omega0_ratio*T
   double omega0;
   double accumulation = 0.0;     // accumpulation point for the mesh
   bool one            = false;   // For Nz=1, no subdir.
   bool normalization  = false;   // What cross-over function to use?
   double filterlow    = 0.0;     // filter all input data points with |omega|<filterlow
   double filterhigh   = DBL_MAX; // filter all input data points with |omega|>filterhigh
   bool keeppositive   = true;    // Keep omega>0 data points when reading input data
   bool keepnegative   = true;    // Keep omega<0 data points when reading input data
   bool meshpositive   = true;    // Make omega>0 output mesh
   bool meshnegative   = true;    // Make omega<0 output mesh
   bool gaussian      = false; // Gaussian broadening scheme
   bool finalgaussian = false; // Final pass of Gaussian broadening of width ggamma*T
   bool finalderfd    = false; // Final pass of broadening with a derivative of FD distribution
   int nrcol = 1; // Number of columns
   int col   = 1; // Which y column are we interested in?
   std::string name; // filename of binary files containing the raw data
   int Nz;      // Number of spectra (1..Nz)
   const std::string output_filename     = "spec.dat";
   const std::string cumulative_filename = "cumulative.dat";
   std::vector<std::vector<double>> buffers; // binary data buffers
   using mapdd = std::map<double, double>;
   using vec = std::vector<double>;
   mapdd spec;           // Spectrum
   unsigned int nr_spec; // Number of raw spectrum points
   vec vfreq, vspec;     // Same info as spectrum, but in vector<double> form
   vec mesh; // Frequency mesh
   vec a;    // Spectral function
   vec c;    // Cumulative spectrum = int_{-inf}^omega a(x)dx.
   std::string mesh_filename = "";
   
   void usage(std::ostream &F = std::cout) {
     F << "Usage: broaden <name> <Nz> <alpha> <T> [omega0_ratio]" << std::endl;
     F << std::endl;
     F << "Optional parameters:" << std::endl;
     F << " -h -- show help (when used as sole cmd line switch)" << std::endl;
     F << " -v -- verbose" << std::endl;
     F << " -m <min> -- minimal mesh frequency" << std::endl;
     F << " -M <max> -- maximal mesh frequency" << std::endl;
     F << " -r <ratio> -- ratio between two consecutive frequency points" << std::endl;
     F << " -o -- one .dat file" << std::endl;
     F << " -2 -- use the 2nd column for weight values (complex spectra)" << std::endl;
     F << " -3 -- use the 3rd column for weight values (complex spectra)" << std::endl;
     F << " -n -- normalization-conserving broadening kernel" << std::endl;
     F << " -s -- compute weighted integrals for testing sum-rules" << std::endl;
     F << " -c -- compute cumulative spectrum" << std::endl;
     F << " -g -- Gaussian broadening (width alpha)" << std::endl;
     F << " -f -- final Gaussian broadening pass" << std::endl;
     F << " -x -- final derFD broadening pass" << std::endl;
     F << " -a -- accumulation point for the mesh" << std::endl;
     F << " -l -- filter out low-frequency raw data" << std::endl;
     F << " -h -- filter out high-frequency raw data" << std::endl;
     F << " -P -- keep only positive input frequencies" << std::endl;
     F << " -N -- keep only negative input frequencies" << std::endl;
     F << " -A -- output only positive frequencies" << std::endl;
     F << " -B -- output only negative frequencies" << std::endl;
     F << " -L <filename> -- load the frequency mesh from a file" << std::endl;
   }
   
   void cmd_line(int argc, char *argv[]) {
     if (argc == 2 && std::string(argv[1]) == "-h") {
       usage();
       exit(EXIT_SUCCESS);
     }
     char c;
     while (c = getopt(argc, argv, "vm:M:r:o23nscgf:x:a:l:h:PNABL:"), c != -1) {
       switch (c) {
       case 'c': cumulative = true; break;
       case 's': sumrules = true; break;
       case 'v': verbose = true; break;
       case 'm':
         broaden_min = atof(optarg);
         if (verbose) { std::cout << "broaden_min=" << broaden_min << std::endl; }
         break;
       case 'M':
         broaden_max = atof(optarg);
         if (verbose) { std::cout << "broaden_max=" << broaden_max << std::endl; }
         break;
       case 'r':
         broaden_ratio = atof(optarg);
         if (verbose) { std::cout << "broaden_ratio=" << broaden_ratio << std::endl; }
         break;
       case 'o': one = true; break;
       case '2':
         nrcol = 2;
         col   = 1;
         break;
       case '3':
         nrcol = 2;
         col   = 2;
         break;
       case 'n': normalization = true; break;
       case 'g': gaussian = true; break;
       case 'f':
         finalgaussian = true;
         ggamma        = atof(optarg);
         if (verbose) { std::cout << "final Gaussian with gamma=" << ggamma << std::endl; }
         break;
       case 'x':
         finalderfd = true;
         dgamma     = atof(optarg);
         if (verbose) { std::cout << "final derFD with gamma=" << dgamma << std::endl; }
         break;
       case 'a':
         accumulation = atof(optarg);
         if (verbose) { std::cout << "accumulation=" << accumulation << std::endl; }
         break;
       case 'l':
         filterlow = atof(optarg);
         if (verbose) { std::cout << "filterlow=" << filterlow << std::endl; }
         break;
       case 'h':
         filterhigh = atof(optarg);
         if (verbose) { std::cout << "filterhigh=" << filterhigh << std::endl; }
         break;
       case 'P': keepnegative = false; break;
       case 'N': keeppositive = false; break;
       case 'A': meshnegative = false; break;
       case 'B': meshpositive = false; break;
       case 'L':
         mesh_filename = std::string(optarg);
         break;
       default: abort();
       }
     }
     auto remaining = argc - optind; // arguments left
     if (remaining != 5 && remaining != 4) {
       usage();
       exit(1);
     }
     name = std::string(argv[optind]); // Name of spectral density files
     Nz = atoi(argv[optind + 1]); // Number of z-values
     assert(Nz >= 1);
     alpha = atof(argv[optind + 2]); // High-energy broadening parameter
     assert(alpha > 0.0);
     T = atof(argv[optind + 3]); // Temperature
     assert(T > 0.0);
     if (remaining == 5) {
       omega0_ratio = atof(argv[optind + 4]); // omega0/T
       assert(omega0_ratio > 0.0);
       omega0 = omega0_ratio * T;
     }
     if (remaining == 4) {
       omega0_ratio = 1e-9; // Effectively zero
       omega0       = omega0_ratio * T;
     }
     std::cout << "Processing: " << name << std::endl;
     if (verbose) { std::cout << "Nz=" << Nz << " alpha=" << alpha << " T=" << T << " omega0_ratio=" << omega0_ratio << std::endl; }
   }
   void check_buffer_normalisation(const std::vector<double> &buffer, const int col) {
     const auto cols = 1 + nrcol;
     const auto rows = buffer.size()/cols;
     auto sum = 0.0;
     for (auto j = 0; j < rows; j++) sum += buffer[cols * j + col];
     std::cout << "Weight=" << sum << std::endl;
   }
   // Load all the input data.
   void read_files() {
     buffers.resize(Nz+1);
     for (auto i = 1; i <= Nz; i++) {
       buffers[i] = load(one && Nz == 1 ? name : tostring(i) + "/" + name, nrcol, verbose);
       if (verbose) check_buffer_normalisation(buffers[i], col);
     }
   }
   // Combine data from all NRG runs (z-averaging).
   void merge() {
     const auto cols = 1 + nrcol; // number of elements in a line
     // Sum weight corresponding to equal frequencies.  Map of (frequency,weight) pairs is used for this purpose.
     for (auto i = 1; i <= Nz; i++) {
       for (auto l = 0; l < buffers[i].size()/cols; l++) {
         auto freq  = buffers[i][cols * l];
         auto value = buffers[i][cols * l + col];
         spec[freq] += value;
       }
     }
     nr_spec = spec.size();
     if (verbose) { std::cout << nr_spec << " unique frequencies." << std::endl; }
     // Normalize weight by 1/Nz, determine total weight, and store the (frequency,weight) data in the form of linear
     // vectors for faster access in the ensuing calculations.
     auto sum = 0.0;
     for (auto & I : spec) {
       const auto weight = (I.second /= Nz); // Normalize weight on the fly
       const auto freq   = I.first;
       vfreq.push_back(freq);
       vspec.push_back(weight);
       sum += weight;
     }
     if (verbose) { std::cout << "Total weight=" << sum << std::endl; }
     assert(vfreq.size() == nr_spec && vspec.size() == nr_spec);
   }

   void filter() {
     for (auto j = 0; j < nr_spec; j++) {
       if (abs(vfreq[j]) < filterlow) { vspec[j] = 0.0; }
       if (abs(vfreq[j]) > filterhigh) { vspec[j] = 0.0; }
     }
     for (auto j = 0; j < nr_spec; j++) {
       if (!keeppositive && vfreq[j] >= 0) { vspec[j] = 0.0; }
       if (!keepnegative && vfreq[j] < 0) { vspec[j] = 0.0; }
     }
   }

   void integrals_for_sumrules() {
     const auto nr    = vfreq.size();
     auto sum         = 0.0;
     auto sumpos      = 0.0;
     auto sumneg      = 0.0;
     auto sumfermi    = 0.0;
     auto sumbose     = 0.0;
     auto sumfermiinv = 0.0;
     auto sumboseinv  = 0.0;
     for (int i = 0; i < nr; i++) {
       const auto omega = vfreq[i];
       const auto w     = vspec[i];
       sum += w;
       if (omega > 0.0) { sumpos += w; }
       if (omega < 0.0) { sumneg += w; }
       const auto f  = 1 / (1 + exp(-omega / T));
       const auto b  = 1 / (1 - exp(-omega / T));
       const auto fi = 1 / (1 + exp(+omega / T)); // fi=1-f
       const auto bi = 1 / (1 - exp(+omega / T)); // bi=1-b
       if (std::isfinite(f))  { sumfermi    += f * w;  }
       if (std::isfinite(f))  { sumbose     += b * w;  }
       if (std::isfinite(fi)) { sumfermiinv += fi * w; }
       if (std::isfinite(bi)) { sumboseinv  += bi * w; }
     }
     std::cout << "Total weight=" << sum << std::endl;
     std::cout << "Positive-omega weight=" << sumpos << std::endl;
     std::cout << "Negative-omega weight=" << sumneg << std::endl;
     std::cout << "Integral with fermionic kernel=" << sumfermi << std::endl;
     std::cout << "Integral with bosonic kernel=" << sumbose << std::endl;
     std::cout << "Integral with fermionic kernel (omega -> -omega)=" << sumfermiinv << std::endl;
     std::cout << "Integral with bosonic kernel (omega -> -omega)=" << sumboseinv << std::endl;
   }

   // Modified log-Gaussian broadening kernel. For gamma=alpha/4, the kernel is symmetric in both arguments. e is the
   // energy of the spectral function point being computed. ept is the energy of point.
   inline auto BR_L_orig(const double e, const double ept) {
     if ((e < 0.0 && ept > 0.0) || (e > 0.0 && ept < 0.0)) return 0.0;
     if (ept == 0.0) return 0.0;
     const auto gamma = alpha / 4;
     return exp(-sqr(log(e / ept) / alpha - gamma)) / (alpha * abs(e) * m_SQRTPI);
   }

   // As above, with support for a shifted accumulation point
   inline auto BR_L_acc(const double e0, const double ept0) {
     auto e   = e0;
     auto ept = ept0;
     if (e > accumulation && ept > accumulation) {
       const auto shift = accumulation;
       e                = e - shift;
       ept              = ept - shift;
     }
     if (e < -accumulation && ept < -accumulation) {
       const auto shift = -accumulation; // note the sign!
       e                = e - shift;
       ept              = ept - shift;
     }
     if ((e < 0.0 && ept > 0.0) || (e > 0.0 && ept < 0.0)) return 0.0;
     if (ept == 0.0) return 0.0;
     const auto gamma = alpha / 4;
     return exp(-sqr(log(e / ept) / alpha - gamma)) / (alpha * abs(e) * m_SQRTPI);
   }

#define BR_L BR_L_acc

   // Normalized to 1, width omega0. The kernel is symmetric in both arguments.
   inline auto BR_G(const double e, const double ept) { 
     return exp(-sqr((e - ept) / omega0)) / (omega0 * m_SQRTPI); 
   }

   inline auto BR_G_alpha(const double e, const double ept) {
     const auto width = alpha;
     return exp(-sqr((e - ept) / width)) / (width * m_SQRTPI);
   }

   inline auto BR_h0(const double x) {
     const auto absx = abs(x);
     return absx > omega0 ? 1.0 : exp(-sqr(log(absx / omega0) / alpha));
   }

   // normalization = true: Cross-over funciton as proposed by A. Weichselbaum et al. 
   // normalization = false: BR_h is a function of 'e', not of 'ept'! This breaks the normalization at finite
   // temperatures. It gives nicer spectra, especially in combination with the self-energy trick.
   inline auto BR_h(const double e, const double ept) {
     return normalization ? BR_h0(ept) : BR_h0(e);
   }

   // e - output energy
   // ept - energy of the delta peak (data point)
   auto bfnc(const double e, const double ept) {
     if (gaussian) {
       return BR_G_alpha(e, ept);
     } else {
       const auto part_l = BR_L(e, ept);
       const auto part_g = BR_G(e, ept);
       const auto h      = BR_h(e, ept);
       assert(h >= 0.0 && h <= 1.0);
       return part_l * h + part_g * (1.0 - h);
     }
   }

   vec broaden(const vec &mesh) {
     if (verbose) { std::cout << "Broadening. Number of mesh points = " << mesh.size() << std::endl; }
     vec result(mesh.size());
     std::transform(mesh.begin(), mesh.end(), result.begin(), [this](const auto m) {
       return std::transform_reduce(vspec.begin(), vspec.end(), vfreq.begin(), 0.0, std::plus<>(), 
                                    [this,m](const auto weight, const auto freq) {
                                      return weight * bfnc(m, freq); }); });
     return result;
   }
   
   // Cumulative spectrum
   void calc_cumulative(const vec &mesh, vec &c) {
     const auto nr_mesh = mesh.size();
     if (verbose) std::cout << "Calculating cumulative spectrum." << std::endl;
     c.resize(nr_mesh);
     auto sum = 0.0;
     auto j   = 0;
     for (auto i = 0; i < nr_mesh; i++) {
       const auto max_freq = mesh[i];
       while (vfreq[j] < max_freq) {
         sum += vspec[j];
         j++;
       }
       c[i] = sum;
     }
     std::cout << "End sum=" << sum << std::endl;
   }

   void check_normalizations(const vec &x) {
     vec y(x.size());
     // For a range of delta peak positions, compute the total weight of the resulting broadened spectra.
     for (auto z = 1e-10; z < 1.0; z *= 10) {
       const auto nr = x.size();
       for (auto i = 0; i < nr; i++) { y[i] = bfnc(x[i], z); }
       std::cout << "z=" << z << " weight=" << trapez(x, y) << std::endl;
     }
   }
 public:
   Broaden(int argc, char *argv[]) {
     cmd_line(argc, argv);
   }
   void calc() {
     read_files();
     merge();
     filter();
     if (sumrules) integrals_for_sumrules();
     if (mesh_filename != "")
       mesh = load_mesh(mesh_filename, verbose);
     else
       mesh = make_mesh(broaden_min, broaden_max, broaden_ratio, accumulation, meshpositive, meshnegative);
     if (verbose) check_normalizations(mesh);
     a = broaden(mesh);
     if (finalgaussian) convolve(mesh, a, ggamma * T, gaussian_kernel);
     if (finalderfd) convolve(mesh, a, dgamma * T, derfd_kernel);
     std::cout << "Estimated weight (trapezoidal rule)=" << trapez(mesh, a) << std::endl;
     save(output_filename, mesh, a, verbose);
     if (cumulative) {
       calc_cumulative(mesh, c);
       save(cumulative_filename, mesh, c, verbose);
     }
   }
};

} // namespace

#endif
