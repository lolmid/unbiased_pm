#include <RcppEigen.h>
using namespace Rcpp;
using namespace std;

// [[Rcpp::export]]
void levydriven_rtrans_(NumericMatrix & xparticles, const NumericVector & theta, const NumericVector & thetatransform){
  RNGScope scope;
  double theta2overtheta3 = thetatransform(0);
  double theta4theta2theta2overtheta3 = thetatransform(1);
  double expminustheta4 = thetatransform(2);
  double oneovertheta4 = thetatransform(3);
  int nparticles = xparticles.rows();
  NumericVector sum_weighted_e(nparticles);
  NumericVector sum_e(nparticles);
  NumericVector k = rpois(nparticles, theta4theta2theta2overtheta3);
  NumericVector c(100);
  NumericVector e(100);
  double new_z,new_v;
  for (int i = 0; i < nparticles; i++){
    if (k(i) > 100){
      k(i) = 100; // we say that in fact the poisson distribution is truncated at 100
    }
    c = runif(k(i));
    e = rexp(k(i), theta2overtheta3); // rcpp rexp is parametrized with rate
    sum_e(i) = sum(e);
    sum_weighted_e(i) = sum(exp(-theta(4) * c) * e);
    new_z = expminustheta4 * xparticles(i,1) + sum_weighted_e(i);
    new_v = oneovertheta4 * (xparticles(i,1) - new_z + sum_e(i));
    xparticles(i,0) = new_v;
    xparticles(i,1) = new_z;
  }
  // return(xparticles);
}

// /////////////////////////////////////////////////////////////////////////////////////////////////////////
// /////////////////////////////////////////       Single factor     ///////////////////////////////////////
// /////////////////////////////////////////////////////////////////////////////////////////////////////////
// // Levy-driven initial draw for the latent states in the single factor case
// // [[Rcpp::export]]
// NumericMatrix rinitial_SVLevy_cpp(int N, double t, double xi,
//                                   double w2, double lambda){
//   RNGScope scope;
//   NumericMatrix X(N, 2);
//   // Note: Rcpp's rgamma takes shape and SCALE (1/rate) parameter
//   NumericVector z0 = rgamma(N,pow(xi,2)/w2,w2/xi);
//   NumericVector k = rpois(N,lambda*pow(xi,2)/w2);
//   for (int ix = 0; ix < N; ix ++){
//     NumericVector c1k = runif(k(ix),0,t);
//     NumericVector e1k = rexp(k(ix),xi/w2);
//     X(ix, 1) = exp(-lambda)*z0(ix) + sum(exp(-lambda*(t-c1k))*e1k);
//     X(ix, 0) = (1/lambda)*(z0(ix) - X(ix,1) + sum(e1k));
//   }
//   return X;
// }
// // Levy-driven transition for the latent states (dimX by Nx matrix) in the single factor case
// // [[Rcpp::export]]
// NumericMatrix rtransition_SVLevy_cpp(NumericMatrix Xold, double t_1, double t,
//                                      double xi, double w2, double lambda){
//   RNGScope scope;
//   int N = Xold.nrow();
//   NumericMatrix Xnew(N, 2);
//   NumericVector k = rpois(N,lambda*pow(xi,2)/w2);
//   for (int ix = 0; ix < N; ix ++){
//     NumericVector c1k = runif(k(ix),t_1,t);
//     NumericVector e1k = rexp(k(ix),xi/w2);
//     Xnew(ix,1) = exp(-lambda)*Xold(ix,1) + sum(exp(-lambda*(t-c1k))*e1k);
//     Xnew(ix,0) = (1/lambda)*(Xold(ix,1) - Xnew(ix,1) + sum(e1k));
//   }
//   return Xnew;
// }
// // Density evaluation, taking Yt as a (necessarily 1 by 1) vector in the single factor case
// // [[Rcpp::export]]
// NumericMatrix dobs_SVLevy_cpp(NumericVector Yt, NumericMatrix Xts,
//                               double mu, double beta, bool log){
//   int N = Xts.nrow();
//   NumericMatrix dobs(1,N);
//   for (int ix = 0; ix < N; ix ++){
//     dobs(_,ix) = dnorm(Yt, mu + beta*Xts(ix,0), sqrt(Xts(ix,0)), log);
//   }
//   return dobs;
// }
// // First derivative evaluation of dobs in the single factor case
// // [[Rcpp::export]]
// NumericMatrix d1logdobs_SVLevy_cpp(NumericVector Yt, NumericMatrix Xts,
//                                    double mu, double beta){
//   int N = Xts.ncol();
//   NumericMatrix d1(N,1);
//   for (int ix = 0; ix < N; ix ++){
//     d1(ix,_) = (mu + beta*Xts(0,ix) - Yt)/Xts(0,ix);
//   }
//   return d1;
// }
// // Second derivative evaluation of dobs in the single factor case
// // [[Rcpp::export]]
// NumericMatrix d2logdobs_SVLevy_cpp(NumericVector Yt, NumericMatrix Xts,
//                                    double mu, double beta){
//   int N = Xts.ncol();
//   NumericMatrix d2(N,1);
//   for (int ix = 0; ix < N; ix ++){
//     d2(ix,0) = -1/Xts(0,ix);
//   }
//   return d2;
// }
// /////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////       Multi-factor no leverage      ////////////////////////////////////
// /////////////////////////////////////////////////////////////////////////////////////////////////////////
// // Levy-driven initial draw for the latent states in the multi-factor case without leverage
// // [[Rcpp::export]]
// NumericMatrix rinitial_SVLevy_multifactor_cpp(int N, double t, double xi,
//                                               double w2,
//                                               double lambda1, double lambda2,
//                                               double w){
//   RNGScope scope;
//   NumericMatrix X(4, N);
//   double xi1 = w*xi;
//   double w21 = w*w2;
//   double xi2 = (1-w)*xi;
//   double w22 = (1-w)*w2;
//   // Note: Rcpp's rgamma takes shape and SCALE (1/rate) parameter
//   NumericVector z01 = rgamma(N,pow(xi1,2)/w21,w21/xi1);
//   NumericVector z02 = rgamma(N,pow(xi2,2)/w22,w22/xi2);
//   NumericVector k1 = rpois(N,lambda1*pow(xi1,2)/w21);
//   NumericVector k2 = rpois(N,lambda2*pow(xi2,2)/w22);
//   for (int ix = 0; ix < N; ix ++){
//     // First factor
//     NumericVector c1k1 = runif(k1(ix),0,t);
//     NumericVector e1k1 = rexp(k1(ix),xi1/w21);
//     X(1,ix) = exp(-lambda1)*z01(ix) + sum(exp(-lambda1*(t-c1k1))*e1k1);
//     X(0,ix) = (1/lambda1)*(z01(ix) - X(1,ix) + sum(e1k1));
//     // Second factor
//     NumericVector c1k2 = runif(k2(ix),0,t);
//     NumericVector e1k2 = rexp(k2(ix),xi2/w22);
//     X(3,ix) = exp(-lambda2)*z02(ix) + sum(exp(-lambda2*(t-c1k2))*e1k2);
//     X(2,ix) = (1/lambda2)*(z02(ix) - X(3,ix) + sum(e1k2));
//   }
//   return X;
// }
// // Levy-driven transition for the latent states (dimX by Nx matrix) in the multi-factor case without leverage
// // [[Rcpp::export]]
// NumericMatrix rtransition_SVLevy_multifactor_cpp(NumericMatrix Xold, double t_1,
//                                                  double t,
//                                                  double xi, double w2,
//                                                  double lambda1, double lambda2,
//                                                  double w){
//   RNGScope scope;
//   int N = Xold.ncol();
//   double xi1 = w*xi;
//   double w21 = w*w2;
//   double xi2 = (1-w)*xi;
//   double w22 = (1-w)*w2;
//   NumericMatrix Xnew(4, N);
//   NumericVector k1 = rpois(N,lambda1*pow(xi1,2)/w21);
//   NumericVector k2 = rpois(N,lambda2*pow(xi2,2)/w22);
//   for (int ix = 0; ix < N; ix ++){
//     // First factor
//     NumericVector c1k1 = runif(k1(ix),t_1,t);
//     NumericVector e1k1 = rexp(k1(ix),xi1/w21);
//     Xnew(1,ix) = exp(-lambda1)*Xold(1,ix) + sum(exp(-lambda1*(t-c1k1))*e1k1);
//     Xnew(0,ix) = (1/lambda1)*(Xold(1,ix) - Xnew(1,ix) + sum(e1k1));
//     // Second factor
//     NumericVector c1k2 = runif(k2(ix),t_1,t);
//     NumericVector e1k2 = rexp(k2(ix),xi2/w22);
//     Xnew(3,ix) = exp(-lambda2)*Xold(3,ix) + sum(exp(-lambda2*(t-c1k2))*e1k2);
//     Xnew(2,ix) = (1/lambda2)*(Xold(3,ix) - Xnew(3,ix) + sum(e1k2));
//   }
//   return Xnew;
// }
// // Density evaluation, taking Yt as a (necessarily 1 by 1) vector in the multi-factor case without leverage
// // [[Rcpp::export]]
// NumericMatrix dobs_SVLevy_multifactor_cpp(NumericVector Yt, NumericMatrix Xts,
//                                           double mu, double beta, bool log){
//   int N = Xts.ncol();
//   NumericMatrix dobs(1,N);
//   for (int ix = 0; ix < N; ix ++){
//     double vt = Xts(0,ix)+Xts(2,ix);
//     dobs(_,ix) = dnorm(Yt, mu + beta*vt, sqrt(vt), log);
//   }
//   return dobs;
// }
// // First derivative evaluation of dobs in the multi-factor case without leverage
// // [[Rcpp::export]]
// NumericMatrix d1logdobs_SVLevy_multifactor_cpp(NumericVector Yt, NumericMatrix Xts,
//                                                double mu, double beta){
//   int N = Xts.ncol();
//   NumericMatrix d1(N,1);
//   for (int ix = 0; ix < N; ix ++){
//     double vt = Xts(0,ix)+Xts(2,ix);
//     d1(ix,_) = (mu + beta*vt - Yt)/vt;
//   }
//   return d1;
// }
// // Second derivative evaluation of dobs in the multi-factor case without leverage
// // [[Rcpp::export]]
// NumericMatrix d2logdobs_SVLevy_multifactor_cpp(NumericVector Yt, NumericMatrix Xts,
//                                                double mu, double beta){
//   int N = Xts.ncol();
//   NumericMatrix d2(N,1);
//   for (int ix = 0; ix < N; ix ++){
//     double vt = Xts(0,ix)+Xts(2,ix);
//     d2(ix,0) = -1/vt;
//   }
//   return d2;
// }
// /////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////       Multi-factor with leverage     ///////////////////////////////////
// /////////////////////////////////////////////////////////////////////////////////////////////////////////
// // Levy-driven initial draw for the latent states in the multi-factor case with leverage
// // [[Rcpp::export]]
// NumericMatrix rinitial_SVLevy_multifactorleverage_cpp(int N, double t, double xi,
//                                                       double w2, double lambda1,
//                                                       double lambda2, double w){
//   RNGScope scope;
//   NumericMatrix X(6, N);
//   double xi1 = w*xi;
//   double w21 = w*w2;
//   double xi2 = (1-w)*xi;
//   double w22 = (1-w)*w2;
//   // Note: Rcpp's rgamma takes shape and SCALE (1/rate) parameter
//   NumericVector z01 = rgamma(N,pow(xi1,2)/w21,w21/xi1);
//   NumericVector z02 = rgamma(N,pow(xi2,2)/w22,w22/xi2);
//   NumericVector k1 = rpois(N,lambda1*pow(xi1,2)/w21);
//   NumericVector k2 = rpois(N,lambda2*pow(xi2,2)/w22);
//   for (int ix = 0; ix < N; ix ++){
//     // First factor
//     NumericVector c1k1 = runif(k1(ix),0,t);
//     NumericVector e1k1 = rexp(k1(ix),xi1/w21);
//     X(4,ix) = sum(e1k1);
//     X(1,ix) = exp(-lambda1)*z01(ix) + sum(exp(-lambda1*(t-c1k1))*e1k1);
//     X(0,ix) = (1/lambda1)*(z01(ix) - X(1,ix) + X(4,ix));
//     // Second factor
//     NumericVector c1k2 = runif(k2(ix),0,t);
//     NumericVector e1k2 = rexp(k2(ix),xi2/w22);
//     X(5,ix) = sum(e1k2);
//     X(3,ix) = exp(-lambda2)*z02(ix) + sum(exp(-lambda2*(t-c1k2))*e1k2);
//     X(2,ix) = (1/lambda2)*(z02(ix) - X(3,ix) + X(5,ix));
//   }
//   return X;
// }
// // Levy-driven transition for the latent states (dimX by Nx matrix) in the multi-factor case with leverage
// // [[Rcpp::export]]
// NumericMatrix rtransition_SVLevy_multifactorleverage_cpp(NumericMatrix Xold, double t_1,
//                                                          double t, double xi,
//                                                          double w2,
//                                                          double lambda1, double lambda2,
//                                                          double w){
//   RNGScope scope;
//   int N = Xold.ncol();
//   double xi1 = w*xi;
//   double w21 = w*w2;
//   double xi2 = (1-w)*xi;
//   double w22 = (1-w)*w2;
//   NumericMatrix Xnew(6, N);
//   NumericVector k1 = rpois(N,lambda1*pow(xi1,2)/w21);
//   NumericVector k2 = rpois(N,lambda2*pow(xi2,2)/w22);
//   for (int ix = 0; ix < N; ix ++){
//     // First factor
//     NumericVector c1k1 = runif(k1(ix),t_1,t);
//     NumericVector e1k1 = rexp(k1(ix),xi1/w21);
//     Xnew(4,ix) = sum(e1k1);
//     Xnew(1,ix) = exp(-lambda1)*Xold(1,ix) + sum(exp(-lambda1*(t-c1k1))*e1k1);
//     Xnew(0,ix) = (1/lambda1)*(Xold(1,ix) - Xnew(1,ix) + Xnew(4,ix));
//     // Second factor
//     NumericVector c1k2 = runif(k2(ix),t_1,t);
//     NumericVector e1k2 = rexp(k2(ix),xi2/w22);
//     Xnew(5,ix) = sum(e1k2);
//     Xnew(3,ix) = exp(-lambda2)*Xold(3,ix) + sum(exp(-lambda2*(t-c1k2))*e1k2);
//     Xnew(2,ix) = (1/lambda2)*(Xold(3,ix) - Xnew(3,ix) + Xnew(5,ix));
//   }
//   return Xnew;
// }
// // Density evaluation, taking Yt as a (necessarily 1 by 1) vector in the multi-factor case with leverage
// // [[Rcpp::export]]
// NumericMatrix dobs_SVLevy_multifactorleverage_cpp(NumericVector Yt, NumericMatrix Xts,
//                                                   double mu, double beta, double xi,
//                                                   double lambda1, double lambda2,
//                                                   double w, double rho1, double rho2,
//                                                   bool log){
//   int N = Xts.ncol();
//   NumericMatrix dobs(1,N);
//   for (int ix = 0; ix < N; ix ++){
//     double vt = Xts(0,ix)+Xts(2,ix);
//     dobs(_,ix) = dnorm(Yt, mu + beta*vt + rho1*Xts(4,ix) + rho2*Xts(5,ix) - xi*(w*rho1*lambda1+(1-w)*rho2*lambda2), sqrt(vt), log);
//   }
//   return dobs;
// }
// // First derivative evaluation of dobs in the multi-factor case with leverage
// // [[Rcpp::export]]
// NumericMatrix d1logdobs_SVLevy_multifactorleverage_cpp(NumericVector Yt, NumericMatrix Xts,
//                                                        double mu, double beta,  double xi,
//                                                        double lambda1, double lambda2, double w,
//                                                        double rho1, double rho2){
//   int N = Xts.ncol();
//   NumericMatrix d1(N,1);
//   for (int ix = 0; ix < N; ix ++){
//     double vt = Xts(0,ix)+Xts(2,ix);
//     d1(ix,_) = (mu + beta*vt + rho1*Xts(4,ix) + rho2*Xts(5,ix) - xi*(w*rho1*lambda1+(1-w)*rho2*lambda2) - Yt)/vt;
//   }
//   return d1;
// }
// // Second derivative evaluation of dobs in the multi-factor case with leverage
// // [[Rcpp::export]]
// NumericMatrix d2logdobs_SVLevy_multifactorleverage_cpp(NumericVector Yt, NumericMatrix Xts,
//                                                        double mu, double beta){
//   return d2logdobs_SVLevy_multifactor_cpp(Yt, Xts, mu, beta);
// }
//
