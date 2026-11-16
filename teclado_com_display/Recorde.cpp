#include "Recorde.h"

#include <Preferences.h>

namespace recorde {

namespace {

constexpr const char* NAMESPACE = "senha_quimica";
constexpr const char* CHAVE     = "recorde";

}  // namespace

uint32_t carregar() {
  Preferences preferencias;
  preferencias.begin(NAMESPACE, /*readOnly=*/true);
  const uint32_t valor = preferencias.getUInt(CHAVE, 0);
  preferencias.end();
  return valor;
}

bool registrar(uint32_t pontos) {
  if (pontos <= carregar()) return false;

  Preferences preferencias;
  preferencias.begin(NAMESPACE, /*readOnly=*/false);
  preferencias.putUInt(CHAVE, pontos);
  preferencias.end();
  return true;
}

}  // namespace recorde
