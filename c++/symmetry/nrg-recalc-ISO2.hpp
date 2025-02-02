namespace NRG {

// *** WARNING!!! Modify nrg-recalc-ISO2.cc.m4, not nrg-recalc-ISO2.cc !!!

// Quantum number dependant recalculation routines
// Rok Zitko, rok.zitko@ijs.si, Feb 2006, May 2008
// This file pertains to (I,S) subspaces
// Version for EVEN number of impurities.

namespace NRG {

// m4 macros for nrg-recalc-*.cc files
// Rok Zitko, rok.zitko@ijs.si, 2007-2020










  




}


template<typename SC>
MatrixElements<SC> SymmetryISO2<SC>::recalc_doublet(const DiagInfo<SC> &diag, const MatrixElements<SC> &cold) const {
  MatrixElements<SC> cnew;
  for(const auto &[I1, eig]: diag) {
    int ii1 = I1.get("II");
    int ss1 = I1.get("SS");
    Invar Ip;

    Ip = Invar(ii1 - 1, ss1 + 1);
    switch (P.channels) {
  case 1: { {
  nrglog('f', "RECALC(fn=" << "iso2/iso2-1ch-doubletmp.dat" << ", Iop=" << Invar(2, 2) << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc<SC>> recalc_table = {
#include "iso2/iso2-1ch-doubletmp.dat"
      };
      auto cn = this->recalc_general(diag, cold, I1, Ip, recalc_table, Invar(2, 2));
      if (cn) cnew[II] = *cn;
    }
  }
} } break;
  case 2: { {
  nrglog('f', "RECALC(fn=" << "iso2/iso2-2ch-doubletmp.dat" << ", Iop=" << Invar(2, 2) << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc<SC>> recalc_table = {
#include "iso2/iso2-2ch-doubletmp.dat"
      };
      auto cn = this->recalc_general(diag, cold, I1, Ip, recalc_table, Invar(2, 2));
      if (cn) cnew[II] = *cn;
    }
  }
} } break;
  default: my_assert_not_reached();
  };

    Ip = Invar(ii1-1, ss1-1);
    switch (P.channels) {
  case 1: { {
  nrglog('f', "RECALC(fn=" << "iso2/iso2-1ch-doubletmm.dat" << ", Iop=" << Invar(2, 2) << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc<SC>> recalc_table = {
#include "iso2/iso2-1ch-doubletmm.dat"
      };
      auto cn = this->recalc_general(diag, cold, I1, Ip, recalc_table, Invar(2, 2));
      if (cn) cnew[II] = *cn;
    }
  }
} } break;
  case 2: { {
  nrglog('f', "RECALC(fn=" << "iso2/iso2-2ch-doubletmm.dat" << ", Iop=" << Invar(2, 2) << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc<SC>> recalc_table = {
#include "iso2/iso2-2ch-doubletmm.dat"
      };
      auto cn = this->recalc_general(diag, cold, I1, Ip, recalc_table, Invar(2, 2));
      if (cn) cnew[II] = *cn;
    }
  }
} } break;
  default: my_assert_not_reached();
  };

    Ip = Invar(ii1+1, ss1+1);
    switch (P.channels) {
  case 1: { {
  nrglog('f', "RECALC(fn=" << "iso2/iso2-1ch-doubletpp.dat" << ", Iop=" << Invar(2, 2) << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc<SC>> recalc_table = {
#include "iso2/iso2-1ch-doubletpp.dat"
      };
      auto cn = this->recalc_general(diag, cold, I1, Ip, recalc_table, Invar(2, 2));
      if (cn) cnew[II] = *cn;
    }
  }
} } break;
  case 2: { {
  nrglog('f', "RECALC(fn=" << "iso2/iso2-2ch-doubletpp.dat" << ", Iop=" << Invar(2, 2) << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc<SC>> recalc_table = {
#include "iso2/iso2-2ch-doubletpp.dat"
      };
      auto cn = this->recalc_general(diag, cold, I1, Ip, recalc_table, Invar(2, 2));
      if (cn) cnew[II] = *cn;
    }
  }
} } break;
  default: my_assert_not_reached();
  };

    Ip = Invar(ii1+1, ss1-1);
    switch (P.channels) {
  case 1: { {
  nrglog('f', "RECALC(fn=" << "iso2/iso2-1ch-doubletpm.dat" << ", Iop=" << Invar(2, 2) << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc<SC>> recalc_table = {
#include "iso2/iso2-1ch-doubletpm.dat"
      };
      auto cn = this->recalc_general(diag, cold, I1, Ip, recalc_table, Invar(2, 2));
      if (cn) cnew[II] = *cn;
    }
  }
} } break;
  case 2: { {
  nrglog('f', "RECALC(fn=" << "iso2/iso2-2ch-doubletpm.dat" << ", Iop=" << Invar(2, 2) << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc<SC>> recalc_table = {
#include "iso2/iso2-2ch-doubletpm.dat"
      };
      auto cn = this->recalc_general(diag, cold, I1, Ip, recalc_table, Invar(2, 2));
      if (cn) cnew[II] = *cn;
    }
  }
} } break;
  default: my_assert_not_reached();
  };
  }
  return cnew;
}

template<typename SC>
Opch<SC> SymmetryISO2<SC>::recalc_irreduc(const Step &step, const DiagInfo<SC> &diag) const {
  Opch<SC> opch(P);
  for(const auto &[Ip, eig]: diag) {
    Invar I1;

    // NOTE: ii,ss only couples to ii+-1,ss+-1 in general, even for
    // several channels.

    int iip = Ip.get("II");
    int ssp = Ip.get("SS");
    // NN is index n of f_n, the last site in the chain prior to adding
    // the new site (f_{n+1}).
    int NN = step.getnn();

    I1 = Invar(iip + 1, ssp + 1);
    switch (P.channels) {
  case 1: { {
  nrglog('f', "RECALC_F(fn=" << "iso2/iso2-1ch-spinup-isoupa.dat" << ", ch=" << 0 << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip) && this->recalc_f_coupled(I1, Ip, this->Invar_f)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc_f<SC>> recalc_table = {
#include "iso2/iso2-1ch-spinup-isoupa.dat"
      };
      opch[0][0][II] = this->recalc_f(diag, I1, Ip, recalc_table);
    }
  }
} } break;
  case 2: { {
  nrglog('f', "RECALC_F(fn=" << "iso2/iso2-2ch-spinup-isoupa.dat" << ", ch=" << 0 << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip) && this->recalc_f_coupled(I1, Ip, this->Invar_f)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc_f<SC>> recalc_table = {
#include "iso2/iso2-2ch-spinup-isoupa.dat"
      };
      opch[0][0][II] = this->recalc_f(diag, I1, Ip, recalc_table);
    }
  }
};
     	      {
  nrglog('f', "RECALC_F(fn=" << "iso2/iso2-2ch-spinup-isoupb.dat" << ", ch=" << 1 << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip) && this->recalc_f_coupled(I1, Ip, this->Invar_f)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc_f<SC>> recalc_table = {
#include "iso2/iso2-2ch-spinup-isoupb.dat"
      };
      opch[1][0][II] = this->recalc_f(diag, I1, Ip, recalc_table);
    }
  }
} } break;
  default: my_assert_not_reached();
  };

    I1 = Invar(iip+1, ssp-1);
    switch (P.channels) {
  case 1: { {
  nrglog('f', "RECALC_F(fn=" << "iso2/iso2-1ch-spindown-isoupa.dat" << ", ch=" << 0 << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip) && this->recalc_f_coupled(I1, Ip, this->Invar_f)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc_f<SC>> recalc_table = {
#include "iso2/iso2-1ch-spindown-isoupa.dat"
      };
      opch[0][0][II] = this->recalc_f(diag, I1, Ip, recalc_table);
    }
  }
} } break;
  case 2: { {
  nrglog('f', "RECALC_F(fn=" << "iso2/iso2-2ch-spindown-isoupa.dat" << ", ch=" << 0 << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip) && this->recalc_f_coupled(I1, Ip, this->Invar_f)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc_f<SC>> recalc_table = {
#include "iso2/iso2-2ch-spindown-isoupa.dat"
      };
      opch[0][0][II] = this->recalc_f(diag, I1, Ip, recalc_table);
    }
  }
};
	          {
  nrglog('f', "RECALC_F(fn=" << "iso2/iso2-2ch-spindown-isoupb.dat" << ", ch=" << 1 << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip) && this->recalc_f_coupled(I1, Ip, this->Invar_f)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc_f<SC>> recalc_table = {
#include "iso2/iso2-2ch-spindown-isoupb.dat"
      };
      opch[1][0][II] = this->recalc_f(diag, I1, Ip, recalc_table);
    }
  }
} } break;
  default: my_assert_not_reached();
  };

    I1 = Invar(iip-1, ssp+1);
    switch (P.channels) {
  case 1: { {
  nrglog('f', "RECALC_F(fn=" << "iso2/iso2-1ch-spinup-isodowna.dat" << ", ch=" << 0 << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip) && this->recalc_f_coupled(I1, Ip, this->Invar_f)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc_f<SC>> recalc_table = {
#include "iso2/iso2-1ch-spinup-isodowna.dat"
      };
      opch[0][0][II] = this->recalc_f(diag, I1, Ip, recalc_table);
    }
  }
} } break;
  case 2: { {
  nrglog('f', "RECALC_F(fn=" << "iso2/iso2-2ch-spinup-isodowna.dat" << ", ch=" << 0 << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip) && this->recalc_f_coupled(I1, Ip, this->Invar_f)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc_f<SC>> recalc_table = {
#include "iso2/iso2-2ch-spinup-isodowna.dat"
      };
      opch[0][0][II] = this->recalc_f(diag, I1, Ip, recalc_table);
    }
  }
};
	          {
  nrglog('f', "RECALC_F(fn=" << "iso2/iso2-2ch-spinup-isodownb.dat" << ", ch=" << 1 << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip) && this->recalc_f_coupled(I1, Ip, this->Invar_f)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc_f<SC>> recalc_table = {
#include "iso2/iso2-2ch-spinup-isodownb.dat"
      };
      opch[1][0][II] = this->recalc_f(diag, I1, Ip, recalc_table);
    }
  }
} } break;
  default: my_assert_not_reached();
  };

    I1 = Invar(iip-1, ssp-1);
    switch (P.channels) {
  case 1: { {
  nrglog('f', "RECALC_F(fn=" << "iso2/iso2-1ch-spindown-isodowna.dat" << ", ch=" << 0 << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip) && this->recalc_f_coupled(I1, Ip, this->Invar_f)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc_f<SC>> recalc_table = {
#include "iso2/iso2-1ch-spindown-isodowna.dat"
      };
      opch[0][0][II] = this->recalc_f(diag, I1, Ip, recalc_table);
    }
  }
} } break;
  case 2: { {
  nrglog('f', "RECALC_F(fn=" << "iso2/iso2-2ch-spindown-isodowna.dat" << ", ch=" << 0 << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip) && this->recalc_f_coupled(I1, Ip, this->Invar_f)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc_f<SC>> recalc_table = {
#include "iso2/iso2-2ch-spindown-isodowna.dat"
      };
      opch[0][0][II] = this->recalc_f(diag, I1, Ip, recalc_table);
    }
  }
};
	          {
  nrglog('f', "RECALC_F(fn=" << "iso2/iso2-2ch-spindown-isodownb.dat" << ", ch=" << 1 << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip) && this->recalc_f_coupled(I1, Ip, this->Invar_f)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc_f<SC>> recalc_table = {
#include "iso2/iso2-2ch-spindown-isodownb.dat"
      };
      opch[1][0][II] = this->recalc_f(diag, I1, Ip, recalc_table);
    }
  }
} } break;
  default: my_assert_not_reached();
  };
  }
  return opch;
}

template<typename SC>
MatrixElements<SC> SymmetryISO2<SC>::recalc_triplet(const DiagInfo<SC> &diag, const MatrixElements<SC> &cold) const {
  MatrixElements<SC> cnew;
  for(const auto &[I1, eig]: diag) {
    int ii1 = I1.get("II");
    int ss1 = I1.get("SS");
    Invar Ip;

    Ip = Invar(ii1, ss1);
    switch (P.channels) {
  case 1: { {
  nrglog('f', "RECALC(fn=" << "iso2/iso2-1ch-triplets.dat" << ", Iop=" << Invar(1, 3) << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc<SC>> recalc_table = {
#include "iso2/iso2-1ch-triplets.dat"
      };
      auto cn = this->recalc_general(diag, cold, I1, Ip, recalc_table, Invar(1, 3));
      if (cn) cnew[II] = *cn;
    }
  }
} } break;
  case 2: { {
  nrglog('f', "RECALC(fn=" << "iso2/iso2-2ch-triplets.dat" << ", Iop=" << Invar(1, 3) << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc<SC>> recalc_table = {
#include "iso2/iso2-2ch-triplets.dat"
      };
      auto cn = this->recalc_general(diag, cold, I1, Ip, recalc_table, Invar(1, 3));
      if (cn) cnew[II] = *cn;
    }
  }
} } break;
  default: my_assert_not_reached();
  };

    Ip = Invar(ii1, ss1+2);
    switch (P.channels) {
  case 1: { {
  nrglog('f', "RECALC(fn=" << "iso2/iso2-1ch-tripletp.dat" << ", Iop=" << Invar(1, 3) << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc<SC>> recalc_table = {
#include "iso2/iso2-1ch-tripletp.dat"
      };
      auto cn = this->recalc_general(diag, cold, I1, Ip, recalc_table, Invar(1, 3));
      if (cn) cnew[II] = *cn;
    }
  }
} } break;
  case 2: { {
  nrglog('f', "RECALC(fn=" << "iso2/iso2-2ch-tripletp.dat" << ", Iop=" << Invar(1, 3) << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc<SC>> recalc_table = {
#include "iso2/iso2-2ch-tripletp.dat"
      };
      auto cn = this->recalc_general(diag, cold, I1, Ip, recalc_table, Invar(1, 3));
      if (cn) cnew[II] = *cn;
    }
  }
} } break;
  default: my_assert_not_reached();
  };

    Ip = Invar(ii1, ss1-2);
    switch (P.channels) {
  case 1: { {
  nrglog('f', "RECALC(fn=" << "iso2/iso2-1ch-tripletm.dat" << ", Iop=" << Invar(1, 3) << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc<SC>> recalc_table = {
#include "iso2/iso2-1ch-tripletm.dat"
      };
      auto cn = this->recalc_general(diag, cold, I1, Ip, recalc_table, Invar(1, 3));
      if (cn) cnew[II] = *cn;
    }
  }
} } break;
  case 2: { {
  nrglog('f', "RECALC(fn=" << "iso2/iso2-2ch-tripletm.dat" << ", Iop=" << Invar(1, 3) << ")");
  auto II = Twoinvar(I1, Ip);
  if (diag.count(I1) && diag.count(Ip)) {
    if (diag.at(I1).getnrstored() && diag.at(Ip).getnrstored()) {
      std::initializer_list<Recalc<SC>> recalc_table = {
#include "iso2/iso2-2ch-tripletm.dat"
      };
      auto cn = this->recalc_general(diag, cold, I1, Ip, recalc_table, Invar(1, 3));
      if (cn) cnew[II] = *cn;
    }
  }
} } break;
  default: my_assert_not_reached();
  };
  }
  return cnew;
}

}
