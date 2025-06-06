//  =================================================================
//
//  OptimCommandLine.cpp
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

#include <OptimMessages.h>
#include <OptimApp.h>
#include <OptimMainWindow.h>
#include <OptimMdiArea.h>
#include <OptimStateMachine.h>
#include <OptimTrackerNew.h>
#include <OptimEditor.h>
#include <BeamMoments.h>
#include <Bunch.h>
#include <Utility.h>
#include <Twiss.h>
#include <Element.h>
#include <RMatrix.h>
#include <OptimCalc.h>
#include <Constants.h>
#include <ParticleTrackingDialog.h>
#include <OrbitDialog.h>
#include <EditReadDialog.h>
#include <Globals.h>

#include <QMdiSubWindow>
#include <QMdiArea>
#include <QTextBlock>
#include <QTimer>

#include <ctime>

#include <optionparser.h>

#undef emit
#include <sqlite/execute.hpp>
#include <sqlite/private/private_accessor.hpp>
#include <sqlite/query.hpp>
#include <sqlite3.h>

using Utility::decodeExtLine;
using Utility::filterName;
using Utility::strcmpr;
using Utility::GetUserNameLocal;
using Constants::PI;

#define LSTR 1024

enum  optionIndex { UNKNOWN, TWISS, TWISSMAD, COUPLEDTWISS, COUPLEDTWISSSC, CENTROID, ORBIT, READBDL,  WRITESNAP, MULTIPTRACK, HELP };

const option::Descriptor usage[] =
 {
  {TWISS,          0, "f", "twiss-table",       option::Arg::None,  "-f, --twiss-table:      save a table of Twiss functions (beta-functions, dispersions etc.)"},  
  {TWISSMAD,       0, "s", "twiss-table-mad",   option::Arg::None,  "-s, --twiss-table-mad:  save a table of Twiss functions to a file with MAD like structure."},
  {COUPLEDTWISS,   0, "c", "coupled-twiss",     option::Arg::None,  "-c, --coupled-twiss:    save a table of coupled Twiss functions."},  
  {COUPLEDTWISSSC, 0, "i", "coupled-twiss-sc",  option::Arg::None, " -i, --coupled-twiss-sc: save a table of Twiss functions computed with accounting beam space charge."},
  {CENTROID,       0, "t", "centroid",          option::Arg::None,  "-t, --centroid:         save a table of beam centroid positions (trajectory)."}, 
  {ORBIT,          0, "o", "orbit",             option::Arg::None,  "-o, --orbit:            save a table of beam Orbit positions."},  
  {READBDL,        0, "r", "bdl",               option::Arg::None,  "-r, --bdl:              read BdL from .snap file and create new OptiM file."},
  {WRITESNAP,      0, "w", "write-snap",        option::Arg::None,  "-w, --snap:             create a .snap file."}, 
  {MULTIPTRACK,    0, "p", "--track",           option::Arg::None,  "-p, --track:            multi-particle tracking."},
  {HELP,           0, "-h" , "help",            option::Arg::None,  "-h  --help              Print usage and exit." },
  {0,0,0,0,0,0}
 };

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool OptimMainWindow::commandLine(int& argc, char** argv)
{

  /******* 
  
bool adjustargc =(argc>0);  
     
   argc-=(argc>0); argv+=(argc>0); // skip program name argv[0] if present
   option::Stats  stats(usage, argc, argv);
   option::Option options[stats.options_max], buffer[stats.buffer_max];
   option::Parser parse(usage, argc, argv, options, buffer);
   

   if (parse.error())
     exit(1);

   if (options[HELP] ) {
     option::printUsage(std::cout, usage);
     exit(0);
  }

  argc += adjustargc ? 1:0 ; argv -= adjustargc ? 1:0;
  
  ******/
  
  bool& guimode = qobject_cast<OptimApp*>(qApp)->guimode_;

  char buf[256], comment[128], buf1[80], ring, filter[32];
  int i, nln;
  unsigned long len;
  double s;
  Twiss v;
  bool ClsLat, mtcs;
  FunctionDlgStruct NStf;
  
  int ierr = 0; // error status  

  OptimEditor* editor = 0;

  // Command line argument analysis

  guimode = true;

  if( argc == 1 ) { return true;}
  
  FILE* fp = 0;
  if( argc == 2) {
   fp = fopen(argv[1], "r");
   std::shared_ptr<FILE> sfp(fp, std::fclose);
   if(!fp) {
     sprintf(buf,"\"%s\" \"%s\" ", argv[0], argv[1]);
     OptimMessageBox::warning(this, "Incorrect command line", buf, QMessageBox::Ok);
     exit(1);
   }
   //fclose(fp);
   this->open(argv[1]);
   QTimer::singleShot(0, this, SLOT(stateLatticeLoaded())); 
   QTimer::singleShot(0, this, SLOT(resizeLatticeWindow())); 
   return true;
  } //  argc == 2 

if( argc >=4 ){
   fp = fopen( argv[2], "r");
   std::shared_ptr<FILE> sfp(fp,std::fclose);
   if( (argv[1][0] != '-') || (!fp) ) {
swerr:     	
     buf[0]=0;
   for(i=0; i<argc; ++i) {
     strcat(buf, argv[i]);
     strcat(buf, " ");}
     OptimMessageBox::warning(this, "Incorrect command line",buf, QMessageBox::Ok);
     exit(1);
   }

   //fclose(fp);
   this->open( argv[2]); 
   fp = fopen( argv[3], "w");
   sfp.reset(fp,std::fclose);
      
   if (!fp) goto swerr;

   guimode = false;
   
   time_t t;
   tm* tm_str;

   switch ( toupper(argv[1][1]) ){
     // Multiparticle tracking
     case 'P':
       //fclose(fp);
       if(argc >= 5) strcpy(filter, argv[5]); else strcpy(filter, "*");
       if(argc >= 6) { mtcs = (argv[6][0] == 'Y'); } else { argv[6][0] ='Y';}
       if(argc >= 7) i    = atoi(argv[7]);  else i   = 1;
       if(argc >= 8) ring = argv[8][0];     else ring='L';
       ierr = TrackOffLine( argv[4], argv[3], mtcs, filter, i, ring);
       exit(ierr);
       // Print twiss functions into a file
     case 'F':  
     case 'S':
       //fclose(fp);
       strcpy(NStf.Filter, "*");
       if (argc>=5) strcpy(NStf.Filter, argv[4]);
       NStf.MatchCase    = true;
       if( argc>=6) {if( toupper(argv[5][0]) == 'N') NStf.MatchCase    = false; }
       NStf.NoFirstLetter = true;
       if( argc>=7) {if( toupper(argv[6][0]) == 'N') NStf.NoFirstLetter= false; }
        
       strcpy(comment, "#");
       if( argc>=8) strcpy(comment, argv[7]);
       NStf.Step = 0;
       if( argc>=9) NStf.Step = atof(argv[8]);
       ClsLat =  CtSt_.IsRingCh  = false;
        
       if( argc>= 10 )  ClsLat =  CtSt_.IsRingCh  = (toupper(argv[9][0]) == 'Y');
       NStf.Start       = 0;
       NStf.Stop        = 0;
       NStf.printname   = true;
       NStf.PrintEnerg  = true;
       NStf.StartSfrom0 = false;

       ierr = ( toupper(argv[1][1]) =='F') ? ViewFunctions(argv[3], &NStf, comment, ClsLat) : ViewMachine(argv[3], &NStf, comment,ClsLat);

       exit(ierr);
       // Print coupled twiss functions into a file
    case 'C':
      //fclose(fp);
       strcpy(NStf.Filter, "*");
       if( argc>=5) strcpy(NStf.Filter, argv[4]);
       NStf.MatchCase = true;
       if( argc>=6) { if( toupper(argv[5][0]) == 'N') NStf.MatchCase=false;     }
       NStf.NoFirstLetter=true;
       if( argc>=7) { if( toupper(argv[6][0]) == 'N') NStf.NoFirstLetter=false; }
       NStf.Step = 0.0;
       if( argc>=8) NStf.Step = atof(argv[7]);
       CtSt_.IsRingCh = false;
       if( argc>=9) { if( toupper(argv[8][0]) == 'Y') CtSt_.IsRingCh = true; }
       NStf.Start = 0;
       NStf.Stop  = 0;
       NStf.printname   = true;
       NStf.PrintEnerg  = true;
       NStf.StartSfrom0 = false;
       ierr = ViewFunctions4D( argv[3], &NStf);
       exit(ierr);
       // Print coupled twiss functions into a file. Cpace charge effects are taken into account
     case 'I':
       //fclose(fp);
       strcpy(NStf.Filter, "*");
       if(argc>=5) strcpy(NStf.Filter, argv[4]);
       NStf.MatchCase     = true;
       if( argc>=6){ if( toupper(argv[5][0]) == 'N') NStf.MatchCase=false;}
       NStf.NoFirstLetter = true;
       if( argc>=7){ if( toupper(argv[6][0]) == 'N') NStf.NoFirstLetter=false;}
       NStf.Step = 0;
       if( argc>=8) NStf.Step = atoi(argv[7]);
       CtSt_.IsRingCh = true;
       if( argc>=9){ if( toupper(argv[8][0]) == 'Y') CtSt_.IsRingCh = false;}
       NStf.Start = 0;
       NStf.Stop  = 0;
       NStf.printname   = true;
       NStf.PrintEnerg  = true;
       NStf.StartSfrom0 = false;

       //ierr = ViewFunctionsSpCh( argv[3], &NStf );
       ierr = tabulateFunctionsSpCh( argv[3], &NStf );

       exit( ierr );
       // Print trajectory into a file
     case 'T':
       //fclose(fp);
       PTracStruct NSt;
       char OutputType;
       // Parameters set to default
       NSt.printname = true;
       NSt.Start     = 0;
       NSt.Stop      = 0;
       //parameters can be changed from the comand line
       strcpy(NSt.Filter, "*");
       if(argc>=5) strcpy(NSt.Filter, argv[4]);
       NSt.MatchCase = true;
       if( argc>=6){ if( toupper(argv[5][0]) == 'N') NSt.MatchCase=false;}
       ClsLat = true;
       NstTool.LinClosure  = true;
       NstTool.FullClosure = false;
       NstTool.Closure4D   = true;
       if( argc>=7){
         if( toupper(argv[6][0]) == 'N') ClsLat=false;
         if( toupper(argv[6][0]) == 'E'){
           NstTool.FullClosure = true;
           NstTool.LinClosure  = false;
         }
       }
       OutputType='T';
       if( argc>=8) OutputType = argv[7][0];
       NSt.Step = 0.0;
       if( argc>=9) NSt.Step = atof(argv[8]);
       ierr = TypeTrajectory( argv[3], &NSt, ClsLat, OutputType);
       exit(ierr);
       // Print trajectory into a file
   case 'O': // REFERENCE ORBIT 
     {
   
     // *** FIXME *** excited orbit case is currently ignored  

     if ( analyze( true,1) ) exit(1);  // to force reading xo0 etc .. 

     //fclose(fp);
     OrbitNStruct orbitns;
     orbitns.x             = xo0_; 
     orbitns.y             = yo0_;
     orbitns.z             = zo0_;			
     orbitns.tetax         = tetaXo0_;
     orbitns.tetay         = tetaYo0_;
     orbitns.s             = so0_;			
     orbitns.ds            = 0.0; //stepo;
     orbitns.StartFromFile = CtOr;         
     orbitns.MatchCase     = 'Y';
     orbitns.FirstLetter   = 'N'; 
     strcpy(orbitns.Filter, "*");

     //parameters that can be changed from the command line
      strcpy(orbitns.Filter, "*");
     if(argc>=5) {
       strcpy(orbitns.Filter, argv[4]);
     }
     
     orbitns.MatchCase = true;
     if( argc>=6){
       if( toupper(argv[5][0]) == 'N') orbitns.MatchCase = false;
     }

     orbitns.FirstLetter=false;
     if(argc>=7){
         if( toupper(argv[6][0]) == 'Y')  {
	   orbitns.FirstLetter = true;
         }
     }

     OutputType='T'; // text
     if( argc>=8) OutputType = argv[7][0];

     orbitns.ds = 0.0;
     if( argc>=9) orbitns.ds = atof(argv[8]);

     ierr = viewOrbit( argv[3], &orbitns, "" );  
     exit(ierr);
     }


   case 'R':
       if( argc<5) exit(1);
       strcpy(edtrdSt.Filter, "*");
       if( argc>=6) strcpy(edtrdSt.Filter, argv[5]);
       edtrdSt.MatchCase=true;
       if( argc>=7) {if( toupper(argv[6][0]) == 'N') edtrdSt.MatchCase=false;}
       edtrdSt.FirstLetter[0]=' ';    
       edtrdSt.FirstLetter[0] = 0;
       if( argc>=8) edtrdSt.FirstLetter[0]= argv[7][0];
       scale_read=1.0;
       if(argc>=9) scale_read=atof(argv[8]);
       edtrdSt.CompareOnly = false;
       if( cmdEditRead(argv[4])) exit(1);
       if( argc>=10){
         FILE* fp1 = fopen( argv[9], "r");
	 std::shared_ptr<FILE> sfp(fp1,std::fclose);
	 if(!fp1){
	   sprintf(buf,"Cannot open file: %s", argv[9]);
	   OptimMessageBox::warning(this, "Incorrect command line", buf, QMessageBox::Ok);
           exit(1);
      	 }
	 while(fgets(buf, 255, fp1) ){
	   if((*buf=='\n')||(*buf==0)) continue;
      	   sscanf(buf,"%d %s %le %le %le %le %le %le %le %le %le %le %le",
                       &i, buf1, &s, &v.BtX, &v.AlX, &v.BtY, &v.AlY,
                       &v.DsX, &v.DsXp, &v.DsY, &v.DsYp, &v.nuX, &v.nuY);
           if(strcmp("END", buf1)==0) break;
         }
         //fclose(fp1);
         if(strcmp("END", buf1) == 0) substituteBetas (v);
       }
       editor = qobject_cast<OptimEditor*>(LatticeCh_->widget());
       nln   = editor->document()->lineCount();
       for(int i =0; i<nln; ++i){
	 strcpy(buf, editor->document()->findBlockByLineNumber(i).text().toUtf8().data()); // GetLine(buf, 255, i);
         fprintf(fp,"%s\n", buf);
        }
      	exit(0);
      case 'W':
        {
	 if(analyzed_) { if(analyze(false , 1)) exit(0); } else { if( analyze( true, 1)) exit(0);}
         strcpy(NStLat_.Filter, "*");
	 if( argc>=5) strcpy(NStLat_.Filter, argv[4]);
         NStLat_.MatchCase=true;
    	 if(argc>=6){ if( toupper(argv[5][0]) == 'N')  NStLat_.MatchCase=false;}
         NStLat_.printFirstLetter=true;
    	 if(argc>=7){ if( toupper(argv[6][0]) == 'N') NStLat_.printFirstLetter=false;}
	 NStLat_.StartSFromFile=true;
    	 std::string names[nelm_];
         // Printing Burt Header
	 //--- Start BURT header
	 //Time:     Wed Mar 15 09:00:00 1996
	 //Login ID: fox (Tim L. Fox)
	 //Eff  UID: 944
	 //Group ID: 141
         //Keywords: Tunable Optics of Arc 1 by Lebedev
	 //Comments: Momentum = 445 MeV/c
	 //Type:     Absolute
	 //--- End BURT header
         fprintf(fp,"--- Start BURT header\n");
   	 t  = time(0);
   	 tm_str = localtime(&t);
         fprintf(fp,"Time:     %s", asctime(tm_str));
         len=80; GetUserNameLocal(buf,&len); buf[80]=0;
         fprintf(fp,"Login ID: %s\n", buf);
         fprintf(fp,"Eff  UID: 944\nGroup ID: 141\n");
         fprintf(fp,"Keywords: File was generated by OptiM code from %s\n", argv[2]);
         fprintf(fp,"Type:     Absolute\n");
         fprintf(fp,"--- End BURT header");
         // printing BdLs
	 for( i=nln=0; i<nelm_; ++i){
	   if(PrintBdL(i, names, nln, buf))continue;
           nln++;
	   fprintf(fp,"\n%s", buf);
	 }
	}
        default:
	exit(0);
   }
 } // argc >= 4
 return true;
}


/* Used letters: 	f - beta functions
						s - beta functions MAD like output
						c - coupled betas
                  i - coupled betas with space charge
                  t - beam trajectory
                  r - read BdL from .snap file crerate new Optim file
                  w - create .snap file
                  p - macro particle tracking
 ===============================
	Start program with input file
   	optim32 InputFileName
   ===============================
	Generate list of beta-functions
   	optim32 -f InputOptimFileName OutputTwissFileName <filter> <MatchCase> <HideFirstLetter> <Comment> <step> <CloseLattice>
      	defaults:
            filter=*
         	MatchCase=Y
            HideFirstLetter=Y
            Comment=#
            step=0
            CloseLattice=N
   ===============================
	Generate list of coupled beta-functions
   	optim32 -c InputOptimFileName OutputTwiss4DFileName <filter> <MatchCase> <HideFirstLetter> <step> <CloseLattice>
      	defaults:
            filter=*
         	MatchCase=Y
            HideFirstLetter=Y
            step=0
            CloseLattice=N
    ===============================
	Generate list of coupled beta-functions with beam space charge taken into account
   	optim32 -i InputOptimFileName OutputTwiss4DFileName <filter> <MatchCase> <HideFirstLetter> <step> <CloseLattice>
      	defaults:
            filter=*
         	MatchCase=Y
            HideFirstLetter=Y
            step=0
            CloseLattice=N
   ===============================
	Multiparticle tracking
   	optim32 -p InputOptimFileName OutputTrackingFileName InputPartPosFileName <filter> <MatchCase> <Nturn> <ring>
      	defaults:
            filter=*
         	MatchCase=Y
            Nturn=1
            ring=L (should be R or L)
   ===============================
	Output beam trajectory
   	optim32 -t InputOptimFileName OutputTrajFileName <filter> <MatchCase> <CloseLattice> <OutputType> <step>
      	defaults:
            filter=*
         	MatchCase=Y
            HideFirstLetter=Y
            CloseLattice=Y (N - No closure, Y - linear closure, E - nonlinear closure)
            OutputType=T (T - text, B - binary)
            step=0
   ===============================
	Read integral BdLs from .snap file and generate new .opt file
   	optim32 -r InputOptimFileName OutputOptimFileName InputSnapFile <filter> <MatchCase> <NamePrefix> <Scale> <StartTwissFile>
      	defaults:
            filter=*
         	MatchCase=Y
            NamePrefix=""
            Scale=1
         If StartTwissFile is used initial Twiss parameters will be taken from this file 
   ===============================
	Create .snap file
   	optim32 -w InputOptimFileName OutputSnapFileName <filter> <MatchCase> <HideFirstLetter>
      	defaults:
            filter=*
         	MatchCase=Y
            HideFirstLetter=Y
   ===============================
	To generate beta function list and Element parameters (MAD like structure):
		server -s InputOptimFileName OutputTwissFileName <filter> <MatchCase>
	                     <HideFirstLetter> <Comment> <step> <CloseLattice>
			Program puts Comment in a separate line at the beginning of output file
			Default values are:
				filter=*,  MatchCase=Y, HideFirstLetter=Y, Comment=#
				step=0,    CloseLattice=Y (if Y periodic solution will be used).
*/

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int OptimMainWindow::TrackOffLine(char *InputPartPosFile, char *TrackResFile,
				  bool MatchCase, char *filter, int nturn, char ring)
{
  auto con = Globals::preferences().con;

  char buf[LSTR+1];
  double dat[8];
  std::complex<double> ev[4][4];
  RMatrix me;
  RMatrix tm;
  Twiss twiss;
  BeamMoments history;

  FILE* fpr = fopen(InputPartPosFile,"r");
  FILE* fpw = fopen(TrackResFile,"w");
  
  if(!fpr){
    sprintf(buf, "Cannot open file %s to read particle coordinates",InputPartPosFile);
    OptimMessageBox::warning(this, "Tracking", buf, QMessageBox::Ok);
    return 1;
  }
  if(!fpw){
    sprintf(buf, "Cannot open file %s to write trackong results",TrackResFile);
    OptimMessageBox::warning(this, "Tracking", buf, QMessageBox::Ok);
    return 1;
  }

  char* status; 
  if(analyzed_) {
    if(analyze(false,1)) return 1;
  }
  else
    {if(analyze(true,1))return 1;
   }

   // Read particle coordinates
   status = fgets(buf, 255, fpr);
   if(!strcmpr("OptiM Track Data", buf)){
     sprintf(buf, "File <%s> is not an OptiM Track Data file",InputPartPosFile);
     OptimMessageBox::warning(this, "Tracking", buf, QMessageBox::Ok);
     return 1;
   }
   int i = 0;
   while(fgets(buf, 255, fpr) ){
     if(buf[0]=='#') continue;
     if(decodeExtLine(buf, dat, 6)==6) ++i;
   }
   fclose(fpr);
   if(i<1){
     sprintf(buf, "File %s has no particle information or corrapted structure",InputPartPosFile);
     OptimMessageBox::warning(this,"Tracking", buf,QMessageBox::Ok);
     return 1;
   }
   int N = i;

   OptimTrackerNew* tracker = 0;
   QMdiSubWindow* sw = mdiArea->addSubWindow( (tracker = new OptimTrackerNew(this)) ); 

   tracker->N_= N;
   tracker->vin_.resize(N);
   tracker->vfin_.resize(N);
   
   fpr=fopen(InputPartPosFile,"r");

   status = fgets(buf, 255, fpr);
   i=0;
   while(fgets(buf, 255, fpr) ){
     int k; 
     auto& particle = tracker->vin_[i];
     if(buf[0]=='#') continue;
        if( ( k = decodeExtLine(buf, dat, 8) ) >= 6) {
      	  for(int j=0; j<6; ++j) { particle.c[j]=dat[j]; }
	  particle.lost = 0;
   	  if( k >=7 ) particle.pid    = dat[6];  else particle.pid     =  i;
      	  if( k >=8 ) particle.weight = dat[7];  else particle.weight  =  1.;
         ++i;
        }
   } // while 
   fclose(fpr);

   // performs tracking

   double phi    = tetaYo0_ * (PI/180.0);
   double teta   = tetaXo0_ * (PI/180.0);

   double sp = sin(phi);
   double cp = cos(phi);

   double st = sin(teta);
   double ct = cos(teta);
  
   RMatrix_t<3> frame = {{ ct, 0.0, st}, { 0.0, 1.0, 0.0}, { -st, 0.0, ct}};

   double tetaY = tetaYo0_; //*** WARNING*** This is the lattice file initial value

   double Enr   = Ein;
   double gamma = 1.+Enr/ms;
   double dalfa = 0.0; 

   for(int j=0; j<N; ++j) {
     auto& particle_fin =  tracker->vfin_[j];
     auto& particle_in =   tracker->vfin_[j];
     // We start with final state == initial state 
     for(int i=0; i<6; ++i) {
       particle_fin[i] = particle_in[i];}
     }

   Bunch& v   = tracker->vfin_;

   twiss.BtX  = BetaXin;      twiss.BtY=BetaYin;
   twiss.AlX  = AlfaXin;      twiss.AlY=AlfaYin;
   twiss.DsX  = DispXin;      twiss.DsY=DispYin;
   twiss.DsXp = DispPrimeXin; twiss.DsYp=DispPrimeYin;
     
   if(ring == 'R' ){
      findRMatrix(tm);
      i = find_tunes(tm, 100., twiss, &dalfa);
      if(i){
	OptimMessageBox::warning(this, "Tracking", "Failed to find periodic solution for either X or Y", QMessageBox::Ok);
      	return 1;
      }
   }

   twiss.nuX =0.0;			 
   twiss.nuY =0.0;

   twiss.eigenvectors(ev);

   
   //tracker->getHistory(&history, gamma, twiss, v);
   // history.s=0.;
   fprintf(fpw, "#S[cm]_or_N \tname \temitX[cm] \temitY[cm] \tXmax[cm] \tYmax[cm] \tsigmaS[cm] \tcurrent[cm] \tXav[cm] \tYav[cm] \tSav[cm] \tPXav \tPYav \tPSav");
   //fprintf(fpw, "\n%f \t%s \t%e \t%e \t%e \t%e \t%e \t%e \t%e \t%e \t%e \t%e \t%e \t%e",
   //	   history.s, "Start", history.emitX(), history.emitY(), history.Xmax(), history.Ymax(), history.sigmaDP(),
   //	   history.intensity, history.Xav(), history.Yav(), history.Sav(), history.PXav(), history.PYav(), history.PSav());
   
   sqlite::execute(*con, "DELETE FROM Moments WHERE rowid IN (SELECT max(rowid) FROM moments);", true); 
   double spos = 0.0;
   
   if( nturn== 1) {  // INITIAL CONDITION FOR OUTPUT AT ALL ELEMENTS 
      BeamMoments mom(gamma,v, N, false);
      mom.s = spos;
      mom.dbWrite(*con, 0, 0, N); // NOTE: i is the element index
   }
   
   for(int k=0; k<nturn; ++k) {
      for(int i=0; i<nelm_; ++i) {
   	auto  ep = beamline_[i];
      	char nm     = toupper(ep->name()[0]);
        double EnrNew = Enr;
        me = ep->rmatrix(EnrNew, ms, tetaY, 0.0 ,3);
        ep->propagateLatticeFunctions(me, twiss, ev);
        tracker->trackBunchExact(ep.get(), Enr, frame, v, N, k+1, i);

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
	spos += ep->length()*0.01;

	if(nturn == 1) {  //OUTPUT AT ALL ELEMENTS
          BeamMoments mom(gamma,v, N, false);
	  mom.s = spos;
          mom.dbWrite(*con, k, i+1, N); // NOTE: i is the element index
        }
	/*  else {
	  if( elm_selection_.find(i) != elm_selection_.end()) { 
	    // (view_at_elem_>0)&&(view_at_elem_== i)) { // OUTPUT AT SPECIFIC ELEMENT(S)
             BeamMoments mom(gamma,v, N, false);
	     mom.s = spos;
	     mom.dbWrite(*con, k, i, N);
	     v.lossProfile();
	  }   
	*/
 
      }
   }

 
/* OLD CODE 
   tracker->getHistory(&history, gamma, twiss, v);
   history.s=0.;
   fprintf(fpw, "#S[cm]_or_N \tname \temitX[cm] \temitY[cm] \tXmax[cm] \tYmax[cm] \tsigmaS[cm] \tcurrent[cm] \tXav[cm] \tYav[cm] \tSav[cm] \tPXav \tPYav \tPSav");
   fprintf(fpw, "\n%f \t%s \t%e \t%e \t%e \t%e \t%e \t%e \t%e \t%e \t%e \t%e \t%e \t%e",
	   history.s, "Start", history.emitX(), history.emitY(), history.Xmax(), history.Ymax(), history.sigmaDP(),
	   history.intensity, history.Xav(), history.Yav(), history.Sav(), history.PXav(), history.PYav(), history.PSav());
   
   for(int k=0; k<nturn; ++k){
      for(int i=0; i<nelm_; ++i){
   	auto  ep = beamline_[i];
      	char nm     = toupper(ep->name()[0]);
        double EnrNew = Enr;
        me = ep->rmatrix(EnrNew, ms, tetaY, 0.0 ,3);
        ep->propagateLatticeFunctions(me, twiss, ev);
        tracker->trackParticleExact(ep.get(), Enr, tetaY, v, N, k+1, i);
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
        if(nturn==1)history.s=history.s+ep->length()*0.01; else history.s=k;
      	if(filterName(ep->name(), filter, MatchCase)){
          tracker->getHistory(&history, gamma, twiss, v);
	  fprintf(fpw, "\n%f \t%s \t%e \t%e \t%e \t%e \t%e \t%e \t%e \t%e \t%e \t%e \t%e \t%e",
		  history.s, ep->name(), history.emitX(), history.emitY(), history.Xmax(), history.Ymax(), history.sigmaDP(),
		  history.intensity, history.Xav(), history.Yav(), history.Sav(), history.PXav(), history.PYav(), history.PSav());
        }
     }
   }

   ***/  
   fclose(fpw);
   return 0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int OptimMainWindow::ViewMachine(char* filenm, FunctionDlgStruct* NStf, char* comment, bool ClsLat)
{
  Twiss v;
  RMatrix tm;
  char const* pname;
  double dalfa, e1, e2,Hrt;

  if(interrupted_){ interrupted_= false; return 1;}
  if(analyzed_) {if(analyze(false,1)) return 1; } else {if(analyze(true,1)) return 1;}
  
  strcpy(funcFilter, NStf->Filter);
  NoFirstLetter = NStf->NoFirstLetter;
  MatchCase     = NStf->MatchCase;

  v.BtX  = BetaXin;
  v.BtY  = BetaYin;
  v.AlX  = AlfaXin;
  v.AlY  = AlfaYin;
  v.DsX  = DispXin;
  v.DsY  = DispYin;
  v.DsXp = DispPrimeXin;
  v.DsYp = DispPrimeYin;
  v.nuX  = QXin;
  v.nuY  = QYin;
  
  if(ClsLat){
    findRMatrix(tm);
    int istatus = find_tunes(tm, 100., v, &dalfa);
    if(istatus){
      OptimMessageBox::warning(this,"Close Error", "Cannot close for X or Y", QMessageBox::Ok);
      return 1;
    }
    v.nuY = v.nuX = 0.0;
  }
   
  double Lp    = 0.0;
  double tetaY = tetaYo0_; // ***WARNING*** This is the lattice file initial value.
  double Enr   = Ein;

  char const* fln1    = "#N name  s[m]  btx[m]  alx  Qx  Dx[m]  Dxp  bty[m]  aly  Qy  Dy[m]  Dyp ";
  char const* format1 = "%d %s %le %le %le %le %le %le %le %le %le %le %le ";
  char const* fln2    = "type  L[m]  ang[rad]  k1[m^-2]  k2[m^-3]  tilt[rad]  e1[rad]  e2[rad]  Energy[MeV]\n";
  char const* format2 = "%s %le %le %le %le %le %le %le %le\n";

  std::complex<double> ev[4][4];

  FILE* fp = fopen(filenm,"w");
  if (!fp ){
    OptimMessageBox::warning(this, "Error opening file for saving.", filenm, QMessageBox::Ok);
    return 1;
  }

 
 if( strlen(comment)>1) fprintf(fp,"#%s\n", comment);

 fprintf(fp,"%s ",fln1);
 fprintf(fp,"%s ",fln2);
 fprintf(fp,format1, 0, "START", Lp*0.01, v.BtX*0.01, v.AlX, v.nuX, v.DsX*0.01, v.DsXp,
   		     v.BtY*0.01, v.AlY, v.nuY, v.DsY*0.01, v.DsYp);
 fprintf(fp,format2, "MARK", 0., 0., 0., 0., 0., 0., 0., Enr);
 v.eigenvectors(ev);
 
 int i=0;
 for( ; i<nelm_; ++i){

   auto ep = beamline_[i];
   
   tm = ep->rmatrix(Enr, ms, tetaY, 0, 3);

    ep->propagateLatticeFunctions(tm, v, ev);
    
    Lp +=  ep->length();
    //

    if( toupper(ep->name()[0])=='G')continue;

    if(!filterName( ep->fullName(), funcFilter, MatchCase))continue;

    if( NoFirstLetter) {
      if( ((ep->name()[1]>='0') && (ep->name()[1]<='9')) ||(strlen(ep->name())==1)) {
	pname = ep->fullName();
      }
      else {
	pname= &ep->fullName()[1];
      }
    } else pname = &ep->fullName()[1];

    fprintf(fp,format1, i+1, pname, Lp*0.01, v.BtX*0.01, v.AlX, v.nuX, v.DsX*0.01, v.DsXp,
   		        v.BtY*0.01, v.AlY, v.nuY, v.DsY*0.01, v.DsYp);
    Hrt=sqrt(2.*ms*Enr+Enr*Enr)/0.2997925;

    switch(ep->name()[0]){
      case 'B': 
      case 'D':  
      case 'b': 
      case 'd':
        e1=0.; 
        e2=0.;
        if(i>0 && i<nelm_-1){
          if( beamline_[i-1]->etype() =='G') e1=PI/180.*beamline_[i-1]->G;
          if( beamline_[i+1]->etype() =='G') e2=PI/180.*beamline_[i+1]->G;
        }
        else{
          if(i==nelm_-1){
            if( beamline_[i-1]->etype() =='G') e1=PI/180.*beamline_[i-1]->G;
        }
          if(i==0){
            if( beamline_[i+1]->etype() =='G') e2=PI/180.*beamline_[i+1]->G;
          }
        }
        fprintf(fp,format2, "SBEND",ep->length()*0.01, ep->B*ep->length()/Hrt, 10000.*ep->G/Hrt, 0., PI/180.*ep->tilt(), e1, e2, Enr);
        break;
      case 'Q':  
      case 'q':
	fprintf(fp,format2, "QUAD", ep->length()*0.01, 0., 10000.*ep->G/Hrt, 0., PI/180.*ep->tilt(), 0., 0., Enr);
      break;
      case 'S':  
      case 's':
	fprintf(fp,format2, "SEXT", ep->length()*0.01, 0., 0., 1000000.*ep->G/Hrt, 0., 0., 0., Enr);
        break;
      case 'i':
	fprintf(fp,format2, "MONT", ep->length()*0.01, 0., 0., 0., 0., 0., 0., Enr);
        break;
      case 'I':
	fprintf(fp,format2, "MARK", ep->length()*0.01, 0., 0., 0., 0., 0., 0., Enr);
        break;
      case 'K': 
      case 'k':
	fprintf(fp,format2, "KICK", ep->length()*0.01, 0., 0., 0., PI/180.*ep->tilt(), 0., 0., Enr);
        break;
      case 'O': 
      case 'o': 
       default:
	fprintf(fp,format2, "DRIFT", ep->length()*0.01, 0., 0., 0., 0., 0., 0., Enr);
	break;
      }
 }
    fprintf(fp,format1, i+1, "END", Lp*0.01, v.BtX*0.01, v.AlX, v.nuX, v.DsX*0.01, v.DsXp,
   				    v.BtY*0.01, v.AlY, v.nuY, v.DsY*0.01, v.DsYp);
    fprintf(fp,format2, "MARK", 0., 0., 0., 0., 0., 0., 0., Enr);

    fclose(fp);
    return 0;
}
