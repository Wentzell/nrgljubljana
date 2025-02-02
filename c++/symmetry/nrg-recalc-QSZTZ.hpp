namespace NRG {

// *** WARNING!!! Modify nrg-recalc-QSZTZ.cc.m4, not nrg-recalc-QSZTZ.cc !!!

// Quantum number dependant recalculation routines
// Rok Zitko, rok.zitko@ijs.si, Mar 2016
// This file pertains to (Q,Sz,Tz) subspaces

namespace NRG {

// m4 macros for nrg-recalc-*.cc files
// Rok Zitko, rok.zitko@ijs.si, 2007-2020










  




}


template<typename SC>
MatrixElements<SC> SymmetryQSZTZ<SC>::recalc_doublet(const DiagInfo<SC> &diag, const SubspaceStructure &substruct, const MatrixElements<SC> &cold) const {
  MatrixElements<SC> cnew;
  for(const auto &[I1, eig]: diag) {
    int q1   = I1.get("Q");
    int ssz1  = I1.get("SZ");
    int tz1 = I1.get("TZ");
    Invar Ip;

    // Invar(1,2,+-1,0) is correct. 1 = add charge, 2 = doublet,
    // 1 = triplet (because working with abs orbital momentum QNs)

    Ip = Invar(q1 - 1, ssz1 + 1, tz1 - 1);
    {
  nrglog('f', "RECALC(fn=" << "qsztz/qsztz-doubletp-1.dat" << ", Iop=" << Invar(1, -1, +1) << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc<SC>> recalc_table = {
#include "qsztz/qsztz-doubletp-1.dat"
      };
      auto cn = this->recalc_general(diag, substruct, cold, I1, Ip, recalc_table, Invar(1, -1, +1));
      if (cn) cnew[II] = *cn;
    }
  }
};

    Ip = Invar(q1 - 1, ssz1 - 1, tz1 - 1);
    {
  nrglog('f', "RECALC(fn=" << "qsztz/qsztz-doubletm-1.dat" << ", Iop=" << Invar(1, +1, +1) << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc<SC>> recalc_table = {
#include "qsztz/qsztz-doubletm-1.dat"
      };
      auto cn = this->recalc_general(diag, substruct, cold, I1, Ip, recalc_table, Invar(1, +1, +1));
      if (cn) cnew[II] = *cn;
    }
  }
};

    Ip = Invar(q1 - 1, ssz1 + 1, tz1);
    {
  nrglog('f', "RECALC(fn=" << "qsztz/qsztz-doubletp0.dat" << ", Iop=" << Invar(1, -1, 0) << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc<SC>> recalc_table = {
#include "qsztz/qsztz-doubletp0.dat"
      };
      auto cn = this->recalc_general(diag, substruct, cold, I1, Ip, recalc_table, Invar(1, -1, 0));
      if (cn) cnew[II] = *cn;
    }
  }
};

    Ip = Invar(q1 - 1, ssz1 - 1, tz1);
    {
  nrglog('f', "RECALC(fn=" << "qsztz/qsztz-doubletm0.dat" << ", Iop=" << Invar(1, +1, 0) << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc<SC>> recalc_table = {
#include "qsztz/qsztz-doubletm0.dat"
      };
      auto cn = this->recalc_general(diag, substruct, cold, I1, Ip, recalc_table, Invar(1, +1, 0));
      if (cn) cnew[II] = *cn;
    }
  }
};

    Ip = Invar(q1 - 1, ssz1 + 1, tz1 + 1);
    {
  nrglog('f', "RECALC(fn=" << "qsztz/qsztz-doubletp+1.dat" << ", Iop=" << Invar(1, -1, -1) << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc<SC>> recalc_table = {
#include "qsztz/qsztz-doubletp+1.dat"
      };
      auto cn = this->recalc_general(diag, substruct, cold, I1, Ip, recalc_table, Invar(1, -1, -1));
      if (cn) cnew[II] = *cn;
    }
  }
};

    Ip = Invar(q1 - 1, ssz1 - 1, tz1 + 1);
    {
  nrglog('f', "RECALC(fn=" << "qsztz/qsztz-doubletm+1.dat" << ", Iop=" << Invar(1, +1, -1) << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc<SC>> recalc_table = {
#include "qsztz/qsztz-doubletm+1.dat"
      };
      auto cn = this->recalc_general(diag, substruct, cold, I1, Ip, recalc_table, Invar(1, +1, -1));
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
Opch<SC> SymmetryQSZTZ<SC>::recalc_irreduc(const Step &step, const DiagInfo<SC> &diag) const {
  Opch<SC> opch(P);
  for(const auto &[Ip, eig]: diag) {
    int qp   = Ip.get("Q");
    int sszp  = Ip.get("SZ");
    int tzp = Ip.get("TZ");
    Invar I1;

    I1 = Invar(qp + 1, sszp + 1, tzp + 1);
    {
  nrglog('f', "RECALC_F(fn=" << "qsztz/qsztz-spinup+1.dat" << ", ch=" << 0 << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip) && this->recalc_f_coupled(I1, Ip, this->Invar_f)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc_f<SC>> recalc_table = {
#include "qsztz/qsztz-spinup+1.dat"
      };
      opch[0][0][II] = this->recalc_f(diag, I1, Ip, recalc_table);
    }
  }
};

    I1 = Invar(qp + 1, sszp + 1, tzp);
    {
  nrglog('f', "RECALC_F(fn=" << "qsztz/qsztz-spinup0.dat" << ", ch=" << 0 << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip) && this->recalc_f_coupled(I1, Ip, this->Invar_f)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc_f<SC>> recalc_table = {
#include "qsztz/qsztz-spinup0.dat"
      };
      opch[0][0][II] = this->recalc_f(diag, I1, Ip, recalc_table);
    }
  }
};

    I1 = Invar(qp + 1, sszp + 1, tzp - 1);
    {
  nrglog('f', "RECALC_F(fn=" << "qsztz/qsztz-spinup-1.dat" << ", ch=" << 0 << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip) && this->recalc_f_coupled(I1, Ip, this->Invar_f)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc_f<SC>> recalc_table = {
#include "qsztz/qsztz-spinup-1.dat"
      };
      opch[0][0][II] = this->recalc_f(diag, I1, Ip, recalc_table);
    }
  }
};

    I1 = Invar(qp + 1, sszp - 1, tzp + 1);
    {
  nrglog('f', "RECALC_F(fn=" << "qsztz/qsztz-spindo+1.dat" << ", ch=" << 0 << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip) && this->recalc_f_coupled(I1, Ip, this->Invar_f)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc_f<SC>> recalc_table = {
#include "qsztz/qsztz-spindo+1.dat"
      };
      opch[0][0][II] = this->recalc_f(diag, I1, Ip, recalc_table);
    }
  }
};

    I1 = Invar(qp + 1, sszp - 1, tzp);
    {
  nrglog('f', "RECALC_F(fn=" << "qsztz/qsztz-spindo0.dat" << ", ch=" << 0 << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip) && this->recalc_f_coupled(I1, Ip, this->Invar_f)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc_f<SC>> recalc_table = {
#include "qsztz/qsztz-spindo0.dat"
      };
      opch[0][0][II] = this->recalc_f(diag, I1, Ip, recalc_table);
    }
  }
};

    I1 = Invar(qp + 1, sszp - 1, tzp - 1);
    {
  nrglog('f', "RECALC_F(fn=" << "qsztz/qsztz-spindo-1.dat" << ", ch=" << 0 << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip) && this->recalc_f_coupled(I1, Ip, this->Invar_f)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc_f<SC>> recalc_table = {
#include "qsztz/qsztz-spindo-1.dat"
      };
      opch[0][0][II] = this->recalc_f(diag, I1, Ip, recalc_table);
    }
  }
};
  }
  return opch;
}

template<typename SC>
MatrixElements<SC> SymmetryQSZTZ<SC>::recalc_triplet(const DiagInfo<SC> &diag, const SubspaceStructure &substruct, const MatrixElements<SC> &cold) const {
  MatrixElements<SC> cnew;
  for(const auto &[I1, eig]: diag) {
    int q1   = I1.get("Q");
    int ssz1  = I1.get("SZ");
    int tz1 = I1.get("TZ");
    Invar Ip;

    Ip = Invar(q1, ssz1, tz1);
    {
  nrglog('f', "RECALC(fn=" << "qsztz/qsztz-triplets.dat" << ", Iop=" << Invar(0, 3, 0) << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc<SC>> recalc_table = {
#include "qsztz/qsztz-triplets.dat"
      };
      auto cn = this->recalc_general(diag, substruct, cold, I1, Ip, recalc_table, Invar(0, 3, 0));
      if (cn) cnew[II] = *cn;
    }
  }
};

    Ip = Invar(q1, ssz1 + 2, tz1);
    {
  nrglog('f', "RECALC(fn=" << "qsztz/qsztz-tripletp.dat" << ", Iop=" << Invar(0, 3, 0) << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc<SC>> recalc_table = {
#include "qsztz/qsztz-tripletp.dat"
      };
      auto cn = this->recalc_general(diag, substruct, cold, I1, Ip, recalc_table, Invar(0, 3, 0));
      if (cn) cnew[II] = *cn;
    }
  }
};

    Ip = Invar(q1, ssz1 - 2, tz1);
    {
  nrglog('f', "RECALC(fn=" << "qsztz/qsztz-tripletm.dat" << ", Iop=" << Invar(0, 3, 0) << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc<SC>> recalc_table = {
#include "qsztz/qsztz-tripletm.dat"
      };
      auto cn = this->recalc_general(diag, substruct, cold, I1, Ip, recalc_table, Invar(0, 3, 0));
      if (cn) cnew[II] = *cn;
    }
  }
};
  }
  return cnew;
}

}
