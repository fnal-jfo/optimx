//  =================================================================
//
//  ScientificDoubleSpinBox.cpp
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
#include <ScientificDoubleSpinBox.h>
#include <QRegExp>
#include <QLineEdit>
#include <set>
#include <algorithm>
#include <iostream>
#include <QRegularExpression>
#include <cmath>
#include <cfloat>

// --------------------------------------------------------------------
// A regular expression for floating point numbers in scientific notation.
// Match groups are the whole string, the whole coefficient (mantissa),
// the decimal part of the coefficient, and the exponent part.
//--------------------------------------------------------------------

//QRegExp float_re( R"(([+-]?\d+(\.\d*)?|\.\d+)([eE][+-]?\d+)?)");
QRegularExpression float_re( R"((?<mantissa>[-]?((\d+(\.\d*)?)|(\.\d+)))([eE](?<exponent>[+-]?\d+))?)");


bool  ScientificValidator::valid_float_string(QString& s)
{

#if 0
  bool ematch = float_re.exactMatch(s);
  return (ematch > 0); 
#endif
  
  auto ematch = float_re.match(s);
  return ematch.hasMatch(); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ScientificValidator:: ScientificValidator(QObject* parent)
  : QValidator(parent)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


QValidator::State ScientificValidator::validate(QString& s, int& position) const
{
  static std::set<char> intermediate {'E','e', '.','-','+'};

  if ( valid_float_string(s) ) {
     return State::Acceptable;
  };

  if ( position == 0) return State::Intermediate;

  if  ( std::find( intermediate.begin(), intermediate.end(),  s[position-1]) 
       != intermediate.end() ) {
       return State::Intermediate;
  };

  return State::Invalid;
}

  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ScientificValidator::fixup(QString& input) const
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ScientificDoubleSpinBox::ScientificDoubleSpinBox( QWidget* parent)
  : QDoubleSpinBox(parent), validator_(new ScientificValidator(this))
{
  setMinimum(-1.0e99);
  setMaximum(1.0e99);
  setDecimals(DBL_MAX_10_EXP + DBL_DIG); // from <cfloat>
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

QValidator::State ScientificDoubleSpinBox::validate(QString& text, int& position) const
{
  return validator_->validate(text, position);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ScientificDoubleSpinBox::fixup(QString& input) const
{
  return validator_->fixup(input);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double ScientificDoubleSpinBox::valueFromText(QString& input) const
{
  double value = input.toDouble();
  return value;
} 


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

QString ScientificDoubleSpinBox::textFromValue( double value) const
{
  QString s("");
  s.setNum(value, 'g', 6);
  return s;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ScientificDoubleSpinBox::stepBy(int steps)
{
#if  0
  // ***** original code ****** 
  // should really be advancing by steps*delta
  
  QString s        = cleanText(); 
  int groups       = float_re.exactMatch(s);
  QStringList list = float_re.capturedTexts();

  double decimal   = list[1].toDouble();
  decimal += steps;   // increment the decimal part
  QString ns("");
  ns.setNum(decimal);

  ns += list[3];      // list[3] is the fractional part (including the period)
  lineEdit()->setText(ns);
#endif
  //------------------------------------------------------------------------------
  
  QString s                     = cleanText(); 
  QRegularExpressionMatch match = float_re.match(s);

  //std::cout <<  "mantissa = "<<  match.captured("mantissa").toUtf8().data() << std::endl;
  //std::cout <<  "exponent = "<<  match.captured("exponent").toUtf8().data() << std::endl;
  
  double mantissa  = match.captured("mantissa").toDouble();
  double value     = mantissa + steps;   // increment the decimal part
  //s =  match.captured("exponent").toInt();
  s =  match.captured("exponent");
  int exponent  = (s == "") ? 0 : match.captured("exponent").toInt();
  
  value *= pow(10,exponent);
  
  QString ns("");
  ns.setNum(value);

  lineEdit()->setText(ns);

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


QString ScientificDoubleSpinBox::format_float(double value)
{
  // Modified form of the 'g' format specifier.

  // QString s("{:g}").format(value).replace("e+", "e");
  // s = re.sub("e(-?)0*(\d+)", r"e\1\2", s);
  // return string;

  QString s("");
  s.setNum(value, 'g', 6);  
  return s;  
}

