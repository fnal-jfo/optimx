//  =================================================================
//
//  OptimCL.cpp
//
//  This file is part of OptiMX, an interactive tool  
//  for beam optics design and analysis. 
//
//  Copyright (c) 2025 Fermi Forward Discovery Group, LLC.
//  This material was produced under U.S. Government contract
//  89243024CSC000002 for Fermi National Accelerator Laboratory (Fermilab),
//  which is operated by Fermi Forward Discovery Group, LLC for the
//  U.S. Department of Energy. The U.S. Government has rights to use,
//  reproduce, and distribute this software.
//
//  NEITHER THE GOVERNMENT NOR FERMI FORWARD DISCOVERY GROUP, LLC
//  MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR ASSUMES ANY
//  LIABILITY FOR THE USE OF THIS SOFTWARE.
//
//  If software is modified to produce derivative works, such modified
//  software should be clearly marked, so as not to confuse it with the
//  version available from Fermilab.
//
//  Additionally, this program is free software; you can redistribute
//  it and/or modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version. Accordingly,
//  this program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
//  See the GNU General Public License for more details.
//
//  https://www.gnu.org/licenses/old-licenses/gpl-2.0.html
//  https://www.gnu.org/licenses/gpl-3.0.html
//
//  =================================================================
//

int OptimmainWindow::TrackOffLine(char *InputPartPosFile, char *TrackResFile,
				  bool MatchCase, char *filter, int nturn, char ring)
{
  int i, j, k, N;
  char nm, buf[LSTR+1];
  double dat[8], Enr, EnrNew;
  std::complex<double> ev[4][4];
  RMatrix me, tm;
  Element *ep;
  Twiss twiss;
  Coordinates *v;
  BeamParamStruct history;

  FILE* fpr=fopen(InputPartPosFile,"r");
  FILE* fpw=fopen(TrackResFile,"w");
  
  if(!fpr){
    sprintf(buf, "Cannot open file %s to read particle coordinates",InputPartPosFile);
    OptimWarningMessage(this, "Tracking", buf, QMessageBox::Ok);
    return 1;
  }
  if(!fpw){
    sprintf(buf, "Cannot open file %s to write trackong results",TrackResFile);
    OptimWarningMessage(this, "Tracking", buf, QMessageBox::Ok);
    return 1;
  }
  
  if(analyzed_) {if(analyze(false,1)) return 1;} else {if(analyze(true,1))return 1;}
   // Read particle coordinates
   fgets(buf, 255, fpr);
   if(!strcmpr("OptiM Track Data", buf)){
     sprintf(buf, "File <%s> is not OptiM Track Data file",InputPartPosFile);
     OptimWarningMessage(this, "Tracking", buf, QMessageBox::Ok);
     return 1;
   }
   i=0;
   while(fgets(buf, 255, fpr) ){
     if(buf[0]=='#') continue;
     if(DecodeExtLine(buf, dat, 6)==6) ++i;
   }
   fclose(fpr);
   if(i<1){
     sprintf(buf, "File %s has no particle information or corrapted structure",InputPartPosFile);
     OptimWarningMessage(this,"Tracking", buf,QMessageBox::Ok);
     return 1;
   }
   N=i;

   OptimTracker* tracker = 0;
   QMdiSubWindow* sw = mdiArea->addSubWindow( (tracker = new OptimTracker(this)) ); 

   tracker->N_=N;
   tracker->vin_  = new Coordinates[i];
   tracker->vfin_ = new Coordinates[i];
   tracker->loss_ = new LossStruct[i];
   fpr=fopen(InputPartPosFile,"r");
   fgets(buf, 255, fpr);
   i=0;
   while(fgets(buf, 255, fpr) ){
   if(buf[0]=='#') continue;
      if((k=DecodeExtLine(buf, dat, 8))>=6){
      	for(j=0; j<6; j++) tracker->vin_[i].c[j]=dat[j];
	tracker->loss_[i].lost=0;
   	if(k>=7)tracker->loss_[i].PartNumber = dat[6];  else tracker->loss_[i].PartNumber= i;
      	if(k>=8)tracker->loss_[i].Weight     = dat[7];  else tracker->loss_[i].Weight    = 1.;
        ++i;
      }
   }
   fclose(fpr);

   // performs tracking
   double tetaY = tetaYo0;
   double Enr   = Ein;
   double gamma = 1.+Enr/ms;
   double dalfa = 0.0; 

   for(j=0; j<N; ++j) { 
      for(i=0; i<6; ++i) tracker->vfin_[j].c[i]=tracker->vin_[j].c[i];
       v = tracker->vfin_;
       twiss.BtX  = BetaXin;      twiss.BtY=BetaYin;
       twiss.AlX  = AlfaXin;      twiss.AlY=AlfaYin;
       twiss.DsX  = DispXin;      twiss.DsY=DispYin;
       twiss.DsXp = DispPrimeXin; twiss.DsYp=DispPrimeYin;
   }   
   if(ring == 'R' ){
      FindRMatrix(&tm);
      i = find_tunes(tm, 100., &twiss, &dalfa);
      if(i){
	OptimWarningMessage(this, "Tracking", "Cannot close for X or Y", QMessageBox::Ok);
      	return 1;
      }
   }
   twiss.nuX=0.;			 
   twiss.nuY=0.;
   GetEigenVecFrom2DTwiss(&twiss, ev);
   tracker->GetHistory(&history, gamma, &twiss, v);
   history.s=0.;
   fprintf(fpw, "#S[cm]_or_N \tname \temitX[cm] \temitY[cm] \tXmax[cm] \tYmax[cm] \tsigmaS[cm] \tcurrent[cm] \tXav[cm] \tYav[cm] \tSav[cm] \tPXav \tPYav \tPSav");
   fprintf(fpw, "\n%f \t%s \t%e \t%e \t%e \t%e \t%e \t%e \t%e \t%e \t%e \t%e \t%e \t%e",
   history.s, "Start", history.emitX, history.emitY, history.Xmax, history.Ymax, history.sigmaDP,
   history.current, history.Xav, history.Yav, history.Sav, history.PXav, history.PYav, history.PSav);
   
   for(k=0; k<nturn; k++){
      for(i=0; i<nelm_; i++){
   	ep = beamline_[i];
      	nm = ep->ename();
        EnrNew=Enr;
        el_to_matr(&EnrNew, ms, &tetaY, ep, &me,0.,3);
        BtfTransform(&me, &twiss, ev);
        tracker->TrackParticleExact(ep, Enr, tetaY, v, N, k+1, i);
        switch(nm){
          case 'E': 
          case 'X': 
          case 'A': 
          case 'W':
            Enr   = EnrNew;
   	    gamma = 1.+Enr/ms;
          default:
          break;
        }
        if(nturn==1)history.s=history.s+ep->L*0.01; else history.s=k;
      	if(filterName(ep->name, filter, MatchCase)){
          tracker->GetHistory(&history, gamma, &twiss, v);
	  fprintf(fpw, "\n%f \t%s \t%e \t%e \t%e \t%e \t%e \t%e \t%e \t%e \t%e \t%e \t%e \t%e",
   		       history.s, ep->name, history.emitX, history.emitY, history.Xmax, history.Ymax, history.sigmaDP,
                       history.current, history.Xav, history.Yav, history.Sav, history.PXav, history.PYav, history.PSav);
        }
     }
   }
   fclose(fpw);
   return 0;
}
