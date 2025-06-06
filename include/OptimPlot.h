//  =================================================================
//
//  OptimPlot.h
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

#ifndef  OPTIMPLOT_H
#define  OPTIMPLOT_H

#include <QwtPlot>
#include <QwtPlotCurve>
#include <QwtSymbol>

#include <LegoData.h>
#include <PlotOptionsDialog.h>

#include <vector>
#include <QPointer>
#include <QPainter>
#include <QString>
#include <QDateTime>

#include <vector>
#include <list>
#include <string>
#include <memory>
#include <variant>
#include <functional>

class OptimLegoPlotItem;
class OptimPlotZoomer;
class QwtPlotPicker;
class QFileInfo;
class PlotOptionsDialog;
class MeasuredDataDialog;
class EditMeasuredDataDialog;
class QwtPlotTextLabel;
class DistancePicker;
class QwtScaleWidget;
class QwtLegend;
class QwtPlotGrid;

struct CurveSpec { 
  std::string  legendname;
  double       const*  xv; 
  double       const*  yv; 
  int          n; 
  QwtSymbol::Style symbol; 
  
  QwtPlot::Axis axis;
  //  union vertical_title_t { 
  //   char const*  left;
  //  char const*  right;
  //} vertical_title;
  std::string   vertical_title;
  std::string   color; 
  //  ~CurveSpec() {}
};


struct PlotSpec { 
  std::string title;
  std::string bottom_title;
  std::list<CurveSpec> curvespecs;
};

namespace sqlite {
  class connection;
}

struct DBSpecs { 
  std::string              dbfile;
  std::vector<std::string> queries;
  void (*init_custom_fncts)(sqlite::connection& con);
  DBSpecs() : init_custom_fncts(0) {} 
};

struct CurveState { 
  std::string title;
  bool        visible;
};

namespace sqlite {
  class connection;
}

class QwtPlotTextLabel;

class OptimPlot: public QwtPlot { 

 Q_OBJECT
 Q_PROPERTY( int compact); 

 public:

  OptimPlot( QWidget* parent=0, std::vector<LegoData> const& = std::vector<LegoData>());

  virtual ~OptimPlot();

  void       configure( PlotSpec const& plotspec, DBSpecs const& = DBSpecs());
  void configureCurves( PlotSpec const& plotspec, bool clearplot, DBSpecs const& = DBSpecs());

  void  initialize(OptimPlot const* p); 

  QSize minimumSizeHint(); 
  void  replot();
  void  createLego( std::vector<LegoData> const& legodata );
  void  setLegoData( std::vector<LegoData> const& ); 
  void  setLegoVisible( bool enable);
  bool  legoIsVisible() const; 
 
  // char* axs[2];  // FIXME ! 

  void saveAs();
  void saveAsText(QString const& filename);
  void clear();
  void setAxesTitleFont(QFont const& font);
  void setPlotTitleFont(QFont const& font);
  void setPlotTitle(QString const& s);

  void usePlotState(OptimPlot const* p);

  PlotOptions const& getPlotOpts() const;
  void setPlotOpts(PlotOptions const& opts);

  void removeDuplicateCurves();

  void setOptions();
  void setPreferences();

  public slots:

    void  enableCurve( QVariant const& itemInfo, bool on, int index);
    void  enableCurve  ( QVariant const &itemInfo, int index);
    void  resetZoomBase( QwtPlotItem* item, bool  on);
    void  plotIsZoomed(  QRectF const&);
    void  options(); 
    void  addMeasuredData(); 
    void  editMeasuredData(); 
    void  fixedLimits(  QwtPlot::Axis axisid);
    void  setDisplayStyle(int flags);
    void  copyToClipboard();
   

 protected:

  bool event(QEvent *e); 
  void resizeEvent(QResizeEvent* e);

 private:
 
  void               ctor( QWidget* parent, std::vector<LegoData> const& legodata);
  void          showCurve( QwtPlotItem *item, bool on );
  
  QwtPlotItem*  findPlotItemByName( QString const& title);
  QVariant      findLegendItemInfoByName(QString const& title);


  void showLegoPlot();

  OptimLegoPlotItem* lego_; 

  DistancePicker* distance_picker_left_;
  DistancePicker* distance_picker_right_;

  OptimPlotZoomer*          zoomer1_;
  OptimPlotZoomer*          zoomer2_;
  QwtPlotPicker*            tracker1_;
  QwtPlotPicker*            tracker2_;

  QPointer<PlotOptionsDialog>        opts_dialog_;               // plot specific instance
  QPointer<MeasuredDataDialog>       measured_data_dialog_;      // plot specific instance
  QPointer<EditMeasuredDataDialog>   edit_measured_data_dialog_; // plot specific instance

  std::vector<CurveState>            curve_states_;

  sqlite::connection*                con_; 
  
  //------------------
  // external data ... 
  // -----------------

  int ndata_;                    // number of external data files (arbitrarily limited to 8) 

  int                            len_data_;     // number of columns // [NUMBER_OF_EXTERN_DATA]  
  int                            color_data_;  // color code // [NUMBER_OF_EXTERN_DATA];
  int                            cross_data_;   // symbol code

  int                            nc_ext_;
  int                            dcol_m_;
  int                            n_scraper_;  

  std::string                    name_ext_;
  double                         scale_;
  std::vector<double>            Xext_;
  std::vector<double>            Yext_;
  std::vector<double>            YextM_;
  std::vector<double>            YextP_;
  double                         Xoffs_;
  std::vector<std::string>       elmNames_;

  std::vector<double>            scraperS_;
  std::vector<double>            scraperPos_; // scraperPos[2];

  static std::vector<QwtSymbol::Style>    symbols_;
  static std::vector<std::string>         colors_;

  QwtPlotTextLabel*                       data_label_;
  QString                                 datatxt_;
  QDateTime                               datetime_;
  
 signals:

  void detach();
  void attach();

 private:
  
  PlotOptions plot_opts_; 

  QwtPlotGrid*  grid_;
  QwtLegend*    legend_;

  QwtPlotTextLabel* tsitem_; // the timestamp plot item 
  
  // this is used for measured data (KLUDGE ! fixme !!!)   

  bool    mdata_show_legend_;
  bool    mdata_show_full_path_;
  bool    mdata_data_only_;
  
};



#endif // OPTIMPLOT_H
