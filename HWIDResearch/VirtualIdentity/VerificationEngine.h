#pragma once

#include "IdentityProfile.h"
#include <string>
#include <vector>

enum class ObservationStatus { Alterado, NaoAlterado, NaoSuportado, VerificacaoFalhou };
struct IdentityObservation { std::string category; std::string name; std::string value; std::string source; bool supported = true; };
struct ComparisonRow { std::string category; std::string name; std::string previous; std::string expected; std::string observed; std::string source; ObservationStatus status; };

std::vector<ComparisonRow> compareIdentity(const IdentityProfile& expected, const std::vector<IdentityObservation>& before, const std::vector<IdentityObservation>& after);
const char* observationStatusText(ObservationStatus status);

