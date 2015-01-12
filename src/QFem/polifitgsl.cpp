#include "polifitgsl.h"
#include <gsl/gsl_multifit.h>
#include <stdbool.h>
#include <math.h>
#include <gsl/gsl_matrix.h>
bool polynomialfit(int obs, int degree,
           std::vector<double> dx, std::vector<double> dy, std::vector<double> &store) /* n, p */
{
  gsl_multifit_linear_workspace *ws;
  gsl_matrix *cov, *X;
  gsl_vector *y, *c;
  double chisq;

  int i, j;

  while (obs<degree+1)
  {
      std::vector<double> dxNew, dyNew;
      for (int i = 0; i < (int)dx.size(); i++)
      {
          double newXPoint,newYPoint;
          if (i < (int)dx.size()-1)
             {
                 newXPoint = (dx.at(i)+dx.at(i+1))/2;
                 newYPoint = (dy.at(i)+dy.at(i+1))/2;
             }
         dxNew.push_back(dx.at(i));
         dyNew.push_back(dy.at(i));
         if (i < (int)dx.size()-1)
            {
                 dxNew.push_back(newXPoint);
                 dyNew.push_back(newYPoint);
             }
      }
      dx = dxNew;
      dy = dyNew;
      dxNew.clear();
      dyNew.clear();
      obs = dx.size();
  }

  X = gsl_matrix_alloc(obs, degree);
  y = gsl_vector_alloc(obs);
  c = gsl_vector_alloc(degree);
  cov = gsl_matrix_alloc(degree, degree);

  for(i=0; i < obs; i++) {
    gsl_matrix_set(X, i, 0, 1.0);
    for(j=0; j < degree; j++) {
        if(j<2){
            gsl_matrix_set(X, i, j, 0);
        }
        else{
      gsl_matrix_set(X, i, j, pow(dx.at(i), j));
    }
    }
    gsl_vector_set(y, i, dy.at(i));
  }

  ws = gsl_multifit_linear_alloc(obs, degree);
  gsl_multifit_linear(X, y, c, cov, &chisq, ws);

  /* store result ... */
  for(i=0; i < degree; i++)
  {
      store.push_back(gsl_vector_get(c, i));
  }

  gsl_multifit_linear_free(ws);
  gsl_matrix_free(X);
  gsl_matrix_free(cov);
  gsl_vector_free(y);
  gsl_vector_free(c);
  return true; /* we do not "analyse" the result (cov matrix mainly)
          to know if the fit is "good" */
}
