
; import/overload stubs for the VIC20 kernal floating point implementation

    .import ___cbmkernal_afloat
    .import ___cbmkernal_aufloat
    .import ___cbmkernal_axfloat
    .import ___cbmkernal_axufloat
    .import ___cbmkernal_eaxfloat
    .import ___cbmkernal_eaxufloat
    .import ___cbmkernal_feaxint
    .import ___cbmkernal_feaxlong
    .import ___cbmkernal_fbnegeax
    .import ___cbmkernal_ftosaddeax
    .import ___cbmkernal_ftossubeax
    .import ___cbmkernal_ftosdiveax
    .import ___cbmkernal_ftosmuleax
    .import ___cbmkernal_ftoseqeax
    .import ___cbmkernal_ftosneeax
    .import ___cbmkernal_ftosgteax
    .import ___cbmkernal_ftoslteax
    .import ___cbmkernal_ftosgeeax
    .import ___cbmkernal_ftosleeax

    .export afloat      := ___cbmkernal_afloat
    .export aufloat     := ___cbmkernal_aufloat
    .export axfloat     := ___cbmkernal_axfloat
    .export axufloat    := ___cbmkernal_axufloat
    .export eaxfloat    := ___cbmkernal_eaxfloat
    .export eaxufloat   := ___cbmkernal_eaxufloat
    .export feaxint     := ___cbmkernal_feaxint
    .export feaxlong    := ___cbmkernal_feaxlong
    .export fbnegeax    := ___cbmkernal_fbnegeax
    .export ftosaddeax  := ___cbmkernal_ftosaddeax
    .export ftossubeax  := ___cbmkernal_ftossubeax
    .export ftosdiveax  := ___cbmkernal_ftosdiveax
    .export ftosmuleax  := ___cbmkernal_ftosmuleax
    .export ftoseqeax   := ___cbmkernal_ftoseqeax
    .export ftosneeax   := ___cbmkernal_ftosneeax
    .export ftosgteax   := ___cbmkernal_ftosgteax
    .export ftoslteax   := ___cbmkernal_ftoslteax
    .export ftosgeeax   := ___cbmkernal_ftosgeeax
    .export ftosleeax   := ___cbmkernal_ftosleeax

    .import ___cbmkernal_powf
    .import ___cbmkernal_sinf
    .import ___cbmkernal_cosf
    .import ___cbmkernal_logf
    .import ___cbmkernal_expf
    .import ___cbmkernal_sqrtf
    .import ___cbmkernal_tanf
    .import ___cbmkernal_atanf
    .import ___cbmkernal_fabsf
    .import ___cbmkernal_roundf
    .import ___cbmkernal_truncf

    .export _powf       := ___cbmkernal_powf
    .export _sinf       := ___cbmkernal_sinf
    .export _cosf       := ___cbmkernal_cosf
    .export _logf       := ___cbmkernal_logf
    .export _expf       := ___cbmkernal_expf
    .export _sqrtf      := ___cbmkernal_sqrtf
    .export _tanf       := ___cbmkernal_tanf
    .export _atanf      := ___cbmkernal_atanf
    .export _fabsf      := ___cbmkernal_fabsf
    .export _roundf     := ___cbmkernal_roundf
    .export _truncf     := ___cbmkernal_truncf
