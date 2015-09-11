// This file was generated by Rcpp::compileAttributes
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

// readInput
List readInput(StringVector input);
RcppExport SEXP rcat_readInput(SEXP inputSEXP) {
BEGIN_RCPP
    Rcpp::RObject __result;
    Rcpp::RNGScope __rngScope;
    Rcpp::traits::input_parameter< StringVector >::type input(inputSEXP);
    __result = Rcpp::wrap(readInput(input));
    return __result;
END_RCPP
}
// run_cat2
List run_cat2(List input, CharacterVector filter);
RcppExport SEXP rcat_run_cat2(SEXP inputSEXP, SEXP filterSEXP) {
BEGIN_RCPP
    Rcpp::RObject __result;
    Rcpp::RNGScope __rngScope;
    Rcpp::traits::input_parameter< List >::type input(inputSEXP);
    Rcpp::traits::input_parameter< CharacterVector >::type filter(filterSEXP);
    __result = Rcpp::wrap(run_cat2(input, filter));
    return __result;
END_RCPP
}
// run_cat1
SEXP run_cat1(List params);
RcppExport SEXP rcat_run_cat1(SEXP paramsSEXP) {
BEGIN_RCPP
    Rcpp::RObject __result;
    Rcpp::RNGScope __rngScope;
    Rcpp::traits::input_parameter< List >::type params(paramsSEXP);
    __result = Rcpp::wrap(run_cat1(params));
    return __result;
END_RCPP
}
// run_cat0
StringVector run_cat0(CharacterVector input, CharacterVector output, CharacterVector filter);
RcppExport SEXP rcat_run_cat0(SEXP inputSEXP, SEXP outputSEXP, SEXP filterSEXP) {
BEGIN_RCPP
    Rcpp::RObject __result;
    Rcpp::RNGScope __rngScope;
    Rcpp::traits::input_parameter< CharacterVector >::type input(inputSEXP);
    Rcpp::traits::input_parameter< CharacterVector >::type output(outputSEXP);
    Rcpp::traits::input_parameter< CharacterVector >::type filter(filterSEXP);
    __result = Rcpp::wrap(run_cat0(input, output, filter));
    return __result;
END_RCPP
}
// read_cat_serieses
SEXP read_cat_serieses(StringVector filename);
RcppExport SEXP rcat_read_cat_serieses(SEXP filenameSEXP) {
BEGIN_RCPP
    Rcpp::RObject __result;
    Rcpp::RNGScope __rngScope;
    Rcpp::traits::input_parameter< StringVector >::type filename(filenameSEXP);
    __result = Rcpp::wrap(read_cat_serieses(filename));
    return __result;
END_RCPP
}
// write_cat_serieses
SEXP write_cat_serieses(StringVector filename, List data);
RcppExport SEXP rcat_write_cat_serieses(SEXP filenameSEXP, SEXP dataSEXP) {
BEGIN_RCPP
    Rcpp::RObject __result;
    Rcpp::RNGScope __rngScope;
    Rcpp::traits::input_parameter< StringVector >::type filename(filenameSEXP);
    Rcpp::traits::input_parameter< List >::type data(dataSEXP);
    __result = Rcpp::wrap(write_cat_serieses(filename, data));
    return __result;
END_RCPP
}
// rcat_load
SEXP rcat_load();
RcppExport SEXP rcat_rcat_load() {
BEGIN_RCPP
    Rcpp::RObject __result;
    Rcpp::RNGScope __rngScope;
    __result = Rcpp::wrap(rcat_load());
    return __result;
END_RCPP
}
