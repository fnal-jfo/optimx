//  =================================================================
//
//  JetColorMap.cpp
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
#include <JetColorMap.h>
#include <cmath>
#include <iostream>

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


JetLinearColorMap::JetLinearColorMap(QwtColorMap::Format f)
  : QwtColorMap (f), step_(0.125)
  {

   // this the Matlab Jet Color Map
   // Ref: https://stackoverflow.com/questions/7706339/grayscale-to-red-green-blue-matlab-jet-color-scale

    v_.push_back({0.0,     0,    0,    127}); // #00007F: dark blue
    v_.push_back({0.125,   0,    0,     255}); // #0000FF: blue
    v_.push_back({0.250,   0,    127,   255}); // #007FFF: azure
    v_.push_back({0.375,   0,    255,  255} ); // #00FFFF: cyan
    v_.push_back({0.500,   127,  255,  127} ); // #7FFF7F: light green
    v_.push_back({0.625,   255,  255,  0 }  ); // #FFFF00: yellow
    v_.push_back({0.750,   255,  127,  0}  ); // #FF7F00: orange
    v_.push_back({0.875,   255,  0,    0}  ); // #FF0000: red
    v_.push_back({1.0,     127,  0,    0}  ); // #7F0000: dark red   
  }  

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetLinearColorMap::~JetLinearColorMap()
  {}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

QRgb JetLinearColorMap::rgb( QwtInterval const& interval, double value) const
  {

    double vmin = interval.minValue();
    double vmax = interval.maxValue();

    if (value < vmin) value = vmin;
    if (value > vmax) value = vmax;
    
    double s = (value-vmin)/(vmax-vmin);
    
    
    //int i= 0;
    //for ( ; i<8; ++i) {
    //  if(  (s >= v_[i].s) and (s <= v_[i+1].s)) break; 
    //} 

    int i =  floor(s/step_);

    if (i==8) return qRgb(v_[i].r, v_[i].g, v_[i].b);

    //if (i <0 || i >= 8) {
    //  std::cout << " s = " << s << std::endl;
    //  std::cout << " step_ = " << step_ << std::endl;
    //  std::cout << " i = " << i << std::endl;
    //  exit(1);
    // }
    
    double const  deltas = step_; // (v_[i+1].s - v_[i].s);
    double          frac = (s-v_[i].s)/deltas;
    
    auto r = v_[i].r + (v_[i+1].r -v_[i].r) * frac;
    auto g = v_[i].g + (v_[i+1].g -v_[i].g) * frac;
    auto b = v_[i].b + (v_[i+1].b -v_[i].b) * frac;

    return qRgb(r,g,b);
    
  }


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetLogColorMap::JetLogColorMap(QwtColorMap::Format f)
    : JetLinearColorMap(f)
  {} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetLogColorMap::~JetLogColorMap()
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

QRgb JetLogColorMap::rgb( QwtInterval const& interval, double value) const
{

    double vmin = interval.minValue();
    double vmax = interval.maxValue();

    if (value < vmin) value = vmin;
    if (value > vmax) value = vmax;
    
    double s = (value-vmin)/(vmax-vmin);
    
    auto val = JetLinearColorMap::rgb(QwtInterval(std::log10(interval.minValue()),
				                  std::log10(interval.maxValue())),
				                  std::log10(value));

    return val;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// QwtLinearColorMap appears to have an interpolation bug, probably related
// to integer arithmetic. For this reason, the code below is disabled. 

/**********************************************************************************

JetColorMap::JetColorMap(QwtColorMap::Format f)
    : QwtLinearColorMap(f)
{

  setMode(ScaledColors);

  // this the Matlab Jet Color Map
  // Ref: https://stackoverflow.com/questions/7706339/grayscale-to-red-green-blue-matlab-jet-color-scale

  addColorStop(0.0  ,   QColor(0,    0,    127) ); // #00007F: dark blue
  addColorStop(0.125,   QColor(0,    0,    255) ); // #0000FF: blue
  addColorStop(0.250,   QColor(0,    127,  255) ); // #007FFF: azure
  addColorStop(0.375,   QColor(0,    255,  255) ); // #00FFFF: cyan
  addColorStop(0.500,   QColor(127,  255,  127) ); // #7FFF7F: light green
  addColorStop(0.625,   QColor(255,  255,  0 )  ); // #FFFF00: yellow
  addColorStop(0.750,   QColor(255,  127,  0)   ); // #FF7F00: orange
  addColorStop(0.875,   QColor(255,  0,    0)   ); // #FF0000: red
  addColorStop(0.999,   QColor(127,  0,    0)   ); // #7F0000: dark red 
  addColorStop(1.0,     QColor(127,  0,    0)   ); // #7F0000: dark red 
  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetColorMap::~JetColorMap()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

QRgb JetColorMap::rgb( QwtInterval const& interval, double value) const
{
  value  = std::max(value,interval.minValue());
  auto val = QwtLinearColorMap::rgb(QwtInterval(interval.minValue(),
				          interval.maxValue()), value );

  return val;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


JetLogColorMap::JetLogColorMap(QwtColorMap::Format f)
    : JetColorMap(f)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetLogColorMap::~JetLogColorMap()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
QRgb JetLogColorMap::rgb( QwtInterval const& interval, double value) const
{
  value  = std::max(value,interval.minValue());
  auto val = JetColorMap::rgb(QwtInterval(std::log10(interval.minValue()),
				      std::log10(interval.maxValue())), std::log10(value));

  return val;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

**********************************************************************************/
