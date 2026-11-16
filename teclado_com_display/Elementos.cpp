#include "Elementos.h"

namespace elementos {

namespace {

// Índice 0 = número atômico 1
const char* const SIMBOLOS[TOTAL] = {
  "H",  "He", "Li", "Be", "B",  "C",  "N",  "O",  "F",  "Ne",  //  1-10
  "Na", "Mg", "Al", "Si", "P",  "S",  "Cl", "Ar", "K",  "Ca",  // 11-20
  "Sc", "Ti", "V",  "Cr", "Mn", "Fe", "Co", "Ni", "Cu", "Zn",  // 21-30
  "Ga", "Ge", "As", "Se", "Br", "Kr", "Rb", "Sr", "Y",  "Zr",  // 31-40
  "Nb", "Mo", "Tc", "Ru", "Rh", "Pd", "Ag", "Cd", "In", "Sn",  // 41-50
  "Sb", "Te", "I",  "Xe", "Cs", "Ba", "La", "Ce", "Pr", "Nd",  // 51-60
  "Pm", "Sm", "Eu", "Gd", "Tb", "Dy", "Ho", "Er", "Tm", "Yb",  // 61-70
  "Lu", "Hf", "Ta", "W",  "Re", "Os", "Ir", "Pt", "Au", "Hg",  // 71-80
  "Tl", "Pb", "Bi", "Po", "At", "Rn", "Fr", "Ra", "Ac", "Th",  // 81-90
  "Pa", "U",  "Np", "Pu", "Am", "Cm", "Bk", "Cf", "Es"         // 91-99
};

}  // namespace

const char* simbolo(uint8_t numeroAtomico) {
  if (numeroAtomico < 1 || numeroAtomico > TOTAL) return "?";
  return SIMBOLOS[numeroAtomico - 1];
}

}  // namespace elementos
