// Parsing a JSON number that reader.kf has already checked against JSON's
// grammar, so strtod sees nothing it would read differently.

#include <stdlib.h>

double kf_json_parse_float64(const char* text) {
    return strtod(text, NULL);
}
