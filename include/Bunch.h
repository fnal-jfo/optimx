//  =================================================================
//
//  Bunch.h
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

#ifndef BUNCH_H
#define BUNCH_H

#include <vector>
#include <Coordinates.h>

class Bunch {

 public:

  // NOTE: for now the lost_particles_ array is NOT used.
  //       in the future, it will store lost particles, and the Coordinates::lost attribute will be eliminated. 

  typedef std::vector<Coordinates>::iterator iterator;  
  typedef std::vector<Coordinates>::const_iterator const_iterator;  

  Bunch();
  Bunch(std::initializer_list<Coordinates> lst); // initialize a bunch from a container of Coordinates 

  Coordinates const& operator[](int i) const { return particles_[i]; } 
  Coordinates&       operator[](int i)       { return particles_[i]; } 

  std::vector<int>& lossProfile();  

  int    size()     const;      // to TOTAL no of particles
  int    nlost()    const;      // the no of lost particles

  void resize(unsigned int n)        { lost_particles_.resize(0); particles_.resize(n); }

  iterator begin() {return particles_.begin(); } 
  iterator end()   {return particles_.end(); } 

  const_iterator cbegin() { return particles_.cbegin(); } 
  const_iterator   cend() { return particles_.cend(); } 

  
private:
  
  std::vector<Coordinates>      particles_;   
  std::vector<Coordinates> lost_particles_;   
  std::vector<int>           loss_profile_; 
};


#endif // BUNCH_H
