namespace NRG {

// *** WARNING!!! Modify nrg-recalc-QSTZ.cc.m4, not nrg-recalc-QSTZ.cc !!!

// Quantum number dependant recalculation routines
// Rok Zitko, rok.zitko@ijs.si, Feb 2016
// This file pertains to (Q,S,Tz) subspaces

namespace NRG {

// m4 macros for nrg-recalc-*.cc files
// Rok Zitko, rok.zitko@ijs.si, 2007-2020










  




}


template<typename SC>
MatrixElements<SC> SymmetryQSTZ<SC>::recalc_doublet(const DiagInfo<SC> &diag, const MatrixElements<SC> &cold) const {
  MatrixElements<SC> cnew;
  for(const auto &[I1, eig]: diag) {
    int q1   = I1.get("Q");
    int ss1   = I1.get("SS");
    int tz1 = I1.get("TZ");
    Invar Ip;

    // Two different lengths: D_3CH_a and D_3CH_b

    // Invar(1,2,+-1,0) is correct. 1 = add charge, 2 = doublet,
    // 1 = triplet (because working with abs orbital momentum QNs)

    Ip = Invar(q1 - 1, ss1 + 1, tz1 - 1);
    {
  nrglog('f', "RECALC(fn=" << "qstz/qstz-doubletp-1.dat" << ", Iop=" << Invar(1, 2, +1) << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc<SC>> recalc_table = {
#include "qstz/qstz-doubletp-1.dat"
      };
      auto cn = this->recalc_general(diag, cold, I1, Ip, recalc_table, Invar(1, 2, +1));
      if (cn) cnew[II] = *cn;
    }
  }
};

    Ip = Invar(q1 - 1, ss1 - 1, tz1 - 1);
    {
  nrglog('f', "RECALC(fn=" << "qstz/qstz-doubletm-1.dat" << ", Iop=" << Invar(1, 2, +1) << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc<SC>> recalc_table = {
#include "qstz/qstz-doubletm-1.dat"
      };
      auto cn = this->recalc_general(diag, cold, I1, Ip, recalc_table, Invar(1, 2, +1));
      if (cn) cnew[II] = *cn;
    }
  }
};

    Ip = Invar(q1 - 1, ss1 + 1, tz1);
    {
  nrglog('f', "RECALC(fn=" << "qstz/qstz-doubletp0.dat" << ", Iop=" << Invar(1, 2, 0) << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc<SC>> recalc_table = {
#include "qstz/qstz-doubletp0.dat"
      };
      auto cn = this->recalc_general(diag, cold, I1, Ip, recalc_table, Invar(1, 2, 0));
      if (cn) cnew[II] = *cn;
    }
  }
};

    Ip = Invar(q1 - 1, ss1 - 1, tz1);
    {
  nrglog('f', "RECALC(fn=" << "qstz/qstz-doubletm0.dat" << ", Iop=" << Invar(1, 2, 0) << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc<SC>> recalc_table = {
#include "qstz/qstz-doubletm0.dat"
      };
      auto cn = this->recalc_general(diag, cold, I1, Ip, recalc_table, Invar(1, 2, 0));
      if (cn) cnew[II] = *cn;
    }
  }
};

    Ip = Invar(q1 - 1, ss1 + 1, tz1 + 1);
    {
  nrglog('f', "RECALC(fn=" << "qstz/qstz-doubletp+1.dat" << ", Iop=" << Invar(1, 2, -1) << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc<SC>> recalc_table = {
#include "qstz/qstz-doubletp+1.dat"
      };
      auto cn = this->recalc_general(diag, cold, I1, Ip, recalc_table, Invar(1, 2, -1));
      if (cn) cnew[II] = *cn;
    }
  }
};

    Ip = Invar(q1 - 1, ss1 - 1, tz1 + 1);
    {
  nrglog('f', "RECALC(fn=" << "qstz/qstz-doubletm+1.dat" << ", Iop=" << Invar(1, 2, -1) << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc<SC>> recalc_table = {
#include "qstz/qstz-doubletm+1.dat"
      };
      auto cn = this->recalc_general(diag, cold, I1, Ip, recalc_table, Invar(1, 2, -1));
      if (cn) cnew[II] = *cn;
    }
  }
};
  }
  return cnew;
}

// ch=1 <-> Tz=+1
// ch=2 <-> Tz=0
// ch=3 <-> Tz=-1

template<typename SC>
Opch<SC> SymmetryQSTZ<SC>::recalc_irreduc(const Step &step, const DiagInfo<SC> &diag) const {
  Opch<SC> opch(P);
  for(const auto &[Ip, eig]: diag) {
    int qp   = Ip.get("Q");
    int ssp   = Ip.get("SS");
    int tzp = Ip.get("TZ");
    Invar I1;

    // The different files just correspond to contributions computed
    // for various d[CR,sz,tz] operators.
    // Check: there should not be any lines with equal subspaces
    // indexes in different files!! That's indeed the case for the
    // generated files for symtype=QST.
    I1 = Invar(qp + 1, ssp + 1, tzp + 1);
    {
  nrglog('f', "RECALC_F(fn=" << "qstz/qstz-spinup+1.dat" << ", ch=" << 0 << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip) && this->recalc_f_coupled(I1, Ip, this->Invar_f)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc_f<SC>> recalc_table = {
#include "qstz/qstz-spinup+1.dat"
      };
      opch[0][0][II] = this->recalc_f(diag, I1, Ip, recalc_table);
    }
  }
};

    I1 = Invar(qp + 1, ssp + 1, tzp);
    {
  nrglog('f', "RECALC_F(fn=" << "qstz/qstz-spinup0.dat" << ", ch=" << 0 << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip) && this->recalc_f_coupled(I1, Ip, this->Invar_f)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc_f<SC>> recalc_table = {
#include "qstz/qstz-spinup0.dat"
      };
      opch[0][0][II] = this->recalc_f(diag, I1, Ip, recalc_table);
    }
  }
};

    I1 = Invar(qp + 1, ssp + 1, tzp - 1);
    {
  nrglog('f', "RECALC_F(fn=" << "qstz/qstz-spinup-1.dat" << ", ch=" << 0 << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip) && this->recalc_f_coupled(I1, Ip, this->Invar_f)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc_f<SC>> recalc_table = {
#include "qstz/qstz-spinup-1.dat"
      };
      opch[0][0][II] = this->recalc_f(diag, I1, Ip, recalc_table);
    }
  }
};

    I1 = Invar(qp + 1, ssp - 1, tzp + 1);
    {
  nrglog('f', "RECALC_F(fn=" << "qstz/qstz-spindo+1.dat" << ", ch=" << 0 << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip) && this->recalc_f_coupled(I1, Ip, this->Invar_f)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc_f<SC>> recalc_table = {
#include "qstz/qstz-spindo+1.dat"
      };
      opch[0][0][II] = this->recalc_f(diag, I1, Ip, recalc_table);
    }
  }
};

    I1 = Invar(qp + 1, ssp - 1, tzp);
    {
  nrglog('f', "RECALC_F(fn=" << "qstz/qstz-spindo0.dat" << ", ch=" << 0 << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip) && this->recalc_f_coupled(I1, Ip, this->Invar_f)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc_f<SC>> recalc_table = {
#include "qstz/qstz-spindo0.dat"
      };
      opch[0][0][II] = this->recalc_f(diag, I1, Ip, recalc_table);
    }
  }
};

    I1 = Invar(qp + 1, ssp - 1, tzp - 1);
    {
  nrglog('f', "RECALC_F(fn=" << "qstz/qstz-spindo-1.dat" << ", ch=" << 0 << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip) && this->recalc_f_coupled(I1, Ip, this->Invar_f)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc_f<SC>> recalc_table = {
#include "qstz/qstz-spindo-1.dat"
      };
      opch[0][0][II] = this->recalc_f(diag, I1, Ip, recalc_table);
    }
  }
};
  }
  return opch;
}

template<typename SC>
MatrixElements<SC> SymmetryQSTZ<SC>::recalc_triplet(const DiagInfo<SC> &diag, const MatrixElements<SC> &cold) const {
  MatrixElements<SC> cnew;
  for(const auto &[I1, eig]: diag) {
    int q1   = I1.get("Q");
    int ss1   = I1.get("SS");
    int tz1 = I1.get("TZ");
    Invar Ip;

    Ip = Invar(q1, ss1, tz1);
    {
  nrglog('f', "RECALC(fn=" << "qstz/qstz-triplets.dat" << ", Iop=" << Invar(0, 3, 0) << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc<SC>> recalc_table = {
#include "qstz/qstz-triplets.dat"
      };
      auto cn = this->recalc_general(diag, cold, I1, Ip, recalc_table, Invar(0, 3, 0));
      if (cn) cnew[II] = *cn;
    }
  }
};

    Ip = Invar(q1, ss1 + 2, tz1);
    {
  nrglog('f', "RECALC(fn=" << "qstz/qstz-tripletp.dat" << ", Iop=" << Invar(0, 3, 0) << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc<SC>> recalc_table = {
#include "qstz/qstz-tripletp.dat"
      };
      auto cn = this->recalc_general(diag, cold, I1, Ip, recalc_table, Invar(0, 3, 0));
      if (cn) cnew[II] = *cn;
    }
  }
};

    Ip = Invar(q1, ss1 - 2, tz1);
    {
  nrglog('f', "RECALC(fn=" << "qstz/qstz-tripletm.dat" << ", Iop=" << Invar(0, 3, 0) << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc<SC>> recalc_table = {
#include "qstz/qstz-tripletm.dat"
      };
      auto cn = this->recalc_general(diag, cold, I1, Ip, recalc_table, Invar(0, 3, 0));
      if (cn) cnew[II] = *cn;
    }
  }
};
  }
  return cnew;
}

}
